/**
 * altera_pcie.c - Driver PCIe Terasic/Intel FPGA
 * Compativel com Linux Kernel 6.8 (Ubuntu 24.04)
 *
 * CORRECAO APLICADA:
 * Adicionado [] em altera_pcie_ids para definir corretamente como ARRAY.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/pci.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/ioctl.h>
#include <linux/version.h>
#include <linux/slab.h> // Necessário para kzalloc/kfree

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Altera PCIe Driver (Fixed for Kernel 6.8)");
MODULE_AUTHOR("Terasic - Patched");

#define DRIVER_NAME "altera_pcie"
#define ALTERA_PCIE_DRIVER_MAGIC 'A'

// --- DEFINICAO DA ESTRUTURA ---
struct altera_ioctl_arg {
    unsigned int bar_num;
    unsigned int offset;
    unsigned int data;
};

#define ALTERA_IOCX_READ_CONF   _IOWR(ALTERA_PCIE_DRIVER_MAGIC, 1, struct altera_ioctl_arg)
#define ALTERA_IOCX_WRITE_CONF  _IOWR(ALTERA_PCIE_DRIVER_MAGIC, 2, struct altera_ioctl_arg)
#define ALTERA_IOCX_READ_MEM    _IOWR(ALTERA_PCIE_DRIVER_MAGIC, 3, struct altera_ioctl_arg)
#define ALTERA_IOCX_WRITE_MEM   _IOWR(ALTERA_PCIE_DRIVER_MAGIC, 4, struct altera_ioctl_arg)

// --- ESTRUTURA DO DISPOSITIVO ---
struct altera_pcie_dev {
    struct pci_dev *pci_dev;
    struct cdev cdev;
    dev_t devt;
    void __iomem *bar[6]; // Corrigido para array de 6 posições para segurança
    size_t bar_len[6];
    int irq;
};

static struct altera_pcie_dev *my_dev;
static struct class *altera_pcie_class;

// --- PROTOTIPOS ---
static int __init altera_pcie_init(void);
static void __exit altera_pcie_exit(void);

// --- FUNCOES DO ARQUIVO ---

static int altera_pcie_open(struct inode *inode, struct file *file) {
    file->private_data = my_dev;
    return 0;
}

static int altera_pcie_release(struct inode *inode, struct file *file) {
    return 0;
}

static long altera_pcie_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct altera_pcie_dev *dev = file->private_data;
    struct altera_ioctl_arg data;
    uint32_t val;

    if (_IOC_TYPE(cmd) != ALTERA_PCIE_DRIVER_MAGIC)
        return -ENOTTY;

    // access_ok (verificação de memória do usuário)
    if (!access_ok((void __user *)arg, _IOC_SIZE(cmd)))
        return -EFAULT;

    if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
        return -EFAULT;

    switch (cmd) {
        case ALTERA_IOCX_READ_CONF:
            pci_read_config_dword(dev->pci_dev, data.offset, &val);
            data.data = val;
            break;
        case ALTERA_IOCX_WRITE_CONF:
            pci_write_config_dword(dev->pci_dev, data.offset, data.data);
            break;
        case ALTERA_IOCX_READ_MEM:
            if (data.bar_num > 5 || !dev->bar[data.bar_num]) return -EINVAL;
            data.data = ioread32(dev->bar[data.bar_num] + data.offset);
            break;
        case ALTERA_IOCX_WRITE_MEM:
            if (data.bar_num > 5 || !dev->bar[data.bar_num]) return -EINVAL;
            iowrite32(data.data, dev->bar[data.bar_num] + data.offset);
            wmb(); // Memory Barrier
            break;
        default:
            return -ENOTTY;
    }

    if (copy_to_user((void __user *)arg, &data, sizeof(data)))
        return -EFAULT;

    return 0;
}

static const struct file_operations altera_pcie_fops = {
  .owner          = THIS_MODULE,
  .open           = altera_pcie_open,
  .release        = altera_pcie_release,
  .unlocked_ioctl = altera_pcie_ioctl,
};

// --- PCI DRIVER ---

// CORRECAO AQUI: Adicionado [] para indicar que é um array
static const struct pci_device_id altera_pcie_ids[] = {
    { PCI_DEVICE(0x1172, 0xE001) },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, altera_pcie_ids);

static int altera_pcie_probe(struct pci_dev *dev, const struct pci_device_id *id) {
    int rc = 0, i;

    printk(KERN_INFO "altera_pcie: Device found (Vendor: %04x, Device: %04x)\n", dev->vendor, dev->device);

    my_dev = kzalloc(sizeof(struct altera_pcie_dev), GFP_KERNEL);
    if (!my_dev) return -ENOMEM;

    my_dev->pci_dev = dev;
    pci_set_drvdata(dev, my_dev);

    rc = pci_enable_device(dev);
    if (rc) {
        printk(KERN_ERR "altera_pcie: Failed to enable device.\n");
        goto err_enable;
    }

    pci_set_master(dev);
    rc = pci_request_regions(dev, DRIVER_NAME);
    if (rc) goto err_regions;

    for (i = 0; i < 6; i++) {
        unsigned long len = pci_resource_len(dev, i);
        unsigned long flags = pci_resource_flags(dev, i);
        if (len > 0 && (flags & IORESOURCE_MEM)) {
            my_dev->bar[i] = pci_iomap(dev, i, 0);
            my_dev->bar_len[i] = len;
            printk(KERN_INFO "altera_pcie: BAR%d mapped at %p (length %lu)\n", i, my_dev->bar[i], len);
        }
    }

    rc = alloc_chrdev_region(&my_dev->devt, 0, 1, DRIVER_NAME);
    if (rc < 0) goto err_cdev;

    cdev_init(&my_dev->cdev, &altera_pcie_fops);
    rc = cdev_add(&my_dev->cdev, my_dev->devt, 1);
    if (rc < 0) goto err_cdev_add;

    // Kernel 6.4+ removeu o parametro THIS_MODULE de class_create
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    altera_pcie_class = class_create(DRIVER_NAME);
#else
    altera_pcie_class = class_create(THIS_MODULE, DRIVER_NAME);
#endif

    if (IS_ERR(altera_pcie_class)) {
        printk(KERN_ERR "altera_pcie: Failed to create class.\n");
        rc = PTR_ERR(altera_pcie_class);
        goto err_class;
    }

    device_create(altera_pcie_class, NULL, my_dev->devt, NULL, DRIVER_NAME);
    printk(KERN_INFO "altera_pcie: Driver initialized successfully.\n");
    return 0;

err_class:
    cdev_del(&my_dev->cdev);
err_cdev_add:
    unregister_chrdev_region(my_dev->devt, 1);
err_cdev:
    for (i = 0; i < 6; i++) if (my_dev->bar[i]) pci_iounmap(dev, my_dev->bar[i]);
    pci_release_regions(dev);
err_regions:
    pci_disable_device(dev);
err_enable:
    kfree(my_dev);
    return rc;
}

static void altera_pcie_remove(struct pci_dev *dev) {
    struct altera_pcie_dev *ade = pci_get_drvdata(dev);
    int i;

    if (altera_pcie_class) {
        device_destroy(altera_pcie_class, ade->devt);
        class_destroy(altera_pcie_class);
    }

    cdev_del(&ade->cdev);
    unregister_chrdev_region(ade->devt, 1);

    for (i = 0; i < 6; i++) {
        if (ade->bar[i]) {
            pci_iounmap(dev, ade->bar[i]);
        }
    }

    pci_release_regions(dev);
    pci_disable_device(dev);
    kfree(ade);
    printk(KERN_INFO "altera_pcie: Driver removed.\n");
}

static struct pci_driver altera_pcie_driver = {
 .name       = DRIVER_NAME,
 .id_table   = altera_pcie_ids,
 .probe      = altera_pcie_probe,
 .remove     = altera_pcie_remove,
};

static int __init altera_pcie_init(void) {
    return pci_register_driver(&altera_pcie_driver);
}

static void __exit altera_pcie_exit(void) {
    pci_unregister_driver(&altera_pcie_driver);
}

module_init(altera_pcie_init);
module_exit(altera_pcie_exit);

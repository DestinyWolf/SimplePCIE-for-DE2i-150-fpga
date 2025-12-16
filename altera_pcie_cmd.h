#ifndef _ALTERA_PCIE_CMD_H
#define _ALTERA_PCIE_CMD_H

#include <linux/ioctl.h>

#define ALTERA_PCIE_DRIVER_MAGIC 'A'

typedef struct {
    unsigned int bar_num;
    unsigned int offset;
    unsigned int data;
} altera_ioctl_arg;

#define ALTERA_IOCX_READ_CONF   _IOWR(ALTERA_PCIE_DRIVER_MAGIC, 1, altera_ioctl_arg)
#define ALTERA_IOCX_WRITE_CONF  _IOWR(ALTERA_PCIE_DRIVER_MAGIC, 2, altera_ioctl_arg)
#define ALTERA_IOCX_READ_MEM    _IOWR(ALTERA_PCIE_DRIVER_MAGIC, 3, altera_ioctl_arg)
#define ALTERA_IOCX_WRITE_MEM   _IOWR(ALTERA_PCIE_DRIVER_MAGIC, 4, altera_ioctl_arg)

#endif

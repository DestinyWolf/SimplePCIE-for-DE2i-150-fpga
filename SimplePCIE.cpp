#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include "SimplePCIE.h"

// Em vez de definir na mão, vamos usar o original do driver
// Certifique-se que o arquivo "altera_pcie_cmd.h" está na mesma pasta!
#include "altera_pcie_cmd.h" 

static int g_driver_fd = -1;

bool PCIE_Open() {
    if (g_driver_fd >= 0) return true; 

    g_driver_fd = open("/dev/altera_pcie", O_RDWR);
    if (g_driver_fd < 0) {
        perror("SimplePCIE: Erro ao abrir /dev/altera_pcie");
        return false;
    }
    return true;
}

void PCIE_Close() {
    if (g_driver_fd >= 0) {
        close(g_driver_fd);
        g_driver_fd = -1;
    }
}

bool PCIE_Write32(int bar, int offset, uint32_t data) {
    if (g_driver_fd < 0) return false;

    altera_ioctl_arg arg;
    memset(&arg, 0, sizeof(arg)); // Limpa a estrutura (Segurança contra lixo de memória)
    
    arg.bar_num = bar;
    arg.offset = offset;
    arg.data = data;
//    arg.device_id = 0; // O driver pode checar isso

    if (ioctl(g_driver_fd, ALTERA_IOCX_WRITE_MEM, &arg) < 0) {
        // Se der erro, imprime o motivo exato (Permissão? Hardware?)
        fprintf(stderr, "PCIE_Write32 Falhou (Bar %d, Off 0x%X): %s\n", 
                bar, offset, strerror(errno));
        return false;
    }
    return true;
}

bool PCIE_Read32(int bar, int offset, uint32_t *data) {
    if (g_driver_fd < 0) return false;

    altera_ioctl_arg arg;
    memset(&arg, 0, sizeof(arg));

    arg.bar_num = bar;
    arg.offset = offset;
    
    if (ioctl(g_driver_fd, ALTERA_IOCX_READ_MEM, &arg) < 0) {
        fprintf(stderr, "PCIE_Read32 Falhou (Bar %d, Off 0x%X): %s\n", 
                bar, offset, strerror(errno));
        return false;
    }
    *data = arg.data;
    return true;
}

/**
 * main.cpp - Aplicação de Teste PCIe para Terasic FPGA
 * Implementação Direta sem dependência de terasic_pcie_qsys.so
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "altera_pcie_cmd.h" // Usa o mesmo header do kernel

// Configurações do Design FPGA (Ajuste conforme seu Qsys)
// Em muitos demos DE2i-150/DE5-Net, o Avalon Slave está no BAR 4
#define TARGET_BAR          0
#define LED_PIO_OFFSET      0x00  // Offset do PIO de LEDs (verifique system.h)
#define BUTTON_PIO_OFFSET   0x20  // Offset do PIO de Botões

// Encapsulamento de funções para simular a API da Terasic
int g_driver_fd = -1;

bool PCIE_Open() {
    g_driver_fd = open("/dev/altera_pcie", O_RDWR);
    if (g_driver_fd < 0) {
        perror("ERRO: Nao foi possivel abrir /dev/altera_pcie");
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
    altera_ioctl_arg arg;
    arg.bar_num = bar;
    arg.offset = offset;
    arg.data = data;
    
    if (ioctl(g_driver_fd, ALTERA_IOCX_WRITE_MEM, &arg) < 0) {
        fprintf(stderr, "PCIE_Write32 falhou: %s\n", strerror(errno));
        return false;
    }
    return true;
}

bool PCIE_Read32(int bar, int offset, uint32_t *data) {
    altera_ioctl_arg arg;
    arg.bar_num = bar;
    arg.offset = offset;
    
    if (ioctl(g_driver_fd, ALTERA_IOCX_READ_MEM, &arg) < 0) {
        fprintf(stderr, "PCIE_Read32 falhou: %s\n", strerror(errno));
        return false;
    }
    *data = arg.data;
    return true;
}

int main() {
    printf("=== Teste de Comunicacao PCIe FPGA (Linux Moderno) ===\n");
    
    if (!PCIE_Open()) {
        printf("Dica: Verifique se o modulo altera_pcie.ko esta carregado e as permissoes do /dev/altera_pcie.\n");
        return -1;
    }

    uint32_t led_val = 0;
    uint32_t btn_val = 0;
    int i;

    printf("Iniciando loop de teste. Pressione CTRL+C para sair.\n");
    printf("Escrevendo padrao binario nos LEDs e lendo Botoes...\n");

    printf("Iniciando teste de Leitura/Escrita...\n");
    for (i = 0; i < 1000; i++) {
        // 1. Escreve nos LEDs
        PCIE_Write32(TARGET_BAR, LED_PIO_OFFSET, led_val);
        
        // 2. Tenta LER os botões (Offset 0x20)
        // Se a FPGA estiver "viva", isso vai retornar 0x0, 0x1, etc.
        // Se estiver "morta", vai retornar 0xFFFFFFFF o tempo todo.
        uint32_t read_back = 0;
        if (PCIE_Read32(TARGET_BAR, BUTTON_PIO_OFFSET, &read_back)) {
            printf("Escrito: %02X | Lido (Botoes): %08X\r", led_val, read_back);
            fflush(stdout);
        }

        led_val++;
        usleep(100000); // 100ms
    }

    printf("\nTeste concluido.\n");
    PCIE_Close();
    return 0;
}

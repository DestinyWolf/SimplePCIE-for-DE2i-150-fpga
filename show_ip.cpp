#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdint.h>
#include "SimplePCIE.h"

// --- CONFIGURAÇÃO DE ENDEREÇO ---
// COLOQUE AQUI O ENDEREÇO QUE VOCÊ VIU NO QSYS (0x90)
#define LCD_PIO_BASE 0x90

// --- Mapeamento de Bits (Deve bater com o Verilog) ---
// lcd_wire[0] -> RS
// lcd_wire[1] -> RW
// lcd_wire[2] -> EN
// lcd_wire[10:3] -> DATA
#define BIT_RS  (1 << 0)
#define BIT_RW  (1 << 1)
#define BIT_EN  (1 << 2)
#define DATA_SHIFT 3 

// Escreve nos pinos brutos do PIO
void LCD_SetPins(uint8_t rs, uint8_t data) {
    uint32_t val = 0;
    
    // Configura RS (0=Cmd, 1=Data)
    if (rs) val |= BIT_RS;
    
    // RW sempre 0 (Escrita), então nem mexemos no bit.
    
    // Prepara o DADO (desloca para bits 3-10)
    // Mascara 0xFF para garantir que só 8 bits passem
    val |= ((uint32_t)(data & 0xFF)) << DATA_SHIFT;
    
    // --- O CICLO DE ESCRITA (TIMING CRÍTICO) ---
    
    // 1. SETUP: Coloca dados na linha, EN = 0
    PCIE_Write32(PCIE_BAR0, LCD_PIO_BASE, val);
    usleep(100); // 100us de setup
    
    // 2. PULSO: Sobe EN = 1
    PCIE_Write32(PCIE_BAR0, LCD_PIO_BASE, val | BIT_EN);
    usleep(1000); // 1ms (Muito tempo, para garantir!)
    
    // 3. LATCH: Desce EN = 0 (O LCD lê aqui na descida)
    PCIE_Write32(PCIE_BAR0, LCD_PIO_BASE, val);
    usleep(1000); // 1ms de hold
}

void LCD_WriteCmd(uint8_t cmd) {
    LCD_SetPins(0, cmd); // RS=0
}

void LCD_WriteData(char c) {
    LCD_SetPins(1, (uint8_t)c); // RS=1
}

// Inicialização "Mágica" do Datasheet Hitachi
void LCD_Init() {
    printf("Iniciando LCD (Modo Lento)...\n");
    
    // 1. Espera a energia estabilizar (>40ms)
    usleep(100000); 
    
    // 2. Sequência de Reset Obrigatória (3x comando 0x38)
    // Isso força o LCD a ir para modo 8-bits, não importa o estado anterior
    LCD_WriteCmd(0x38); usleep(20000); // Wait > 4.1ms
    LCD_WriteCmd(0x38); usleep(5000);  // Wait > 100us
    LCD_WriteCmd(0x38); usleep(5000);
    
    // 3. Configuração Real
    LCD_WriteCmd(0x38); // Function Set: 8-bit, 2 lines, 5x8 dots
    usleep(2000);
    
    LCD_WriteCmd(0x08); // Display OFF (Apaga tudo antes de limpar)
    usleep(2000);
    
    LCD_WriteCmd(0x01); // Clear Display (Demora!)
    usleep(50000);      // 50ms de segurança
    
    LCD_WriteCmd(0x06); // Entry Mode: Increment cursor
    usleep(2000);
    
    // 4. Liga o Display
    // 0x0C = Display ON, Cursor OFF
    // 0x0F = Display ON, Cursor ON, Blink ON (Bom para debug)
    LCD_WriteCmd(0x0F); 
    usleep(2000);
    
    printf("LCD Inicializado.\n");
}

void LCD_Print(const char* str) {
    while (*str) {
        LCD_WriteData(*str);
        str++;
    }
}

// --- Rede ---
void Get_IP_Address(char *buffer) {
    struct ifaddrs *ifaddr, *ifa;
    int s;
    char host[NI_MAXHOST];
    if (getifaddrs(&ifaddr) == -1) { sprintf(buffer, "Erro Rede"); return; }
    strcpy(buffer, "No IP");
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "lo") != 0) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s == 0) { strcpy(buffer, host); break; }
        }
    }
    freeifaddrs(ifaddr);
}

int main() {
    printf("=== DE2i-150 LCD Controller (PIO Mode) ===\n");

    if (!PCIE_Open()) {
        printf("Erro fatal no driver.\n");
        return -1;
    }
    
    // --- Teste Rápido de LED para confirmar Link ---
    PCIE_Write32(PCIE_BAR0, 0x00, 0xFF); // LEDs ON
    usleep(200000);
    PCIE_Write32(PCIE_BAR0, 0x00, 0x00); // LEDs OFF

    char ip_str[50];
    Get_IP_Address(ip_str);
    printf("IP Detectado: %s\n", ip_str);

    // Inicializa e Escreve
    LCD_Init();
    
    LCD_Print("IP Address:");
    
    LCD_WriteCmd(0xC0); // Pula linha
    
    LCD_Print(ip_str);

    PCIE_Close();
    return 0;
}

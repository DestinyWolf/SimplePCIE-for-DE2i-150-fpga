#ifndef SIMPLE_PCIE_H
#define SIMPLE_PCIE_H

#include <stdint.h>

// Definições para facilitar o uso (compatibilidade com código antigo)
#define PCIE_BAR0 0
#define PCIE_BAR1 1
#define PCIE_BAR2 2
#define PCIE_BAR3 3
#define PCIE_BAR4 4
#define PCIE_BAR5 5

// Funções da API Simplificada
bool PCIE_Open();
void PCIE_Close();
bool PCIE_Write32(int bar, int offset, uint32_t data);
bool PCIE_Read32(int bar, int offset, uint32_t *data);

#endif // SIMPLE_PCIE_H

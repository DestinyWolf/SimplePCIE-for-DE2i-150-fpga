// PCIE.h

#ifndef _INC_PCIE_H
#define _INC_PCIE_H

#include "TERASIC_PCIE.h"

#ifdef __cplusplus
extern "C"{
#endif

// Carrega a biblioteca (DLL/SO) dinamicamente
void *PCIE_Load(void);
// Descarrega a biblioteca
void PCIE_Unload(void *lib_handle);

// Ponteiros de função para as chamadas da API
extern LPPCIE_Open          PCIE_Open;
extern LPPCIE_Close         PCIE_Close;
extern LPPCIE_Read32        PCIE_Read32;
extern LPPCIE_Write32       PCIE_Write32;
extern LPPCIE_Read16        PCIE_Read16;
extern LPPCIE_Write16       PCIE_Write16;
extern LPPCIE_Read8         PCIE_Read8;
extern LPPCIE_Write8        PCIE_Write8;
extern LPPCIE_DmaWrite      PCIE_DmaWrite;
extern LPPCIE_DmaRead       PCIE_DmaRead;
extern LPPCIE_DmaFifoWrite  PCIE_DmaFifoWrite;
extern LPPCIE_DmaFifoRead   PCIE_DmaFifoRead;

#ifdef __cplusplus
}
#endif

#endif /* _INC_PCIE_H */

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include "PCIE.h"
#include "TERASIC_PCIE.h" // Garante que os tipos LPPCIE_... sejam vistos

// Definição das variáveis globais
LPPCIE_Open 		PCIE_Open;
LPPCIE_Close 		PCIE_Close;
LPPCIE_Read32 		PCIE_Read32;
LPPCIE_Write32 		PCIE_Write32;
LPPCIE_Read16 		PCIE_Read16;
LPPCIE_Write16 		PCIE_Write16;
LPPCIE_Read8 		PCIE_Read8;
LPPCIE_Write8	 	PCIE_Write8;
LPPCIE_DmaWrite		PCIE_DmaWrite;
LPPCIE_DmaRead		PCIE_DmaRead;
LPPCIE_DmaFifoWrite	PCIE_DmaFifoWrite;
LPPCIE_DmaFifoRead	PCIE_DmaFifoRead;

void *PCIE_Load(void){
	int bSuccess = 1;
	void *lib_handle;
    char szName[256] = "./terasic_pcie_qsys.so";

	lib_handle = dlopen(szName, RTLD_NOW);
	if (!lib_handle){
		printf("Load %s error: %s\r\n", szName, dlerror());
		bSuccess = 0;
	}

	if(bSuccess){
        // AQUI ESTA A CORRECAO: Adicionados (casts) explícitos para C++
		PCIE_Open = (LPPCIE_Open)dlsym(lib_handle, "PCIE_Open");
		PCIE_Close = (LPPCIE_Close)dlsym(lib_handle, "PCIE_Close");
		PCIE_Read32 = (LPPCIE_Read32)dlsym(lib_handle, "PCIE_Read32");
		PCIE_Write32 = (LPPCIE_Write32)dlsym(lib_handle, "PCIE_Write32");
		PCIE_Read16 = (LPPCIE_Read16)dlsym(lib_handle, "PCIE_Read16");
		PCIE_Write16 = (LPPCIE_Write16)dlsym(lib_handle, "PCIE_Write16");
		PCIE_Read8 = (LPPCIE_Read8)dlsym(lib_handle, "PCIE_Read8");
		PCIE_Write8 = (LPPCIE_Write8)dlsym(lib_handle, "PCIE_Write8");
		PCIE_DmaWrite = (LPPCIE_DmaWrite)dlsym(lib_handle, "PCIE_DmaWrite");
		PCIE_DmaRead = (LPPCIE_DmaRead)dlsym(lib_handle, "PCIE_DmaRead");
		PCIE_DmaFifoWrite = (LPPCIE_DmaFifoWrite)dlsym(lib_handle, "PCIE_DmaFifoWrite");
		PCIE_DmaFifoRead = (LPPCIE_DmaFifoRead)dlsym(lib_handle, "PCIE_DmaFifoRead");
		
		if (!PCIE_Open || !PCIE_Read32 || !PCIE_Write32)
			bSuccess = 0;
		
		if (!bSuccess){
			dlclose(lib_handle);
			lib_handle = 0;
		}
	}
	return lib_handle;
}

void PCIE_Unload(void *lib_handle){
    if (lib_handle) dlclose(lib_handle);
}

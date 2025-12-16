#ifndef _INC_TERASIC_PCIE_QSYS_H
#define _INC_TERASIC_PCIE_QSYS_H

/* GNU C/C++ Compiler */
#if defined(__GNUC__)
	#include <stdio.h>
	#define FAR
	#define PASCAL   
	#define __export 
	#define BOOL	int
	#define TRUE	1
	#define FALSE	0
	#define DWORD	unsigned int
	#define WORD	unsigned short
	#define USHORT	unsigned short
	#define UCHAR	unsigned char
	#define ULONG	unsigned int
    #define BYTE    unsigned char
	typedef int	PCIE_HANDLE;
#else
    // MS Windows
    #define NOMINMAX
    #include <windows.h>
    typedef void *PCIE_HANDLE;
#endif

#ifdef __cplusplus
extern "C"{
#endif

#define PCIE_API FAR PASCAL
typedef DWORD PCIE_ADDRESS;
typedef DWORD PCIE_LOCAL_ADDRESS;
typedef DWORD PCIE_LOCAL_FIFO_ID;

#define DEFAULT_PCIE_VID 0x1172
#define DEFAULT_PCIE_DID 0xE001

typedef enum
{
    PCIE_BAR0 = 0,
    PCIE_BAR1,
    PCIE_BAR2,
    PCIE_BAR3,
    PCIE_BAR4,
    PCIE_BAR5
}PCIE_BAR;

typedef struct
{
    DWORD dwCounter;
    DWORD dwLost;
    BOOL fStopped;
} PCIE_INT_RESULT;

#pragma pack(push, 1)
typedef struct {
    USHORT  VendorID;
    USHORT  DeviceID;
    USHORT  Command;
    USHORT  Status;
    UCHAR   RevisionID;
    UCHAR   ProgIf;
    UCHAR   SubClass;
    UCHAR   BaseClass;
    UCHAR   CacheLineSize;
    UCHAR   LatencyTimer;
    UCHAR   HeaderType;
    UCHAR   BIST;
    union {
        struct _MYPCI_HEADER_TYPE_0 {
            ULONG   BaseAddresses[6];
            ULONG   CIS;
            USHORT  SubVendorID;
            USHORT  SubSystemID;
            ULONG   ROMBaseAddress;
            UCHAR   CapabilitiesPtr;
            UCHAR   Reserved1[3];
            ULONG   Reserved2;
            UCHAR   InterruptLine;
            UCHAR   InterruptPin;
            UCHAR   MinimumGrant;
            UCHAR   MaximumLatency;
        } type0;
    } u;
    UCHAR   DeviceSpecific[192];
} PCIE_CONFIG;
#pragma pack(pop)

typedef void (PCIE_API *TERASIC_INT_HANDLER)( PCIE_HANDLE hALTERA, PCIE_INT_RESULT *intResult);

// Ponteiros de função para carregamento dinâmico
typedef BOOL  (PCIE_API *LPPCIE_ScanCard)(WORD wVendorID, WORD wDeviceID, DWORD *pdwDeviceNum, PCIE_CONFIG szConfigList[]);
typedef PCIE_HANDLE (PCIE_API *LPPCIE_Open)(WORD wVendorID, WORD wDeviceID, WORD wCardNum);
typedef void (PCIE_API *LPPCIE_Close)(PCIE_HANDLE hFPGA);
typedef BOOL (PCIE_API *LPPCIE_Read32)(PCIE_HANDLE hFPGA, PCIE_BAR PciBar, PCIE_ADDRESS PciAddress, DWORD *pdwData);
typedef BOOL (PCIE_API *LPPCIE_Write32)(PCIE_HANDLE hFPGA, PCIE_BAR PciBar, PCIE_ADDRESS PciAddress, DWORD dwData);
typedef BOOL (PCIE_API *LPPCIE_Read16)(PCIE_HANDLE hFPGA, PCIE_BAR PciBar, PCIE_ADDRESS PciAddress, WORD *pwData);
typedef BOOL (PCIE_API *LPPCIE_Write16)(PCIE_HANDLE hFPGA, PCIE_BAR PciBar, PCIE_ADDRESS PciAddress, WORD wData);
typedef BOOL (PCIE_API *LPPCIE_Read8)(PCIE_HANDLE hFPGA, PCIE_BAR PciBar, PCIE_ADDRESS PciAddress, BYTE *pcData);
typedef BOOL (PCIE_API *LPPCIE_Write8)(PCIE_HANDLE hFPGA, PCIE_BAR PciBar, PCIE_ADDRESS PciAddress, BYTE cData);
typedef BOOL (PCIE_API *LPPCIE_DmaRead) (PCIE_HANDLE hFPGA, PCIE_LOCAL_ADDRESS LocalAddress, void *pBuffer, DWORD dwBufSize);
typedef BOOL (PCIE_API *LPPCIE_DmaWrite) (PCIE_HANDLE hFPGA, PCIE_LOCAL_ADDRESS LocalAddress, void *pData, DWORD dwDataSize);
typedef BOOL (PCIE_API *LPPCIE_DmaFifoRead) (PCIE_HANDLE hFPGA, PCIE_LOCAL_FIFO_ID LocalFifoId, void *pBuffer, DWORD dwBufSize);
typedef BOOL (PCIE_API *LPPCIE_DmaFifoWrite) (PCIE_HANDLE hFPGA, PCIE_LOCAL_FIFO_ID LocalFifoId, void *pData, DWORD dwDataSize);
typedef BOOL (PCIE_API *LPPCIE_IntIsEnabled) (PCIE_HANDLE hFPGA);
typedef BOOL (PCIE_API *LPPCIE_IntEnable) (PCIE_HANDLE hFPGA, TERASIC_INT_HANDLER funcIntHandler);
typedef void (PCIE_API *LPPCIE_IntDisable) (PCIE_HANDLE hFPGA);

#ifdef __cplusplus
}
#endif

#endif /* _INC_TERASIC_PCIE_QSYS_H */

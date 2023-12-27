#pragma once
#include <ntifs.h>

typedef struct _OB_REG_CONTEXT {
	USHORT Version;
	UNICODE_STRING Altitude;
	USHORT ulIndex;
	OB_OPERATION_REGISTRATION* OperationRegistration;
} REG_CONTEXT, * PREG_CONTEXT;

PVOID OBRegisterHandle;
PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;
ULONG ProtectController, ProtectedPID, EnableProcessHiding;

#define PROCESS_QUERY_LIMITED_INFORMATION 0x1000
#define PROCESS_VM_OPERATION 0x0008
#define PROCESS_VM_READ 0x0010
#define PROCESS_VM_WRITE 0x0020
#define PROCESS_TERMINATE 0x0001
#pragma once
#include "events.h"

#define IO_PROTECT_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8987 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp);

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp);

typedef struct _PROTECT_REQUEST
{
	ULONG ProcessId;

	ULONG ProtectController;
	ULONG EnableProcessHiding;
}KERNEL_PROTECT_REQUEST, *PKERNEL_PROTECT_REQUEST;
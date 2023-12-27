#include "communication.h"

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	NTSTATUS Status;
	ULONG BytesIO = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	// Code received from user space
	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	if (ControlCode == IO_PROTECT_REQUEST)
	{
		PKERNEL_PROTECT_REQUEST ProtectInput = (PKERNEL_PROTECT_REQUEST)Irp->AssociatedIrp.SystemBuffer;

		if (ProtectInput->EnableProcessHiding == 1)
		{
			UINT32 intmadafaka = (UINT32)ProtectInput->ProcessId;
			PCHAR str = HideProcess(intmadafaka);
			// DbgPrintEx(0, 0, "HideProcess executed finished result: <%s>\r\n", str);
		}

		if (ProtectInput->ProtectController == 1)
		{
			ProtectedPID = ProtectInput->ProcessId;
			ProtectProcess(ProtectedPID, 0x72);
			// DbgPrintEx(0, 0, "Protect Process Change to pid %d\r\n", ProtectedPID);
		}

		Status = STATUS_SUCCESS;
		BytesIO = sizeof(KERNEL_PROTECT_REQUEST);
	}

	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
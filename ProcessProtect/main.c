#include "communication.h"
#include "data.h"

extern PVOID OBRegisterHandle = NULL;

NTSTATUS RegisterOBCallback();

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unload Driver\r\n");
	IoDeleteSymbolicLink(&dos);
	IoDeleteDevice(pDriverObject->DeviceObject);

	if (OBRegisterHandle)
	{
		ObUnRegisterCallbacks(OBRegisterHandle);
		OBRegisterHandle = NULL;
	}

	// Delete the processnotify routine
	PsSetCreateProcessNotifyRoutineEx(ProcessNotifyCallbackEx, TRUE);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	DbgPrintEx(0, 0, "DriverEntry\r\n");
	RtlInitUnicodeString(&dev, L"\\Device\\ProtectProcess");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\ProtectProcess");

	IoCreateDevice(pDriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&dos, &dev);

	pDriverObject->DriverUnload = UnloadDriver;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	NTSTATUS status = RegisterOBCallback();
	if (status != STATUS_SUCCESS) {
		DbgPrintEx(0, 0, "RegisterOBCallback Failed Status: %x\r\n", status);
	}

	// Set up the CreateProcess routine.
	status = PsSetCreateProcessNotifyRoutineEx(ProcessNotifyCallbackEx, FALSE);
	if (status != STATUS_SUCCESS) {
		DbgPrintEx(0, 0, "CreateProcessNotifyRoutine Failed Status: %x\r\n", status);
	}

	return STATUS_SUCCESS;
}

NTSTATUS RegisterOBCallback()
{
	OB_OPERATION_REGISTRATION OBOperationRegistration;
	OB_CALLBACK_REGISTRATION OBOCallbackRegistration;
	REG_CONTEXT regContext;
	UNICODE_STRING usAltitude;
	memset(&OBOperationRegistration, 0, sizeof(OB_OPERATION_REGISTRATION));
	memset(&OBOCallbackRegistration, 0, sizeof(OB_CALLBACK_REGISTRATION));
	memset(&regContext, 0, sizeof(REG_CONTEXT));
	regContext.ulIndex = 1;
	regContext.Version = 120;
	RtlInitUnicodeString(&usAltitude, L"666");

	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	if ((USHORT)ObGetFilterVersion() == OB_FLT_REGISTRATION_VERSION)
	{
		OBOperationRegistration.ObjectType = PsProcessType; // Use To Strip Handle Permissions For Threads PsThreadType
		OBOperationRegistration.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		OBOperationRegistration.PostOperation = NULL;
		OBOperationRegistration.PreOperation = OBRegisterCallback;

		OBOCallbackRegistration.Altitude = usAltitude;
		OBOCallbackRegistration.OperationRegistration = &OBOperationRegistration;
		OBOCallbackRegistration.RegistrationContext = &regContext;
		OBOCallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
		OBOCallbackRegistration.OperationRegistrationCount = (USHORT)1;

		Status = ObRegisterCallbacks(&OBOCallbackRegistration, &OBRegisterHandle); // Register The CallBack
	}

	return Status;
}
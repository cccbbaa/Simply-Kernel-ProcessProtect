#include "events.h"

#pragma warning( disable : 4100 4047 4024 4022 4201 4311 4057 4213 4189 4081 4189 4706 4214 4459 4273 4242 4244 4127)

extern ULONG ProtectController = 1;
extern ULONG ProtectedPID = 0;
extern ULONG EnableProcessHiding = 0;

VOID ProcessNotifyCallbackEx(PEPROCESS Process, HANDLE processId, PPS_CREATE_NOTIFY_INFO notifyInfo)
{
	// NotifyInfo is filled when a process is created. Otherwise terminated.
	if (notifyInfo)
	{
		//DebugMessageNormal("PID = %d\r\n", processId);
		//DebugMessageNormal("Process Full Path: %ls \n", notifyInfo->ImageFileName->Buffer);
		//DbgPrintEx(0, 0, "Process Full Path: %ls \r\n", notifyInfo->ImageFileName->Buffer);
		if (wcsstr(notifyInfo->ImageFileName->Buffer, L"\\CProtectE.exe"))
		{
			//DebugMessageNormal("Bomb has been planted!\n");
			//ProtectedPID = (ULONG)processId;
			//DebugMessageNormal("Controller ProcessID: %d\r\n", ProtectedPID);
			//DbgPrintEx(0, 0, "Find ProtectedProcess\r\n");
			if (EnableProcessHiding == 1)
			{
				UINT32 intmadafaka = (UINT32)processId;

				PCHAR str = HideProcess(intmadafaka);
				DbgPrintEx(0, 0, "%s", str);
			}
		}
	}
}

OB_PREOP_CALLBACK_STATUS OBRegisterCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	if (ProtectedPID == 0)
	{
		return OB_PREOP_SUCCESS;
	}

	PEPROCESS OpenedProcess = (PEPROCESS)OperationInformation->Object;
	ULONG OpenedProcessID = (ULONG)PsGetProcessId(OpenedProcess);
	//PEPROCESS CurrentProcess = PsGetCurrentProcess();
	// Allow any driver to get the handle.
	/*if (OperationInformation->KernelHandle == 1)
	{
		return OB_PREOP_SUCCESS;
	}*/

	// https://docs.microsoft.com/en-us/windows/win32/procthread/process-security-and-access-rights
	if (OpenedProcessID == ProtectedPID && ProtectController == 1)
	{
		// DbgPrintEx(0, 0, "Detected try to get <%d> handle\r\n", OpenedProcessID);
		OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
		OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = 0;
		/*if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
		{
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION);
		}
		else
		{
			OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION);
		}*/
	}

	return OB_PREOP_SUCCESS;
}

VOID ProtectProcess(HANDLE currentPid, UCHAR protectCode)
{
	PEPROCESS process = NULL, next = NULL;
	next = process = IoGetCurrentProcess();
	HANDLE pid = NULL;
	LIST_ENTRY64 list = { 0 };
	do
	{
		// DbgPrint("Searching Process\n");
		pid = PsGetProcessId(next);
		if (pid == currentPid)
		{
			DbgPrint("Get Process, Pid: %p WriteCode: %uhhd\n", pid, protectCode);
			*(UCHAR*)((ULONG64)next + 0x87A) = protectCode;
		}
		list = *(PLIST_ENTRY64)((ULONG64)next + 0x448);
		next = (PEPROCESS)((ULONG64)list.Flink - 0x448); // 遍历进程链表
	} while (next != process); // 当遍历一圈后结束
	return;
}
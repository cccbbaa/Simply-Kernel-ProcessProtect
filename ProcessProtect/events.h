#pragma once

#include "data.h"
#include "hide.h"

VOID ProcessNotifyCallbackEx(PEPROCESS Process, HANDLE processId, PPS_CREATE_NOTIFY_INFO notifyInfo);

OB_PREOP_CALLBACK_STATUS OBRegisterCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);

VOID ProtectProcess(HANDLE pid, UCHAR protectCode); // 0x72 FullProtect
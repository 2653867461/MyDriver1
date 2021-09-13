#pragma once

#include"DriverCommon.h"

extern "C" NTSTATUS  DriverEntry(_In_ PDRIVER_OBJECT pDriverObject, _In_ PUNICODE_STRING pRegistryPath);

//extern "C" VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject);

//extern "C" VOID WkStartIo(_In_ PDEVICE_OBJECT pDeviceObject, _In_ PIRP pIrp);
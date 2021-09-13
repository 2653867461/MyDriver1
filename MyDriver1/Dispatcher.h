#pragma once
#include"DriverCommon.h"

extern "C" NTSTATUS ReadComplete(IN PDEVICE_OBJECT, IN PIRP pIrp, IN PVOID Context);
extern "C" NTSTATUS DispatchRead(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
extern "C" NTSTATUS DispatchPNP(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
extern "C" NTSTATUS DispatchGeneral(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP pIrp);
//NTSTATUS DispatchWrite(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP pIrp);
extern "C" NTSTATUS AttachKbdclassDevices(IN PDRIVER_OBJECT pDriverObject);
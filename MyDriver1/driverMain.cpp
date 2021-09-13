#include"driverMain.h"
#include"WkDeviceExtension.h"
#include"Dispatcher.h"

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT pDriverObject, _In_ PUNICODE_STRING pRegistryPath)
{
	NTSTATUS status=STATUS_SUCCESS;
	ULONG i;
	KdPrint(("my driver:entering\n"));
	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
		pDriverObject->MajorFunction[i] = DispatchGeneral;

	pDriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	pDriverObject->MajorFunction[IRP_MJ_PNP] = DispatchPNP;
	//pDriverObject->DriverUnload = DriverUnload;

	gDriverObject = pDriverObject;
	return status;
}



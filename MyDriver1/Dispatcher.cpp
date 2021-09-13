#include"Dispatcher.h"
#include"WkDeviceExtension.h"


NTSTATUS AttachKbdclassDevices(IN PDRIVER_OBJECT pDriverObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING strDriverName;
	PWK_DEVICE_EXTENSION devExt;
	PDEVICE_OBJECT pFilterDeviceObject = NULL;
	PDEVICE_OBJECT pTargetDeviceObject = NULL;
	PDEVICE_OBJECT pLowerDeviceObject = NULL;
	PDRIVER_OBJECT pKbdclassDriver = NULL;

	KdPrint(("start attach kbdclass driver's device\n"));

	RtlInitUnicodeString(&strDriverName, KBD_DRIVER_NAME);
	status = ObReferenceObjectByName(
		&strDriverName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		IoDriverObjectType,
		KernelMode,
		NULL,
		(PVOID*)&pKbdclassDriver
	);

	if (!NT_SUCCESS(status)) {
		KdPrint(("attach device: reference kbdclass driver object failed\n"));
		return status;
	}
	else {
		ObDereferenceObject(pDriverObject);
	}

	pTargetDeviceObject = pKbdclassDriver->DeviceObject;

	while (pTargetDeviceObject)
	{
		status = IoCreateDevice(
			IN pDriverObject,
			sizeof(WK_DEVICE_EXTENSION),
			NULL,
			pTargetDeviceObject->DeviceType,
			pTargetDeviceObject->Characteristics,
			FALSE,
			&pFilterDeviceObject
		);
			if (!NT_SUCCESS(status)) {
				KdPrint(("attach device: create filter device failed"));
				return status;
			}

			pLowerDeviceObject = IoAttachDeviceToDeviceStack(pFilterDeviceObject, pTargetDeviceObject);
			
			if (!pLowerDeviceObject)
			{
				KdPrint(("attach device: attach device failed"));
				IoDeleteDevice(pFilterDeviceObject);
				pFilterDeviceObject = NULL;
				return status;
			}

			devExt = (PWK_DEVICE_EXTENSION)(pFilterDeviceObject->DeviceExtension);
			InitializeDeviceExtension(devExt, pFilterDeviceObject, pTargetDeviceObject, pLowerDeviceObject);

			pFilterDeviceObject->DeviceType = pLowerDeviceObject->DeviceType;
			pFilterDeviceObject->Characteristics = pLowerDeviceObject->Characteristics;
			pFilterDeviceObject->StackSize = pLowerDeviceObject->StackSize + 1;
			pFilterDeviceObject->Flags |= pLowerDeviceObject->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE);

			pTargetDeviceObject = pTargetDeviceObject->NextDevice;
	}
	return status;
}


NTSTATUS DispatchGeneral(_In_ PDEVICE_OBJECT pDeviceObject, _Inout_ PIRP pIrp)
{
	IoSkipCurrentIrpStackLocation(pIrp);
	return IoCallDriver((((PWK_DEVICE_EXTENSION)(pDeviceObject->DeviceExtension))->pLowerDeviceObject), pIrp);
}


NTSTATUS DispatchPNP(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	PWK_DEVICE_EXTENSION pDevExt = NULL;
	PIO_STACK_LOCATION pIrpSp;
	NTSTATUS status = STATUS_SUCCESS;

	pDevExt = (PWK_DEVICE_EXTENSION)(pDeviceObject->DeviceExtension);
	pIrpSp = IoGetCurrentIrpStackLocation(pIrp);

	switch (pIrpSp->MinorFunction)
	{
	case IRP_MN_REMOVE_DEVICE:
		KdPrint(("IRP_MN_REMOVE_DEVICE\n"));
		IoSkipCurrentIrpStackLocation(pIrp);
		IoCallDriver(pDevExt->pLowerDeviceObject, pIrp);
		IoDetachDevice(pDevExt->pLowerDeviceObject);
		IoDeleteDevice(pDeviceObject);
		status = STATUS_SUCCESS;
		break;
	default:
		IoSkipCurrentIrpStackLocation(pIrp);
		status = IoCallDriver(pDevExt->pLowerDeviceObject, pIrp);
	}
	return status;
}


NTSTATUS DispatchRead(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	//NTSTATUS status = STATUS_SUCCESS;
	PWK_DEVICE_EXTENSION pDevExt = NULL;
	PIO_STACK_LOCATION pIrpSp = NULL;

	if (pIrp->CurrentLocation == 1)
	{
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return  STATUS_INVALID_DEVICE_REQUEST;
	}


	++gKeyCount;

	pDevExt = (PWK_DEVICE_EXTENSION)(pDeviceObject->DeviceExtension);

	pIrpSp = IoGetCurrentIrpStackLocation(pIrp);
	IoCopyCurrentIrpStackLocationToNext(pIrp);
	IoSetCompletionRoutine(pIrp, ReadComplete, pDeviceObject, true, true, true);
	return IoCallDriver(pDevExt->pLowerDeviceObject, pIrp);
}



NTSTATUS ReadComplete(IN PDEVICE_OBJECT, IN PIRP pIrp, IN PVOID Context)
{
	PIO_STACK_LOCATION pIrpSp;
	PKEYBOARD_INPUT_DATA keyData = NULL;
	SIZE_T numKeys = 0;
	Context = NULL;

	pIrpSp = IoGetCurrentIrpStackLocation(pIrp);
	if (NT_SUCCESS(pIrp->IoStatus.Status))
	{
		keyData = (PKEYBOARD_INPUT_DATA)pIrp->AssociatedIrp.SystemBuffer;
		numKeys = (pIrp->IoStatus.Information) / sizeof(KEYBOARD_INPUT_DATA);
		while (numKeys--)
		{
			DbgPrint("numKeys:%x  ", (int)numKeys);
			DbgPrint("ScanCode:%x", keyData->MakeCode);
			DbgPrint("%s\n", keyData->Flags ? "up" : "down");
			++keyData;
		}
	}
	--gKeyCount;
	if (pIrp->PendingReturned)
		IoMarkIrpPending(pIrp);

	return pIrp->IoStatus.Status;
}
#include"WkDeviceExtension.h"

NTSTATUS InitializeDeviceExtension(
	IN PWK_DEVICE_EXTENSION pDeviceExtension,
	PDEVICE_OBJECT pFilterDeviceObject,
	PDEVICE_OBJECT pTargetDeviceObject,
	PDEVICE_OBJECT pLowerDeviceObject)
{
	memset(pDeviceExtension, 0, sizeof(WK_DEVICE_EXTENSION));
	pDeviceExtension->pFilterDeviceObject = pFilterDeviceObject;
	pDeviceExtension->pLowerDeviceObject = pLowerDeviceObject;
	pDeviceExtension->pTargetDeviceObject = pTargetDeviceObject;
	
	KeInitializeSpinLock(&pDeviceExtension->IoRequestsSpinLock);
	KeInitializeEvent(&pDeviceExtension->IoInProcessEvent, NotificationEvent, FALSE);

	return STATUS_SUCCESS;

}
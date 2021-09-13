#pragma once
#include"DriverCommon.h"

typedef struct _WK_DEVICE_EXTENSION
{
	ULONG NodeSize;
	PDEVICE_OBJECT pFilterDeviceObject;
	PDEVICE_OBJECT pTargetDeviceObject;
	PDEVICE_OBJECT pLowerDeviceObject;
	KSPIN_LOCK IoRequestsSpinLock;
	KEVENT IoInProcessEvent;

}WK_DEVICE_EXTENSION, * PWK_DEVICE_EXTENSION;

extern "C"  NTSTATUS InitializeDeviceExtension(
	IN PWK_DEVICE_EXTENSION pDeviceExtension,
	PDEVICE_OBJECT pFilterDeviceObject,
	PDEVICE_OBJECT pTargetDeviceObject,
	PDEVICE_OBJECT pLowerDeviceObject);
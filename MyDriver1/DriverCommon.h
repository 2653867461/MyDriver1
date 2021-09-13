#pragma once

#include<ntddk.h>
#include<ntstrsafe.h>

#define KBD_DRIVER_NAME L"\\Driver\\kbdclass"

extern "C" ULONG gKeyCount;
extern "C" PDRIVER_OBJECT gDriverObject;
extern "C" _OBJECT_TYPE * IoDriverObjectType;

#define KEY_DOWN 0 
#define KEY_UP 1

typedef struct _KEYBOARD_INPUT_DATA {
    USHORT UnitId;
    USHORT MakeCode;
    USHORT Flags;
    USHORT Reserved;
    ULONG  ExtraInformation;
} KEYBOARD_INPUT_DATA, * PKEYBOARD_INPUT_DATA;

extern "C"  NTSTATUS
ObReferenceObjectByName(
    __in PUNICODE_STRING ObjectName,
    __in ULONG Attributes,
    __in_opt PACCESS_STATE AccessState,
    __in_opt ACCESS_MASK DesiredAccess,
    __in POBJECT_TYPE ObjectType,
    __in KPROCESSOR_MODE AccessMode,
    __inout_opt PVOID ParseContext,
    __out PVOID* Object
);

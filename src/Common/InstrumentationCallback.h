#pragma once

#ifndef _INSTRUMENTATIONCALLBACK_
#define _INSTRUMENTATIONCALLBACK_

#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>

typedef struct _PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION
{
    ULONG Version;
    ULONG Reserved;
    PVOID Callback;
} PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION, *PPROCESS_INSTRUMENTATION_CALLBACK_INFORMATION;

typedef VOID (__stdcall *PINSTRUMENTATION_CALLBACK_ROUTINE)(ULONG SystemCallNumber, PVOID lpNtFunction, PVOID lpSysretAddr);
typedef PINSTRUMENTATION_CALLBACK_ROUTINE LPINSTRUMENTATION_CALLBACK_ROUTINE;

BOOL RegisterInstrumentationCallbackEx(HANDLE hProcess, LPINSTRUMENTATION_CALLBACK_ROUTINE lpInstrumentationCallback);
BOOL RegisterInstrumentationCallback(LPINSTRUMENTATION_CALLBACK_ROUTINE lpInstrumentationCallback);

#endif
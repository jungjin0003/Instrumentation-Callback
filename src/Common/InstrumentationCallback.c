#include "InstrumentationCallback.h"

LPINSTRUMENTATION_CALLBACK_ROUTINE CallbackRoutine;

VOID CALLBACK InstrumentationCallbackThunk()
{
    static PVOID NtdllBase = NULL;
    static PVOID Win32uBase = NULL;

    ULONG SystemCallNumber = 0xFFFFFFFF;
    PVOID NtFunction = NULL;
    PVOID SysretAddr;

    CONTEXT context = { 0, };
    RtlCaptureContext(&context);

    SysretAddr = context.Rip = context.R10;

    if (NtdllBase == NULL)
        NtdllBase = GetModuleHandleA("ntdll.dll");
    if (Win32uBase == NULL)
        Win32uBase = GetModuleHandleA("win32u.dll");
    
    DWORD NtdllSize = ((IMAGE_NT_HEADERS64 *)((ULONG_PTR)NtdllBase + ((IMAGE_DOS_HEADER *)NtdllBase)->e_lfanew))->OptionalHeader.SizeOfImage;
    DWORD Win32uSize = Win32uBase == NULL ? 0 : ((IMAGE_NT_HEADERS64 *)((ULONG_PTR)Win32uBase + ((IMAGE_DOS_HEADER *)Win32uBase)->e_lfanew))->OptionalHeader.SizeOfImage;

    if ((NtdllBase <= SysretAddr && SysretAddr <= (ULONG_PTR)NtdllBase + NtdllSize) || (Win32uBase <= SysretAddr && SysretAddr <= (ULONG_PTR)Win32uBase + Win32uSize))
    {
        SystemCallNumber = *(ULONG *)((ULONG_PTR)SysretAddr - 0x10);
        NtFunction = (ULONG_PTR)SysretAddr - 0x14;
    }

    CallbackRoutine(SystemCallNumber, NtFunction, SysretAddr);

    RtlRestoreContext(&context, NULL);
}

BOOL RegisterInstrumentationCallbackEx(HANDLE hProcess, LPINSTRUMENTATION_CALLBACK_ROUTINE lpInstrumentationCallback)
{
    PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION nirvana;
    nirvana.Version = 0;
    nirvana.Reserved = 0;
    nirvana.Callback = InstrumentationCallbackThunk;

    CallbackRoutine = lpInstrumentationCallback;

    return NtSetInformationProcess(hProcess, ProcessInstrumentationCallback, &nirvana, sizeof(PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION)) == STATUS_SUCCESS;
}

BOOL RegisterInstrumentationCallback(LPINSTRUMENTATION_CALLBACK_ROUTINE lpInstrumentationCallback)
{
    return RegisterInstrumentationCallbackEx(0xFFFFFFFFFFFFFFFF, lpInstrumentationCallback);
}
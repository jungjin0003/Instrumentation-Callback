#include "InstrumentationCallback.h"

LPINSTRUMENTATION_CALLBACK_ROUTINE CallbackRoutine;

#ifdef _MSC_VER
#define ProcessInstrumentationCallback ((PROCESS_INFORMATION_CLASS)40)

extern VOID CALLBACK InstrumentationCallbackThunkStart();

NTSYSAPI NTSTATUS NTAPI NtSetInformationProcess(HANDLE ProcessHandle, PROCESS_INFORMATION_CLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength);
#endif

#ifdef __GNUC__
VOID InstrumentationCallbackThunkEnd();

__declspec(naked) VOID CALLBACK InstrumentationCallbackThunkStart()
{
    __asm__ __volatile__ (
        "mov qword ptr gs:[0x2E0], rsp\n\t"
        "mov qword ptr gs:[0x2D8], r10\n\t"
        "sub rsp, 0x4D0\n\t"
        "and rsp, 0xFFFFFFFFFFFFFFF6\n\t"
        "mov rcx, rsp\n\t"
        "call %[RtlCaptureContext]\n\t"
        "sub rsp, 0x20\n\t"
        "call %[InstrumentationCallbackThunkEnd]\n\t"
        :
        : [InstrumentationCallbackThunkEnd] "X" (InstrumentationCallbackThunkEnd), [RtlCaptureContext] "X" (RtlCaptureContext)
    );
}
#endif

VOID InstrumentationCallbackThunkEnd(CONTEXT *context)
{
    static PVOID NtdllBase = NULL;
    static PVOID Win32uBase = NULL;

    context->Rip = __readgsqword(0x2D8);
    context->Rsp = __readgsqword(0x2E0);

    if (__readgsbyte(0x02EC))
        goto THUNK_END_DONE;

    __writegsbyte(0x02EC, 0x01);

    ULONG SystemCallNumber = 0xFFFFFFFF;
    PVOID NtFunction = NULL;
    PVOID SysretAddr;

    SysretAddr = context->Rip;

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
    __writegsbyte(0x02EC, 0x00);
THUNK_END_DONE:
    RtlRestoreContext(context, NULL);
}

BOOL RegisterInstrumentationCallbackEx(HANDLE hProcess, LPINSTRUMENTATION_CALLBACK_ROUTINE lpInstrumentationCallback)
{
    PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION nirvana;
    nirvana.Version = 0;
    nirvana.Reserved = 0;
    nirvana.Callback = InstrumentationCallbackThunkStart;

    CallbackRoutine = lpInstrumentationCallback;

    return NtSetInformationProcess(hProcess, ProcessInstrumentationCallback, &nirvana, sizeof(PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION)) == STATUS_SUCCESS;
}

BOOL RegisterInstrumentationCallback(LPINSTRUMENTATION_CALLBACK_ROUTINE lpInstrumentationCallback)
{
    return RegisterInstrumentationCallbackEx(0xFFFFFFFFFFFFFFFF, lpInstrumentationCallback);
}
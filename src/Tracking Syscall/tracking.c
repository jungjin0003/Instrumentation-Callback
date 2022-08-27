#include "tracking.h"

VOID CALLBACK InstrumentationCallbackThunk()
{
    PVOID SysretAddr;
    __asm__ __volatile__ (
        "mov %[SysretAddr], r10\n\t"
        :
        : [SysretAddr] "m" (SysretAddr);
    );
}

VOID RegisterInstrumentationCallbackEx(HANDLE hProcess, LPINSTRUMENTATION_CALLBACK_ROUTINE lpInstrumentationCallback)
{
    
}

VOID RegisterInstrumentationcallback(LPINSTRUMENTATION_CALLBACK_ROUTINE lpInstrumentationCallback)
{
    RegisterInstrumentationCallbackEx(0xFFFFFFFFFFFFFFFF, lpInstrumentationCallback);
}
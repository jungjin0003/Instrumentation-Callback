#include "detect.h"

VOID CALLBACK InstrumentationCallback(ULONG SystemCallNumber, PVOID lpNtFunction, PVOID lpSysret)
{
    if (SystemCallNumber == 0xFFFFFFFF && lpNtFunction == NULL)
    {
        printf("[DETECT] Detect manual syscall!!\n");
        __debugbreak();
    }
}
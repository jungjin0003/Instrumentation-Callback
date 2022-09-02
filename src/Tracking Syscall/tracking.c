#include "tracking.h"

VOID CALLBACK InstrumentationCallback(ULONG SystemCallNumber, PVOID lpNtFunction, PVOID lpSysret)
{
    if (SystemCallNumber == 0xFFFFFFFF && lpNtFunction == NULL)
        printf("[TRACKING] It is manual syscall and sysret address is %p\n", lpSysret);
    else
    {
        HMODULE hModule = GetModuleHandleA("ntdll.dll");
        IMAGE_EXPORT_DIRECTORY *EXPORT = (ULONG_PTR)hModule + ((IMAGE_NT_HEADERS64 *)((ULONG_PTR)hModule + ((IMAGE_DOS_HEADER *)hModule)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

        for (int i = 0; i < EXPORT->NumberOfNames; i++)
        {
            WORD Index = *(WORD *)((ULONGLONG)hModule + EXPORT->AddressOfNameOrdinals + i * 2);
            PVOID Function = (ULONGLONG)hModule + *(DWORD *)((ULONGLONG)hModule + EXPORT->AddressOfFunctions + Index * 4);
            if (lpNtFunction == Function)
            {
                LPCSTR FunctionName = (ULONGLONG)hModule + *(DWORD *)((ULONGLONG)hModule + EXPORT->AddressOfNames + i * 4);
                printf("[TRACKING] %s Called\n", FunctionName);
                goto CallbackExit;
            }
        }

        hModule = GetModuleHandleA("win32u.dll");

        if (hModule == NULL)
            goto CallbackExit;

        EXPORT = (ULONG_PTR)hModule + ((IMAGE_NT_HEADERS64 *)((ULONG_PTR)hModule + ((IMAGE_DOS_HEADER *)hModule)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    
        for (int i = 0; i < EXPORT->NumberOfNames; i++)
        {
            WORD Index = *(WORD *)((ULONGLONG)hModule + EXPORT->AddressOfNameOrdinals + i * 2);
            PVOID Function = (ULONGLONG)hModule + *(DWORD *)((ULONGLONG)hModule + EXPORT->AddressOfFunctions + Index * 4);
            if (lpNtFunction == Function)
            {
                LPCSTR FunctionName = (ULONGLONG)hModule + *(DWORD *)((ULONGLONG)hModule + EXPORT->AddressOfNames + i * 4);
                printf("[TRACKING] %s Called\n", FunctionName);
                goto CallbackExit;
            }
        }

        printf("[TRACKING] Function is exported!!\n");
        printf("[TRACKING] Function address is %p\n", lpNtFunction);
    }
CallbackExit:;
}
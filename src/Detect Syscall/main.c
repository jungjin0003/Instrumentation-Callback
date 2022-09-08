#include <stdio.h>
#include <Windows.h>
#include "..\Common\InstrumentationCallback.h"
#include "detect.h"

__declspec(naked) void MyNtYieldExecution()
{
    __asm__ __volatile__ (
        "mov r10, rcx\n\t"
        "mov eax, 0x46\n\t"
        "syscall\n\t"
        "ret\n\t"
    );
}

int main(int argc, char *argv[])
{
    RegisterInstrumentationCallback(InstrumentationCallback);

    MyNtYieldExecution();

    return 0;
}
#include <stdio.h>
#include <windows.h>
#include <winternl.h>
#include "Common\InstrumentationCallback.h"

VOID CALLBACK InstrumentationCallback(ULONG SystemCallNumber, PVOID lpNtFunction, PVOID lpSysret)
{
    
}

int main(int argc, char *argv[])
{
    printf("Hello World\n");
    return 0;
}
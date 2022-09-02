#include <stdio.h>
#include <windows.h>
#include "..\Common\InstrumentationCallback.h"
#include "tracking.h"

int main(int argc, char *argv[])
{
    RegisterInstrumentationCallback(InstrumentationCallback);

    OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());

    return 0;
}
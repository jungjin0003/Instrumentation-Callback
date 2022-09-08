#pragma once

#include <stdio.h>
#include <Windows.h>

VOID CALLBACK InstrumentationCallback(ULONG SystemCallNumber, PVOID lpNtFunction, PVOID lpSysret);
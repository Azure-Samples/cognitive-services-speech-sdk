#include "stdafx.h"
#include "carbonx_pal.h"
#include <Windows.h>


void PAL_WaitForDebugger()
{
    uint16_t msWaitForDebugger = 5 * 1000;
    uint16_t msSleepPeriod = 100;
    bool fDebugBreak = false;

    for(DWORD dwTickStart = GetTickCount(); GetTickCount() - dwTickStart < msWaitForDebugger; Sleep(msSleepPeriod))
    {
       if (PAL_IsDebuggerPresent())
       {
           fDebugBreak = TRUE;
           break;
       }
    }

    if (fDebugBreak)
    {
       PAL_DebugBreak();
    }
}

bool PAL_IsDebuggerPresent()
{
    return IsDebuggerPresent();
}

void PAL_DebugBreak()
{
    DebugBreak();
}

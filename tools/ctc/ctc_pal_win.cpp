#include "stdafx.h"
#include "ctc_pal.h"
#include <Windows.h>

bool PAL_IsDebuggerPresent()
{
	return IsDebuggerPresent();
}

void PAL_DebugBreak()
{
	DebugBreak();
}



//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// trace_message.cpp: SpxTraceMessage() implementation definition
//

#include "stdafx.h"
#include "trace_message.h"
#include <chrono>
#include <stdio.h>
#include <stdarg.h>
#include <string>


decltype(std::chrono::high_resolution_clock::now()) __g_spx_trace_message_time0 = std::chrono::high_resolution_clock::now();


void SpxTraceMessage(int level, const char* pszTitle, const char* pszFormat, ...)
{
    try
    {
        va_list argptr;
        va_start(argptr, pszFormat);

        auto now = std::chrono::high_resolution_clock::now();
        unsigned long delta = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(now - __g_spx_trace_message_time0).count();

        char deltastr[20];
        _ultoa_s(delta, deltastr, sizeof(deltastr), 10);
        strcat_s(deltastr, sizeof(deltastr), "ms ");

        std::string format(deltastr);
        while (*pszFormat == '\n' || *pszFormat == '\r')
        {
            if (*pszFormat == '\r')
            {
                pszTitle = nullptr;
            }

            format += *pszFormat++;
        }

        if (pszTitle != nullptr)
        {
            format += pszTitle;
        }

        format += pszFormat;

        if (format.length() < 1 || format[format.length() - 1] != '\n')
        {
            format += "\n";
        }

        vfprintf(stderr, format.c_str(), argptr);

        va_end(argptr);
    }
    catch(...)
    {
    }
}

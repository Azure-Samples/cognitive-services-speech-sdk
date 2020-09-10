//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "memory_logger.h"
#include "file_utils.h"
#include "exception.h"
#include "time_utils.h"
#include <stdio.h>
#include <string.h>
#include <chrono>

MemoryLogger& MemoryLogger::Instance()
{
    static MemoryLogger instance;
    return instance;
}

void MemoryLogger::EnableLogging(bool enable)
{
    if (enable)
    {
        m_started++;
    }
    else if (m_started > 0)
    {
        m_started--;
    }

    auto msg = m_started == 0 ? "stopped logging"
        : enable && m_started == 1 ? "started logging"
        : enable ? "enable logging" : "disable logging";

    auto now = std::chrono::system_clock::now();
    auto time = PAL::GetTimeInString(now);

    SPX_TRACE_INFO("%s; c=%d; %s", msg, m_started, time.c_str());
}

bool MemoryLogger::IsLoggingEnabled()
{
    return m_started > 0;
}

void MemoryLogger::DumpToFile(const char* fileName, int options)
{
    SPX_IFTRUE_RETURN(!IsLoggingEnabled());
    SPX_IFTRUE_THROW_HR(options < 0 || options > 7, SPXERR_INVALID_ARG);

    bool stdOut = (options & 2) == 2;
    bool stdErr = (options & 4) == 4 || (fileName == nullptr && options == 0);

    FILE* file = nullptr;
    bool outputFile = (options & 1) == 1;
    if (outputFile && fileName != nullptr)
    {
        PAL::fopen_s(&file, fileName, "w");
        SPX_IFTRUE_THROW_HR(file == nullptr, SPXERR_FILE_OPEN_FAILED);
    }

    if ((stdOut || stdErr) && fileName == nullptr)
    {
        fileName = "CRBN";
    }

    auto start = diagnostics_log_memory_get_line_num_oldest();
    auto stop = diagnostics_log_memory_get_line_num_newest();
    for (auto i = start; i <= stop; i++)
    {
        auto line = diagnostics_log_memory_get_line(i);

        SPX_IFTRUE(stdOut, printf("%s: %s", fileName, line));
        SPX_IFTRUE(stdErr, fprintf(stderr, "%s: %s", fileName, line));

        SPX_IFTRUE(file != nullptr, fprintf(file, "CRBN: %s", line));
    }

    SPX_IFTRUE(file != nullptr, fclose(file));
}

void MemoryLogger::LogToMemory(const char *line)
{
    auto ticket = m_tickets.CreateTicket(); // == 1 -------------------------------- runs parallel with other tickets
    LogToMemoryInternal(ticket, line);
}

void MemoryLogger::LogToMemoryInternal(TicketType ticket, const char* traceLine)
{
    auto traceLineCch = strlen(traceLine) + 1;

    m_tickets.AdvanceStep(ticket); // == 2 ----------------------------------------- sequence/lock step with other tickets

    SPX_IFTRUE(m_bufferPtr + traceLineCch + 1 > m_bufferEnd, m_bufferPtr = m_bufferStart);
    auto lineBufferPtr = m_bufferPtr;
    m_bufferPtr += traceLineCch + 1;

    strcpy(lineBufferPtr, traceLine);
    lineBufferPtr[traceLineCch + 1] = '\0';

    auto lineNum = ticket % m_maxLines;
    m_traceLinePtrs[lineNum] = lineBufferPtr;
    m_numLines++;

    m_tickets.DisposeTicket(ticket); // == DONE ------------------------------------ sequence/lock step with other tickets
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "memory_logger.h"
#include "file_utils.h"
#include "exception.h"
#include <stdio.h>
#include <string.h>

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
    SPX_TRACE_INFO_IF("%s; c=%d", msg, m_started);
}

bool MemoryLogger::IsLoggingEnabled()
{
    return m_started > 0;
}

void MemoryLogger::DumpToFile(const char* fileName, int options)
{
    SPX_IFTRUE_RETURN(!IsLoggingEnabled());
    SPX_IFTRUE_THROW_HR(options != 1, SPXERR_INVALID_ARG); // for future use

    FILE* file = fileName == nullptr ? stderr : nullptr;
    SPX_IFTRUE(fileName != nullptr, PAL::fopen_s(&file, fileName, "w"));
    SPX_IFTRUE_THROW_HR(file == nullptr, SPXERR_FILE_OPEN_FAILED);

    auto start = diagnostics_log_memory_get_line_num_oldest();
    auto stop = diagnostics_log_memory_get_line_num_newest();
    for (auto i = start; i <= stop; i++)
    {
        auto line = diagnostics_log_memory_get_line(i);
        fprintf(file, "DUMP: %s", line);
    }

    SPX_IFTRUE(fileName != nullptr, fclose(file));
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

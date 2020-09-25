//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <string>
#include <iostream>
#include "log_utils.h"
#include "speechapi_c_diagnostics.h"

#pragma once

template<class TicketType = size_t>
class MultiStepTicketQueue
{
public:

    MultiStepTicketQueue()
    {
        std::memset(m_ticketStep, startStep, maxTickets * sizeof(m_ticketStep[0]));
    }

    TicketType CreateTicket()
    {
        auto ticket = m_nextTicket.fetch_add(1);
        auto slot = ticket % maxTickets;

        SPX_DBG_ASSERT(m_ticketStep[slot] == startStep);
        m_ticketStep[slot] = startStep + 1;

        return ticket;
    }

    uint8_t AdvanceStep(TicketType ticket)
    {
        auto slot = ticket % maxTickets;
        return AdvanceStepInternal(ticket, m_ticketStep[slot] + 1);
    }

    void DisposeTicket(TicketType ticket)
    {
        AdvanceStepInternal(ticket, disposedStep);

        auto prevSlot = ticket == 0
            ? maxTickets - 1
            : (ticket - 1) % maxTickets;

        SPX_DBG_ASSERT(m_ticketStep[prevSlot] == disposedStep || ticket == 0);
        m_ticketStep[prevSlot] = startStep;
    }

protected:

    uint8_t AdvanceStepInternal(TicketType ticket, uint8_t newStage)
    {
        auto slot = ticket % maxTickets;
        auto prevSlot = ticket == 0
            ? maxTickets - 1
            : (ticket - 1) % maxTickets;

        auto unblocked = (ticket == 0)
            || (m_ticketStep[prevSlot] > newStage)
            || (newStage == disposedStep && m_ticketStep[prevSlot] == disposedStep);
        
        auto looped = 0;
        for (;;)
        {
            unblocked = (m_ticketStep[prevSlot] > newStage)
                || (newStage == disposedStep && m_ticketStep[prevSlot] == disposedStep)
                || (ticket == 0);

            if (unblocked) break;

            constexpr auto maxSpinCount = 100;
            if (looped++ == maxSpinCount)
            {
                std::this_thread::yield();
                looped = 0;
            }
        }

        m_ticketStep[slot] = newStage;

        SPX_DBG_ASSERT(ticket == 0
            || m_ticketStep[prevSlot] > m_ticketStep[slot]
            || m_ticketStep[prevSlot] == disposedStep);

        return newStage;
    }

private:

    static constexpr uint8_t startStep = 0;
    static constexpr uint8_t disposedStep = UINT8_MAX;

    static constexpr size_t maxTickets = 1024;
    uint8_t m_ticketStep[maxTickets];

    std::atomic<TicketType> m_nextTicket{ 0 };
};

class MemoryLogger
{
public:

    static MemoryLogger& Instance();

    MemoryLogger() = default;
    MemoryLogger(MemoryLogger const&) = delete;
    void operator=(MemoryLogger const&) = delete;

    void Dump(const char* filename, const char* linePrefix, bool emitToStdOut, bool emitToStdErr);

    void EnableLogging(bool enable);
    bool IsLoggingEnabled();

    void LogToMemory(const char *line);

    size_t GetLineNumOldest() const { return m_numLines >= m_maxLines ? m_numLines - m_maxLines : 0; }
    size_t GetLineNumNewest() const { return m_numLines; }

    const char* GetLine(size_t lineNum) { return m_traceLinePtrs[lineNum % m_maxLines]; }

private:

    using TicketType = size_t;
    void LogToMemoryInternal(TicketType ticket, const char* traceLine);

    size_t m_started = 0;

    static constexpr size_t m_maxLines = 10000;
    static constexpr size_t avgCchPerLine = 200;
    static constexpr size_t bufferCch = m_maxLines * avgCchPerLine;
    char m_buffer[bufferCch + 1] = { '\0' };

    char* m_bufferStart = m_buffer;
    char* m_bufferEnd = m_buffer + bufferCch;
    char* m_bufferPtr = m_buffer;

    char* m_traceLinePtrs[m_maxLines] = { nullptr };
    size_t m_numLines = 0;

    MultiStepTicketQueue<TicketType> m_tickets;
};

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "event_logger.h"
#include "exception.h"
#include <stdio.h>
#include <string.h>

void EventLogger::SetLogFilters(const char *filters)
{
    filter->SetFilter(filters);
}

bool EventLogger::IsLoggingEnabled()
{
    ReadLock myLock(&targetLock);
    return callback != NULL;
}

void EventLogger::LogToEvent(const char *logLine)
{
    if (!filter->ShouldLog(logLine))
    {
        return;
    }

    ReadLock myLock(&targetLock);
    if (NULL != callback)
    {
        callback(logLine);
    }
}

void EventLogger::AttachLogTarget(DIAGNOSTICS_CALLBACK_FUNC target)
{
    WriteLock myLock(&targetLock);
    callback = target;
}

EventLogger& EventLogger::Instance()
{
    static EventLogger instance;
    return instance;
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// event_log.h: Class to log to a user registered event.
//

#include <string>
#include <iostream>
#include "log_utils.h"
#include "speechapi_c_diagnostics.h"

#pragma once

class EventLogger
{
public:
    static EventLogger& Instance();

    EventLogger(EventLogger const&) = delete;
    void operator=(EventLogger const&) = delete;

    void SetLogFilters(const char *filters);
    void AttachLogTarget(DIAGNOSTICS_CALLBACK_FUNC target);
    
    bool IsLoggingEnabled();
    void LogToEvent(const char *format);
private:
    EventLogger()
    {
        filter = std::make_shared<LogFilter>();
    };

    ReaderWriterLock targetLock;

    std::shared_ptr<LogFilter> filter;

    DIAGNOSTICS_CALLBACK_FUNC callback;
};

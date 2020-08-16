//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include <property_id_2_name_map.h>
#include "handle_helpers.h"
#include "file_logger.h"
#include "event_logger.h"
#include "trace_message.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;

/// <summary>
/// Defined in speechapi_c_property_bag.cpp
/// </summary>
std::shared_ptr<ISpxNamedProperties> property_bag_from_handle(SPXPROPERTYBAGHANDLE hpropbag);

static inline std::shared_ptr<ISpxNamedProperties> GetProperties(SPXPROPERTYBAGHANDLE hpropbag, void* reserved)
{
    auto properties = reserved == nullptr
        ? property_bag_from_handle(hpropbag)
        : SpxSharedPtrFromThis((ISpxNamedProperties*)reserved);

    SPX_THROW_HR_IF(SPXERR_INVALID_ARG, properties == nullptr);
    return properties;
}

SPXAPI diagnostics_log_start_logging(SPXPROPERTYBAGHANDLE hpropbag, void* reserved)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag != SPXHANDLE_INVALID && reserved != nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto properties = GetProperties(hpropbag, reserved);

        FileLogger::Instance().SetFileOptions(properties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI diagnostics_log_apply_properties(SPXPROPERTYBAGHANDLE hpropbag, void* reserved)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag != SPXHANDLE_INVALID && reserved != nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto properties = GetProperties(hpropbag, reserved);
        FileLogger::Instance().SetFileOptions(properties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI diagnostics_log_stop_logging()
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        FileLogger::Instance().CloseFile();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(void) diagnostics_log_trace_message(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, ...)
{
    try
    {
        va_list argptr;
        va_start(argptr, pszFormat);
        SpxTraceMessage2(level, pszTitle, fileName, lineNumber, pszFormat, argptr);
        va_end(argptr);
    }
    catch(...)
    {
    }
}

SPXAPI_(void) diagnostics_log_trace_message2(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, va_list argptr)
{
    try
    {
        SpxTraceMessage2(level, pszTitle, fileName, lineNumber, pszFormat, argptr);
    }
    catch(...)
    {
    }
}

SPXAPI_(void) diagnostics_log_format_message(char *buffer, size_t bufferSize, int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, va_list argptr)
{
    try
    {
        SpxFormatMessage(buffer, bufferSize, level, pszTitle, fileName, lineNumber, pszFormat, argptr);
    }
    catch (...)
    {
    }
}

SPXAPI diagnostics_logmessage_set_callback(DIAGNOSTICS_CALLBACK_FUNC callback)
{
    EventLogger::Instance().AttachLogTarget(callback);
    return SPX_NOERROR;
}

SPXAPI diagnostics_logmessage_set_filters(const char *filters)
{
    EventLogger::Instance().SetLogFilters(filters);
    return SPX_NOERROR;
}

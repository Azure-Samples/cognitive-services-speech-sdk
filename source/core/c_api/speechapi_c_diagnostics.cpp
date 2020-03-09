//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include "handle_helpers.h"
#include "file_logger.h"
#include "trace_message.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI diagnostics_log_start_logging(SPXPROPERTYBAGHANDLE hpropbag, void* reserved)
{
    return diagnostics_log_apply_properties(hpropbag, reserved);
}

SPXAPI diagnostics_log_apply_properties(SPXPROPERTYBAGHANDLE hpropbag, void* reserved)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag != SPXHANDLE_INVALID && reserved != nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto properties = reserved == nullptr
            ? CSpxSharedPtrHandleTableManager::GetPtr<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>(hpropbag)
            : SpxSharedPtrFromThis((ISpxNamedProperties*)reserved);
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
    UNUSED(level);
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

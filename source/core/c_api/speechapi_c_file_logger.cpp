//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_file_logger.cpp: Internal API definitions for the file logger. These are not exposed through any headers
//

#include "stdafx.h"
#include <file_logger.h>
#include "handle_helpers.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI file_logger_start_logging(SPXSPEECHCONFIGHANDLE hspeechconfig)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_HANDLE, hspeechconfig == SPXHANDLE_INVALID);

    SPXHR hr = SPX_NOERROR;
    SPXPROPERTYBAGHANDLE hpropbag = SPXHANDLE_INVALID;

    try
    {
        SPX_THROW_ON_FAIL(speech_config_get_property_bag(hspeechconfig, &hpropbag));
        auto namedProperties = GetInstance<ISpxNamedProperties>(hpropbag);

        auto& fileLogger = FileLogger::Instance();
        fileLogger.SetFileOptions(namedProperties);
    }
    SPXAPI_CATCH(hr);

    if (hpropbag != SPXHANDLE_INVALID)
    {
        SPX_REPORT_ON_FAIL(property_bag_release(hpropbag));
    }

    return hr;
}

SPXAPI file_logger_stop_logging()
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto& fileLogger = FileLogger::Instance();
        if (fileLogger.IsFileLoggingEnabled())
        {
            fileLogger.CloseFile();
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

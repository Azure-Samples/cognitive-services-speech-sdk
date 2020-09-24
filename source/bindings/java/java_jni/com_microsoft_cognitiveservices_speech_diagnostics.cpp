//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "com_microsoft_cognitiveservices_speech_diagnostics.h"
#include "speechapi_c_diagnostics.h"
#include "jni_utils.h"

JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Diagnostics_logMemoryStartLogging(JNIEnv*, jclass)
{
    diagnostics_log_memory_start_logging();
    return SPX_NOERROR;
}

JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Diagnostics_logMemoryStopLogging(JNIEnv*, jclass)
{
    diagnostics_log_memory_stop_logging();
    return SPX_NOERROR;
}

JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Diagnostics_logMemoryDumpToFile(JNIEnv* env, jclass, jstring filename, jint options)
{
    const char* pszFilename = nullptr;
    SPXHR result = SPX_NOERROR;
    try
    {
        pszFilename = GetStringUTFChars(env, filename);
        diagnostics_log_memory_dump_to_file(pszFilename, static_cast<int>(options));
    }
    catch (...)
    {
        result = SPXERR_RUNTIME_ERROR;
    }

    ReleaseStringUTFChars(env, filename, pszFilename);
    return result;
}

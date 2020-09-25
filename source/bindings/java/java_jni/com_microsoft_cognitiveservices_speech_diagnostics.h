//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Diagnostics_logMemoryStartLogging
    (JNIEnv *, jclass);

JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Diagnostics_logMemoryStopLogging
    (JNIEnv *, jclass);

JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_Diagnostics_logMemoryDump
    (JNIEnv *, jclass, jstring, jstring, jboolean, jboolean);

#ifdef __cplusplus
}
#endif

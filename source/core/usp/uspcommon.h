//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspcommon.h: common definitions and declaration used by USP internal implementation
//

#pragma once

#include <stddef.h>
#include "usperror.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USE_BUFFER_SIZE    ((size_t)-1)

#if defined _MSC_VER
#define PROTOCOL_VIOLATION(__fmt, ...)  LogError("ProtocolViolation:" __fmt, __VA_ARGS__)
#else
#define PROTOCOL_VIOLATION(__fmt, ...)  LogError("ProtocolViolation:" __fmt, ##__VA_ARGS__)
#endif

extern const char* g_keywordContentType;
extern const char* g_messagePathSpeechHypothesis;
extern const char* g_messagePathSpeechPhrase;
extern const char* g_messagePathSpeechFragment;
extern const char* g_messagePathTurnStart;
extern const char* g_messagePathTurnEnd;
extern const char* g_messagePathSpeechEndDetected;
extern const char* g_messagePathSpeechStartDetected;

#define USP_THROW_IF_ARGUMENT_NULL(argument) \
        if (argument == NULL) \
        { \
            std::ostringstream ss; \
            ss << "The argument '" << #argument << "' is null."; \
            throw std::invalid_argument(ss.str()); \
        }

#define USP_RETURN_ERROR_IF_CONTEXT_NULL(context) \
    do { \
        if (context == NULL) \
        { \
            LogError("Context is null."); \
            return USP_INVALID_ARGUMENT; \
        } \
    } while (0)


#define USP_RETURN_NOT_IMPLEMENTED() \
    do { \
        LogError("Not implemented"); \
        return USP_NOT_IMPLEMENTED; \
    } while (0)


typedef struct BUFFER_TAG* BUFFER_HANDLE;
typedef struct _IOBUFFER IOBUFFER;

/**
* Handles response messages from service based on content type.
* @param context The content context.
* @param path The content path.
* @param mime The content type.
* @param ioBuffer The pointer to ioBuffer.
* @param responseContent The content buffer of the response.
* @param responseSize The size of responseContent.
* @return A UspResult indicating success or error.
*/
UspResult ContentDispatch(
    void* context,
    const char* path,
    const char* mime,
    IOBUFFER* ioBuffer,
    BUFFER_HANDLE responseContent,
    size_t responseSize);

#ifdef __cplusplus
} // extern "C" 
#endif

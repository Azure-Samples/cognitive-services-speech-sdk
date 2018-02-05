//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspinternal.h: the header file only used by usplib internally
//

#pragma once

#include <stdint.h>
#if defined(__unix__) || defined(TARGET_OS_IPHONE) || defined (TARGET_OS_MAC)
#include <stddef.h> // for size_t
#endif

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdlib.h>
#include <memory.h>
#ifdef __linux__
#include <unistd.h>
#include <errno.h>
#endif

#include <assert.h>

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/list.h"
#include "usp.h"
#include "uspcommon.h"
#include "metrics.h"
#include "iobuffer.h"
#include "transport.h"
#include "tokenstore.h"
#include "dnscache.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(__a) (sizeof(__a) / sizeof(__a[0]))
#endif

#ifdef __linux__
#define localtime_s(__tm, __timet) localtime_r(__timet, __tm)
#define gmtime_s(__tm, __timet) gmtime_r(__timet, __tm)
#define scanf_s scanf
#define sscanf_s sscanf
#endif

#define USP_RETURN_ERROR_IF_HANDLE_NULL(uspHandle) \
    do { \
        if (uspHandle == NULL) \
        { \
            LogError("The UspHandle is null."); \
            return USP_INVALID_HANDLE; \
        } \
    } while (0)

#define USP_RETURN_ERROR_IF_ARGUMENT_NULL(argument, argumentName) \
    do { \
        if (argument == NULL) \
        { \
            LogError("The argument '%s' is null.", argumentName); \
            return USP_INVALID_ARGUMENT; \
        } \
    } while (0)

#define USP_RETURN_ERROR_IF_CONTEXT_NULL(context) \
    do { \
        if (context == NULL) \
        { \
            LogError("Context is null."); \
            return USP_INVALID_ARGUMENT; \
        } \
    } while (0)

#define USP_RETURN_ERROR_IF_CALLBACKS_NULL(context) \
    do { \
        if (context->callbacks == NULL) \
        { \
            LogError("User callbacks are null."); \
            return USP_CALLBACKS_NOT_SET; \
        } \
    } while (0)

#define USP_RETURN_VOID_IF_CONTEXT_NULL(context) \
    do { \
        if (context == NULL) \
        { \
            LogError("Context is null."); \
            return; \
        } \
    } while (0)

#define USP_RETURN_VOID_IF_CALLBACKS_NULL(context) \
    do { \
        if (context->callbacks == NULL) \
        { \
            LogError("User callbacks are null."); \
            return; \
        } \
    } while (0)

#define USP_RETURN_NOT_IMPLEMENTED() \
    do { \
        LogError("Not implemented"); \
        return USP_NOT_IMPLEMENTED; \
    } while (0)

#ifdef WIN32
// Convert performance counter to microseconds. First coverting to microsecond before dividing
#define USP_LIFE_TIME(uspContext) ((telemetry_gettime() - uspContext->creationTime)*1000000/g_perfCounterFrequency)
#else
#define USP_LIFE_TIME(uspContext) (telemetry_gettime() - uspContext->creationTime)
#endif

typedef struct _UserPathHandler
{
    char* path;
    UspOnUserMessage handler;
} UserPathHandler;

/**
* The UspState represents the state of UspHandle.
*/
typedef enum _USP_STATE
{
    USP_STATE_NOT_INITIALIZED,
    USP_STATE_INITIALIZED,
    USP_STATE_CONNECTED,
    USP_STATE_SHUTDOWN
} UspState;

/**
* The UspContext represents the context data related to a USP client.
*/
typedef struct _UspContext
{
    UspCallbacks* callbacks;
    void* callbackContext;

    UspEndpointType type;
    UspRecognitionMode mode;
    STRING_HANDLE outputFormat;
    STRING_HANDLE language;
    STRING_HANDLE modelId;

    STRING_HANDLE endpointUrl;

    UspAuthenticationType authType;
    STRING_HANDLE authData;

    UspState state;

    LIST_HANDLE userPathHandlerList;

    // Todo: can multiple UspContexts share the work thread?
    THREAD_HANDLE workThreadHandle;

    size_t audioOffset;
    LOCK_HANDLE uspContextLock;
    TransportHandle transportRequest;
    DnsCacheHandle dnsCache;

    // This tick count is set when the UspContext is created.  It is used
    // for metrics.
    uint64_t creationTime;
} UspContext;

/**
* Runs the event loop.
* @param uspHandle The uspHandle.
* @return the current state of the uspHandle.
*
*/
UspState UspRun(UspHandle uspHandle);

/**
* Creates a new UspContext.
* @param contextCreated The pointer that points to the pointer of the created UspContext on return.
* @return A UspResult indicating success or error.
*/
UspResult UspContextCreate(UspContext** contextCreated);

/**
* Destroys the specified UspContext.
* @param uspContext A pointer to the UspContext to be destroyed.
* @return A UspResult indicating success or error.
*/
UspResult UspContextDestroy(UspContext* uspContext);

/**
* Sets the callback table in the specified UspContext.
* @param uspContext A pointer to the UspContext.
* @param callbacks A pointer to the callback table.
* @param callbackContext The context that will be passed as parameter when one of callbacks is invoked.
* @return A UspResult indicating success or error.
*/
UspResult UspSetCallbacks(UspContext* uspContext, UspCallbacks *callbacks, void* callbackContext);

/**
* Initializes transport connection to service.
* @param uspContext A pointer to the UspContext.
* @param endpoint The endpoint URL.
* @return A UspResult indicating success or error.
*/
UspResult TransportInitialize(UspContext* uspContext, const char* endpoint);

/**
* Tears down transport connection to service.
* @param uspContext A pointer to the UspContext.
* @return A UspResult indicating success or error.
*/
UspResult TransportShutdown(UspContext* uspContext);

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

/**
* Return device thumbprint generated from CDP
*/
const char* GetCdpDeviceThumbprint();

/**
* Writes an audio segment to the service.
* @param uspHandle the UspHandle for sending the audio.
* @param data The audio data to be sent. Audio data must be aligned on the audio sample boundary.
* @param size The length of the audio data, in bytes.
* @param bytesWritten A returned pointer to the amount of data that was sent to the service.
* @return A UspResult indicating success or error.
*/
UspResult AudioStreamWrite(UspHandle uspHandle, const void *data, uint32_t size, uint32_t * bytesWritten);

/**
* Flushes any pending audio to be sent to the service.
* @param uspHandle the UspHandle for sending the audio.
* @return A UspResult indicating success or error.
*/
UspResult AudioStreamFlush(UspHandle uspHandle);

#ifdef __cplusplus
} // extern "C" 
#endif

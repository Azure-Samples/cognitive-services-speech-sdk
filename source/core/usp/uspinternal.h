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

#include "azure_c_shared_utility/threadapi.h"
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

/**
 * USP_FLAG_XXX indicates state of the USP context.
*/
#define USP_FLAG_INITIALIZED 0x01
#define USP_FLAG_SHUTDOWN    0x02

/**
* The UspContext represents the context data related to a USP client.
*/
typedef struct _UspContext
{
    UspCallbacks* callbacks;
    void* callbackContext;

    int flags;
    // Todo: can multiple UspContexts share the work thread?
    THREAD_HANDLE workThreadHandle;

    size_t audioOffset;
    LOCK_HANDLE transportRequestLock;
    TransportHandle transportRequest;
    DnsCacheHandle dnsCache;
    TokenStore token_store;

    // This tick count is set when the UspContext is created.  It is used
    // for metrics.
    uint64_t createTime;
} UspContext;


/**
* Creates a new UspContext.
* @param contextCreated The pointer that points to the pointer of the created UspContext on return.
* @param endpoint The service endpoint.
* @return A UspResult indicating success or error.
*/
UspResult UspContextCreate(UspContext** contextCreated, const char* endpoint);

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
* Defines the callback function of asynchrnous complete during content handling.
* @param context The context provided by the application.
*/
typedef void(*CONTENT_ASYNCCOMPLETE_CALLBACK)(void* context);

/**
* Defines the callback function of handling contents.
* @param context The content context.
* @param path The content path.
* @param buffer The content buffer.
* @param bufferSize The size of the buffer.
* @param ioBuffer The pointer to ioBuffer.
* @param asyncCompleteCallback The callback when handling is complete.
* @param asyncCompleteContext The context parameter that is passed when the asyncCompleteCallback is invoked.
* @return A UspResult indicating success or error.
*/
typedef UspResult(*CONTENT_HANDLER_CALLBACK)(void* context, const char* path, uint8_t* buffer, size_t bufferSize, IOBUFFER* ioBuffer, CONTENT_ASYNCCOMPLETE_CALLBACK asyncCompleteCallback, void* asyncCompleteContext);


/**
* Opens a new Speech instance.
* @param ppHandle A pointer to a Speech handle to be returned back to the
* caller.
* @param reserved Reserved, do not use.
* @param pReserved Reserved, do not use.
* @return A UspResult.
*/
UspResult ContentDispatch(void* context, const char* path, const char* mime, IOBUFFER* ioBuffer, BUFFER_HANDLE responseContent, size_t responseSize);

/**
* Handles JSON responses.
* @param context The content context.
* @param path The content path.
* @param buffer The content buffer.
* @param bufferSize The size of the buffer.
* @param ioBuffer The pointer to ioBuffer.
* @param asyncCompleteCallback The callback when handling is complete.
* @param asyncCompleteContext The context parameter that is passed when the asyncCompleteCallback is invoked.
* @return A UspResult indicating success or error.
*/
UspResult JsonResponseHandler(void* context, const char* path, uint8_t* buffer, size_t bufferSize, IOBUFFER* ioBuffer, CONTENT_ASYNCCOMPLETE_CALLBACK asyncCompleteCallBack, void* asyncCompleteContext);

/**
* Handles text responses.
* @param context The content context.
* @param path The content path.
* @param buffer The content buffer.
* @param bufferSize The size of the buffer.
* @param ioBuffer The pointer to ioBuffer.
* @param asyncCompleteCallback The callback when handling is complete.
* @param asyncCompleteContext The context parameter that is passed when the asyncCompleteCallback is invoked.
* @return A UspResult indicating success or error.
*/
UspResult TextResponseHandler(void* context, const char* path, uint8_t* buffer, size_t bufferSize, IOBUFFER* ioBuffer, CONTENT_ASYNCCOMPLETE_CALLBACK asyncCompleteCallBack, void* asyncCompleteContext);


/**
* Return device thumbprint generated from CDP
*/
const char* GetCdpDeviceThumbprint();

/**
* Writes an audio segment to the service.
* @param handle the UspHandle for sending the audio.
* @param data The audio data to be sent. Audio data must be aligned on the audio sample boundary.
* @param size The length of the audio data, in bytes.
* @param bytesWritten A returned pointer to the amount of data that was sent to the service.
* @return A UspResult indicating success or error.
*/
UspResult AudioStreamWrite(UspHandle handle, const void *data, uint32_t size, uint32_t * bytesWritten);

/**
* Flushes any pending audio to be sent to the service.
* @param handle the UspHandle for sending the audio.
* @return A UspResult indicating success or error.
*/
UspResult AudioStreamFlush(UspHandle handle);

#ifdef __cplusplus
} // extern "C" 
#endif

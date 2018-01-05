//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// dnscache.h: definitions and declaration of DNS cache methods
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DNS_CONTEXT* DnsCacheHandle;
typedef void* DNS_RESULT_HANDLE;
typedef void(*DNS_CACHE_NAMERESOLVEDCB)(DnsCacheHandle handle, int error, DNS_RESULT_HANDLE result, void *context);

DnsCacheHandle DnsCacheCreate();

void DnsCacheDestroy(DnsCacheHandle handle);

int DnsCacheGetAddr(DnsCacheHandle handle, const char* host, DNS_CACHE_NAMERESOLVEDCB resolvedCallback, void *context);

void DnsCacheDoWork(DnsCacheHandle handle, void* contextToMatch);

void DnsCacheRemoveContextMatches(DnsCacheHandle handle, void* context);

void DnsCacheResultFree(DNS_RESULT_HANDLE handle);

int DnsCacheLookup(const char* host, DNS_RESULT_HANDLE* result);

#ifdef __cplusplus
} 
#endif

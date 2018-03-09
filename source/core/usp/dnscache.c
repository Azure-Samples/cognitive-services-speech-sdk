//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// dnscache.c: implementation of DNS lookup and cache results.
//

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <assert.h>
#include <stdlib.h>
#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/condition.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "dnscache.h"

void DnsCacheResultFree(DNS_RESULT_HANDLE handle)
{
#ifdef __linux__
    freeaddrinfo((struct addrinfo*)handle);
#else
    (void)handle;
#endif
}

int DnsCacheLookup(const char* host, DNS_RESULT_HANDLE* result)
{
#ifdef __linux__
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));

    return getaddrinfo(host, NULL, &hints, (struct addrinfo**)result);

#else
    // TODO: intentionally not implemented right now.
    (void)host;
    static int dummy = 0;
    *result = (DNS_RESULT_HANDLE)&dummy;
    return dummy;
#endif
}

typedef struct _DNS_REQUEST
{
    char* host;
    DNS_CACHE_NAMERESOLVEDCB resolvedCallback;
    void *context;
    int result;
    DNS_RESULT_HANDLE resultHandle;
} DNS_REQUEST;

#define DNS_CTX_FLAG_SHUTDOWN   1

typedef struct _DNS_CONTEXT
{
    THREAD_HANDLE thread;
    LOCK_HANDLE   entryLock;
    SINGLYLINKEDLIST_HANDLE   entryList;
    SINGLYLINKEDLIST_HANDLE   completeList;
    COND_HANDLE   workEvent;
    DNS_REQUEST*  currentRequest;
    int           flags;
} DNS_CONTEXT;

static void dns_cache_free_request(DNS_REQUEST* request)
{
    if (request->resultHandle)
    {
        DnsCacheResultFree(request->resultHandle);
    }

    free(request);
}

static int DnsWorker(void *args)
{
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)args;

    Lock(ctx->entryLock);
    do
    {
        LIST_ITEM_HANDLE list_item;
        while (NULL != (list_item = singlylinkedlist_get_head_item(ctx->entryList)))
        {
            DNS_REQUEST* req = (DNS_REQUEST*)singlylinkedlist_item_get_value(list_item);
            // This is the only thread that sets the current request.
            assert(ctx->currentRequest == NULL);
            ctx->currentRequest = req;

            Unlock(ctx->entryLock);
            req->result = DnsCacheLookup(req->host, &req->resultHandle);
            Lock(ctx->entryLock);

            // The current request may have been cleared if the caller lost
            // interest in the result.
            if (ctx->currentRequest != NULL)
            {
                assert(ctx->currentRequest == req);
                ctx->currentRequest = NULL;

                singlylinkedlist_add(ctx->completeList, req);
            }
            else
            {
                dns_cache_free_request(req);
            }

            singlylinkedlist_remove(ctx->entryList, list_item);
        }

        if (ctx->flags & DNS_CTX_FLAG_SHUTDOWN)
        {
            break;
        }

        // If we got here, then there's nothing in the request queue.  Wait for
        // new work and loop back.
    } while (COND_OK == Condition_Wait(ctx->workEvent, ctx->entryLock, 0));
    Unlock(ctx->entryLock);

    return 0;
}

static void SignalWork(DNS_CONTEXT* ctx)
{
    Lock(ctx->entryLock);
    Condition_Post(ctx->workEvent);
    Unlock(ctx->entryLock);
}

DnsCacheHandle DnsCacheCreate()
{
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)malloc(sizeof(DNS_CONTEXT));

    memset(ctx, 0, sizeof(DNS_CONTEXT));

    ctx->entryList = singlylinkedlist_create();
    ctx->completeList = singlylinkedlist_create();
    ctx->entryLock = Lock_Init();
    ctx->workEvent = Condition_Init();

    if (ctx->entryList     &&
        ctx->completeList  &&
        ctx->entryLock      &&
        ctx->workEvent     &&
        ThreadAPI_Create(&ctx->thread, DnsWorker, ctx) == THREADAPI_OK)
    {
        return ctx;
    }

    DnsCacheDestroy(ctx);
    return NULL;
}

void DnsCacheDestroy(DnsCacheHandle handle)
{
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)handle;

    ctx->flags |= DNS_CTX_FLAG_SHUTDOWN;

    // wake up the worker thread
    if (NULL != ctx->workEvent)
    {
        SignalWork(ctx);
    }

    if (NULL != ctx->thread)
    {
        ThreadAPI_Join(ctx->thread, NULL);
    }

    if (NULL != ctx->completeList)
    {
        singlylinkedlist_destroy(ctx->completeList);
    }

    if (NULL != ctx->entryList)
    {
        singlylinkedlist_destroy(ctx->entryList);
    }

    if (NULL != ctx->workEvent)
    {
        Condition_Deinit(ctx->workEvent);
    }

    if (NULL != ctx->entryLock)
    {
        Lock_Deinit(ctx->entryLock);
    }

    free(ctx);
}

int DnsCacheGetAddr(DnsCacheHandle handle, const char* host, DNS_CACHE_NAMERESOLVEDCB resolvedCallback, void *context)
{
    size_t len;
    DNS_REQUEST* req;
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)handle;

    if (!handle || !host || !resolvedCallback)
    {
        return -1;
    }

    // create a new request
    len = strlen(host);

    req = (DNS_REQUEST*)malloc(sizeof(DNS_REQUEST) + len + 1);
    if (NULL == req)
    {
        return -1;
    }

    req->resolvedCallback = resolvedCallback;
    req->context = context;
    req->host = (char*)&req[1];
    req->resultHandle = NULL;
    strcpy_s(req->host, len + 1, host);

    // queue it up
    Lock(ctx->entryLock);
    singlylinkedlist_add(ctx->entryList, req);
    Unlock(ctx->entryLock);

    // tell the worker to get started on it.
    SignalWork(ctx);

    return 0;
}

static bool dns_cache_dequeue_context_match_callback(LIST_ITEM_HANDLE item, const void* context)
{
    const DNS_REQUEST* const request =
        (const DNS_REQUEST*)singlylinkedlist_item_get_value(item);
    return request->context == context;
}

static DNS_REQUEST* dns_cache_dequeue_context_match(SINGLYLINKEDLIST_HANDLE list, void* context)
{
    DNS_REQUEST* req = NULL;
    const LIST_ITEM_HANDLE item =
        singlylinkedlist_find(list, dns_cache_dequeue_context_match_callback, context);

    if (item != NULL)
    {
        req = (DNS_REQUEST*)singlylinkedlist_item_get_value(item);

        singlylinkedlist_remove(list, item);
    }
    return req;
}

/*
 * Polls for any work that's completed for looking up DNS entries.
 * Worker thread will run through each requested DNS host, look up the address, 
 * and then post back to the callers thread.
 */
void DnsCacheDoWork(DnsCacheHandle handle, void* contextToMatch)
{
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)handle;
    DNS_REQUEST* req;
    if (NULL != ctx)
    {
        do
        {
            // dequeue a completed work item
            Lock(ctx->entryLock);
            req = dns_cache_dequeue_context_match(ctx->completeList, contextToMatch);
            Unlock(ctx->entryLock);

            if (NULL != req)
            {
                // call the user back with the result (currently not used but
                // we could expose another api to look up the address later)
                if (req->resolvedCallback)
                {
                    req->resolvedCallback(handle, req->result, req->resultHandle, req->context);
                }

                dns_cache_free_request(req);
            }
        } while (NULL != req);
    }
}

static void remove_all_context_matches_from_list(SINGLYLINKEDLIST_HANDLE list, void* context)
{
    LIST_ITEM_HANDLE current = singlylinkedlist_get_head_item(list);

    while (current != NULL)
    {
        // Cache the next pointer now because we may remove current from the
        // list.
        const LIST_ITEM_HANDLE next = singlylinkedlist_get_next_item(current);

        DNS_REQUEST* const req = (DNS_REQUEST*)singlylinkedlist_item_get_value(current);
        if (req->context == context)
        {
            singlylinkedlist_remove(list, current);

            dns_cache_free_request(req);
        }

        current = next;
    }
}

/*
 * Find all DNS requests that have the supplied context and remove them.  This
 * function is intended to clear out callbacks when its context is deleted.
 */
void DnsCacheRemoveContextMatches(DnsCacheHandle handle, void* context)
{
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)handle;

    Lock(ctx->entryLock);

    remove_all_context_matches_from_list(ctx->entryList, context);
    remove_all_context_matches_from_list(ctx->completeList, context);

    if (ctx->currentRequest && (ctx->currentRequest->context == context))
    {
        // The DnsWorker thread is still using this request -- signal that
        // thread to ignore and free the request.
        ctx->currentRequest = NULL;
    }

    Unlock(ctx->entryLock);
}


#include "private-iot-cortana-sdk.h"
#include <assert.h>
#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

///////////////////////////////////////////////////////////////////////////////

void dns_cache_result_free(DNS_RESULT_HANDLE handle)
{
#ifdef __linux__
    freeaddrinfo((struct addrinfo*)handle);
#else
    (void)handle;
#endif
}

int dns_cache_lookup(
    const char* pszHost,
    DNS_RESULT_HANDLE* result)
{
#ifdef __linux__
    struct addrinfo hints;
    int ret;

    memset(&hints, 0, sizeof(hints));

    if (0 != (ret = getaddrinfo(pszHost, NULL, &hints, (struct addrinfo**)result)))
    {
        return ret;
    }
    return ret;
#else
    // TODO: intentionally not implemented right now.
    (void)pszHost;
    static int dummy = 0;
    *result = (DNS_RESULT_HANDLE)&dummy;
    return dummy;
#endif
}

///////////////////////////////////////////////////////////////////////////////

typedef struct _DNS_REQUEST
{
    char* pszHost;
    DNS_CACHE_NAMERESOLVEDCB pfnResolvedCB;
    void *pContext;
    int res;
    DNS_RESULT_HANDLE hResult;
} DNS_REQUEST;

#define DNS_CTX_FLAG_SHUTDOWN   1

typedef struct _DNS_CONTEXT
{
    THREAD_HANDLE hThread;
    LOCK_HANDLE   EntryLock;
    LIST_HANDLE   hEntryList;
    LIST_HANDLE   hCompleteList;
    COND_HANDLE   hWorkEvent;
    DNS_REQUEST*  currentRequest;
    int           flags;
} DNS_CONTEXT;

static void dns_cache_free_request(DNS_REQUEST* request)
{
    if (request->hResult)
    {
        dns_cache_result_free(request->hResult);
    }

    free(request);
}

static int DnsWorker(void *args)
{
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)args;
    DNS_REQUEST* req;

    Lock(ctx->EntryLock);
    do
    {
        req = (DNS_REQUEST*)queue_dequeue(ctx->hEntryList);
        while (req != NULL)
        {
            // This is the only thread that sets the current request.
            assert(ctx->currentRequest == NULL);
            ctx->currentRequest = req;

            Unlock(ctx->EntryLock);
            req->res = dns_cache_lookup(req->pszHost, &req->hResult);
            Lock(ctx->EntryLock);

            // The current request may have been cleared if the caller lost
            // interest in the result.
            if (ctx->currentRequest != NULL)
            {
                assert(ctx->currentRequest == req);
                ctx->currentRequest = NULL;

                queue_enqueue(ctx->hCompleteList, req);
            }
            else
            {
                dns_cache_free_request(req);
            }

            req = (DNS_REQUEST*)queue_dequeue(ctx->hEntryList);
        }

        if (ctx->flags & DNS_CTX_FLAG_SHUTDOWN)
        {
            break;
        }

        // If we got here, then there's nothing in the request queue.  Wait for
        // new work and loop back.
    } while (COND_OK == Condition_Wait(ctx->hWorkEvent, ctx->EntryLock, 0));
    Unlock(ctx->EntryLock);

    return 0;
}

static void SignalWork(DNS_CONTEXT* ctx)
{
    Lock(ctx->EntryLock);
    Condition_Post(ctx->hWorkEvent);
    Unlock(ctx->EntryLock);
}

DNS_CACHE_HANDLE dns_cache_create()
{
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)malloc(sizeof(DNS_CONTEXT));

    memset(ctx, 0, sizeof(DNS_CONTEXT));

    ctx->hEntryList = list_create();
    ctx->hCompleteList = list_create();
    ctx->EntryLock = Lock_Init();
    ctx->hWorkEvent = Condition_Init();

    if (ctx->hEntryList     &&
        ctx->hCompleteList  &&
        ctx->EntryLock      &&
        ctx->hWorkEvent     &&
        ThreadAPI_Create(&ctx->hThread, DnsWorker, ctx) == THREADAPI_OK)
    {
        return ctx;
    }

    dns_cache_destroy(ctx);
    return NULL;
}

void dns_cache_destroy(DNS_CACHE_HANDLE handle)
{
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)handle;

    ctx->flags |= DNS_CTX_FLAG_SHUTDOWN;

    // wake up the worker thread
    if (NULL != ctx->hWorkEvent)
    {
        SignalWork(ctx);
    }

    if (NULL != ctx->hThread)
    {
        ThreadAPI_Join(ctx->hThread, NULL);
    }

    if (NULL != ctx->hCompleteList)
    {
        list_destroy(ctx->hCompleteList);
    }

    if (NULL != ctx->hEntryList)
    {
        list_destroy(ctx->hEntryList);
    }

    if (NULL != ctx->hWorkEvent)
    {
        Condition_Deinit(ctx->hWorkEvent);
    }

    if (NULL != ctx->EntryLock)
    {
        Lock_Deinit(ctx->EntryLock);
    }

    free(ctx);
}

int dns_cache_getaddr(
    DNS_CACHE_HANDLE handle,
    const char* pszHost,
    DNS_CACHE_NAMERESOLVEDCB pfnResolvedCB,
    void *pContext)
{
    size_t len;
    DNS_REQUEST* req;
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)handle;

    if (!handle || !pszHost || !pfnResolvedCB)
    {
        return -1;
    }

    // create a new request
    len = strlen(pszHost);

    req = (DNS_REQUEST*)malloc(sizeof(DNS_REQUEST) + len + 1);
    if (NULL == req)
    {
        return -1;
    }

    req->pfnResolvedCB = pfnResolvedCB;
    req->pContext = pContext;
    req->pszHost = (char*)&req[1];
    req->hResult = NULL;
    strcpy_s(req->pszHost, len + 1, pszHost);

    // queue it up
    Lock(ctx->EntryLock);
    queue_enqueue(ctx->hEntryList, req);
    Unlock(ctx->EntryLock);

    // tell the worker to get started on it.
    SignalWork(ctx);

    return 0;
}

static bool dns_cache_dequeue_context_match_callback(
    LIST_ITEM_HANDLE item,
    const void* pContext)
{
    const DNS_REQUEST* const request =
        (const DNS_REQUEST*)list_item_get_value(item);
    return request->pContext == pContext;
}

static DNS_REQUEST* dns_cache_dequeue_context_match(LIST_HANDLE list, void* pContext)
{
    DNS_REQUEST* req = NULL;
    const LIST_ITEM_HANDLE item =
        list_find(list, dns_cache_dequeue_context_match_callback, pContext);

    if (item != NULL)
    {
        req = (DNS_REQUEST*)list_item_get_value(item);

        list_remove(list, item);
    }
    return req;
}

/*
 * Polls for any work that's completed for looking up DNS entries.
 * Worker thread will run through each requested DNS host, look up the address, 
 * and then post back to the callers thread.
 */
void dns_cache_dowork(DNS_CACHE_HANDLE handle, void* pContextToMatch)
{
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)handle;
    DNS_REQUEST* req;
    if (NULL != ctx)
    {
        do
        {
            // dequeue a completed work item
            Lock(ctx->EntryLock);
            req = dns_cache_dequeue_context_match(ctx->hCompleteList, pContextToMatch);
            Unlock(ctx->EntryLock);

            if (NULL != req)
            {
                // call the user back with the result (currently not used but
                // we could expose another api to look up the address later)
                if (req->pfnResolvedCB)
                {
                    req->pfnResolvedCB(handle, req->res, req->hResult, req->pContext);
                }

                dns_cache_free_request(req);
            }
        } while (NULL != req);
    }
}

static void remove_all_context_matches_from_list(
    LIST_HANDLE list,
    void* pContext)
{
    LIST_ITEM_HANDLE current = list_get_head_item(list);

    while (current != NULL)
    {
        // Cache the next pointer now because we may remove current from the
        // list.
        const LIST_ITEM_HANDLE next = list_get_next_item(current);

        DNS_REQUEST* const req = (DNS_REQUEST*)list_item_get_value(current);
        if (req->pContext == pContext)
        {
            list_remove(list, current);

            dns_cache_free_request(req);
        }

        current = next;
    }
}

/*
 * Find all DNS requests that have the supplied context and remove them.  This
 * function is intended to clear out callbacks when its context is deleted.
 */
void dns_cache_remove_context_matches(DNS_CACHE_HANDLE handle, void* pContext)
{
    DNS_CONTEXT* ctx = (DNS_CONTEXT*)handle;

    Lock(ctx->EntryLock);

    remove_all_context_matches_from_list(ctx->hEntryList, pContext);
    remove_all_context_matches_from_list(ctx->hCompleteList, pContext);

    if (ctx->currentRequest && (ctx->currentRequest->pContext == pContext))
    {
        // The DnsWorker thread is still using this request -- signal that
        // thread to ignore and free the request.
        ctx->currentRequest = NULL;
    }

    Unlock(ctx->EntryLock);
}

///////////////////////////////////////////////////////////////////////////////

#ifndef DNS_CACHE_H
#define DNS_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DNS_CONTEXT* DNS_CACHE_HANDLE;
typedef void* DNS_RESULT_HANDLE;
typedef void(*DNS_CACHE_NAMERESOLVEDCB)(DNS_CACHE_HANDLE handle, int error, DNS_RESULT_HANDLE hResult, void *pContext);

DNS_CACHE_HANDLE dns_cache_create();

void dns_cache_destroy(DNS_CACHE_HANDLE handle);

int dns_cache_getaddr(
	DNS_CACHE_HANDLE handle,
	const char* pszHost,
	DNS_CACHE_NAMERESOLVEDCB pfnResolvedCB,
	void *pContext);

void dns_cache_dowork(DNS_CACHE_HANDLE handle, void* pContextToMatch);
void dns_cache_remove_context_matches(DNS_CACHE_HANDLE handle, void* pContext);

void dns_cache_result_free(DNS_RESULT_HANDLE handle);
int dns_cache_lookup(
	const char* pszHost,
	DNS_RESULT_HANDLE* result);

#ifdef __cplusplus
} 
#endif

#endif
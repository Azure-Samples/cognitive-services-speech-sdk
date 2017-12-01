// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <stdint.h>
#include "event_queue.h"
#include "token_store.h"
#include "dns_cache.h"

#if defined(__unix__) || defined(TARGET_OS_IPHONE) || defined (TARGET_OS_MAC)
#include <stddef.h> // for size_t
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct cortana_auth;
typedef struct cortana_auth* CortanaAuth;

typedef enum
{
    AUTH_INTERNAL_FAILURE = -1,
    AUTH_SUCCEEDED = 0,
    AUTH_EXPIRED = 1,
    AUTH_TRANSPORT_FAILURE = 2,
    AUTH_INVALID_RESPONSE = 3,
    AUTH_INVALID_REQUEST = 4, //Invalid request for tokens recieved
} AUTH_ERROR_CODE;

/**
* Initializes the authentication module.
* @param event_queue The event queue to push work onto.
* @param token_store The store to save and retrieve tokens from.
* @return The initialized auth handle, NULL if failure occured.
*/
CortanaAuth cortana_authorization_initialize(
    EventQueue event_queue,
    TokenStore token_store
);

/**
* Uninitializes the authentication module.
* @param auth The auth handle to uninitialize
*/
void cortana_authorization_uninitialize(
    CortanaAuth cortana_auth
);

/**
 * Instruct cortana authorization
 * to load existing tokens from the 
 * token store. 
 *             
 * Preforms migration if needed. Should be called
 * when the token store has already been configured
 * with tokens.
 *
 * @param cortana_auth A handle to Cortana's auth
 * @return 0 on success
 */
int cortana_authorization_load_existing(
    CortanaAuth cortana_auth
);

/**
 * Set the DNS cache to use for refreshing
 * @param cortana_auth The authorization handle
 * @param dns_cache The cache to set
 */
void cortana_authorization_use_dns_cache(CortanaAuth cortana_auth, DNS_CACHE_HANDLE dns_cache);

/**
 * Sets an authorization token.
 * @param cortana_auth A handle to Cortana's auth
 * @param scopes The authentication scopes these tokens apply to
 * @param accessToken The access token
 * @param refreshToken The refresh token
 * @return A return code or zero if successful.
 */
int cortana_authorization_settoken(
    CortanaAuth cortana_auth,
    const char* scopes,
    const char* accessToken,
    const char* refreshToken,
    long expireTime
    );

/**
 * Sets the user cid.
 * @param cortana_auth A handle to Cortana's auth
 * @param cid A NULL terminated string.
 * @return A return code or zero if successful.
 */
int cortana_authorization_setcid(CortanaAuth cortana_auth, const char* cid);

/**
 * A function that is called to notify of success or failure of 
 * fetching tokens with the transfer code. 
 * @param result Contains the result of the set
 */
typedef void (*TRANSFER_TOKEN_CALLBACK)(AUTH_ERROR_CODE result, void *pContext);

/**
 * Sets the transfer code that is used to get the real tokens. 
 * Once the transfer code is given, Auth will attempt to fetch
 * the real tokens.
 * @param cortana_auth A handle to Cortana's auth
 * @param code A NULL terminated string.
 * @param pContext User context to give to the TRANSFER_TOKEN_CALLBACK
 * @return A return code or zero if successful.
 */
void cortana_authorization_from_transfer_code(CortanaAuth cortana_auth, const char* code, TRANSFER_TOKEN_CALLBACK callback, void *pContext);

/**
 * Sets the ClientIdIndex that is used to exchange for tokens.
 * @param cortana_auth A handle to Cortana's auth
 * @param index An int that is the index of the pre-defined array of ClientIds
 * @return A return code or zero if successful.
 */
int cortana_authorization_set_clientidindex(CortanaAuth cortana_auth, int index);

/**
 * Receive the token requested.
 * @param result The result of the fetch
 * @param scope The scope requested
 * @param token The token. NULL if failed to get the token.
 * @param ttl The time to live of the token
 * @param pContext The user context passed into the request
 */
typedef void (*CORTANA_TOKEN_CALLBACK)(AUTH_ERROR_CODE result, const char* scope, const char* token, int ttl, void* pContext);

/**
* Gets the authorization oAuth token. The token will be passed into 
* the given callback. If the token needs to be refreshed before
* it can be used, it will be refreshed before the callback is called.
* 
* This function may return before calling the callback. Your pContext
* value remain valid until the callback is called.
* 
* This function is thread safe.
* 
* @param cortana_auth A handle to Cortana's auth
* @param scope get token for specified.
* @param callback The callback to recieve the token.
* @param pContext The user context to pass into the callback
* @return A return code of zero if successful.
*/
int cortana_authorization_gettoken(CortanaAuth cortana_auth, const char* scope, CORTANA_TOKEN_CALLBACK callback, void* pContext);

/**
 * Receive the CID requested.
 * @param result The result of the fetch.
 * @param token The cid. NULL if failed to fetch.
 * @param pContext The user context passed into the request
 */
typedef void (*CORTANA_CID_CALLBACK)(AUTH_ERROR_CODE result, const char* cid, void* pContext);

/**
* Gets the user cid.
* 
* This function is thread safe.
* 
* @param cortana_auth A handle to Cortana's auth
* param callback The callback to the CID. 
* @param pContext The user context to pass into the callback
* @return A return code or zero if successful.
*/
int cortana_authorization_getcid(CortanaAuth cortana_auth, CORTANA_CID_CALLBACK callback, void* pContext);

#ifdef __cplusplus
} // extern "C" 
#endif

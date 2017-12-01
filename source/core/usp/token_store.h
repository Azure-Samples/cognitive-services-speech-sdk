#ifndef TOKEN_STORE_H
#define TOKEN_STORE_H

#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/urlencode.h"
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct TokenStore_;
typedef struct TokenStore_* TokenStore;

/**
 * Create a token store
 * @return A handle to the token store. NULL if failure
 */
TokenStore token_store_create();

/**
 * Destroy the token store
 * @param token_store The token store to destroy
 */
void token_store_destroy(TokenStore token_store);

/**
 * A function that returns the current time in UTC
 */
typedef time_t (*TimeProvider)(void* time_context);

/**
 * Override the systems time provider.
 */
void token_store_set_time_provider(
	TokenStore token_store, 
	TimeProvider time_provider,
	void* time_context
);

/**
 * Add a token to the token store
 * @param  token_store          The token store to add to
 * @param  single_scope         The scope to add to
 * @param  access_token         The access token
 * @param  refresh_token        The refresh token
 * @param  expire_length_sec    The number of seconds before
 *                              the token expires
 * @return                      0 for success.
 */
int token_store_add(TokenStore token_store,
    const char * single_scope,
    const char* access_token,
    const char* refresh_token,
    long expire_length_sec
);

/**
 * Remove a token from the store
 *
 * @param  token_store   The token store
 * @param  single_scope  The scope to remove from the store
 */
void token_store_remove(TokenStore token_store,
	const char * single_scope);

/**
 * Get an access token for the given scope.
 * @param  token_store  The token store
 * @param  scope        The scope to fetch for
 * @param  access_token A string handle to fill with the access token
 * @return              0 on success.
 */
int token_store_get_access_token(TokenStore token_store, const char* scope, STRING_HANDLE access_token);

/**
 * Get an refresh token for the given scope.
 * @param  token_store   The token store
 * @param  scope         The scope to fetch for
 * @param  refresh_token A string handle to fill with the refresh token.
 * @return               0 on success.
 */
int token_store_get_refresh_token(TokenStore token_store, const char* scope, STRING_HANDLE refresh_token);

/**
 * Get the time to live for a token
 * @param  token_store The token store
 * @param  scope       The scope to get the TTL
 * @return             The TTL in seconds. 0 if expired. Negative if error.
 */
int token_store_get_time_to_live(TokenStore token_store, const char* scope);

/**
 * Get the current CID
 * @param  token_store The token store
 * @param  cid         A string handle to fill with the cid
 * @return             0 on success.
 */
int token_store_get_cid(TokenStore token_store, STRING_HANDLE cid);

/**
 * Sets the current CID
 * @param  token_store The token store
 * @param  cid         The cid to set
 * @return             0 on success.
 */
int token_store_set_cid(TokenStore token_store, const char* cid);

#ifdef __cplusplus
}
#endif

#endif
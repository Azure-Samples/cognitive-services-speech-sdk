// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef HTTPAPIEX_SAS_H
#define HTTPAPIEX_SAS_H

#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct HTTPAPIEX_SAS_STATE_TAG* HTTPAPIEX_SAS_HANDLE;

MOCKABLE_FUNCTION(, HTTPAPIEX_SAS_HANDLE, HTTPAPIEX_SAS_Create, STRING_HANDLE, key, STRING_HANDLE, uriResource, STRING_HANDLE, keyName);

MOCKABLE_FUNCTION(, void, HTTPAPIEX_SAS_Destroy, HTTPAPIEX_SAS_HANDLE, handle);

MOCKABLE_FUNCTION(, HTTPAPIEX_RESULT, HTTPAPIEX_SAS_ExecuteRequest, HTTPAPIEX_SAS_HANDLE, sasHandle, HTTPAPIEX_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath, HTTP_HEADERS_HANDLE, requestHttpHeadersHandle, BUFFER_HANDLE, requestContent, unsigned int*, statusCode, HTTP_HEADERS_HANDLE, responseHeadersHandle, BUFFER_HANDLE, responseContent);

#ifdef __cplusplus
}
#endif

#endif /* HTTPAPIEX_SAS_H */

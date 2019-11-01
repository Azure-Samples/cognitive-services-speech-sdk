//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/** @file   httpapi_platform_proxy.h
 *  @brief  This module adds an additional method to the standard HTTP API used by the
 *          Azure C IoT client library to support automatic use of platform proxies
 */

#pragma once

#ifndef _MSC_VER
// Fix Linux build warnings as errors
#pragma GCC system_header
#endif

#include <azure_c_shared_utility_httpapi_wrapper.h>

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

/**
 * @brief   Creates an HTTPS connection to the host specified by the @p
 *          hostName parameter. If a platform proxy was set using the 
 *          platform_set_http_proxy method, that will be used here.
 *          Otherwise no proxy will be used
 *
 * @param   hostName    The name of the host.
 * @param   port        The port on the host to connect to.
 * @param   isSecure    Whether or not this is a secure connection.
 *
 *          This function returns a handle to the newly created connection.
 *          You can use the handle in subsequent calls to execute specific
 *          HTTP calls using ::HTTPAPI_ExecuteRequest.
 * 
 * @return  A @c HTTP_HANDLE to the newly created connection or @c NULL in
 *          case an error occurs.
 */
MOCKABLE_FUNCTION(, HTTP_HANDLE, HTTPAPI_CreateConnection_With_Platform_Proxy, const char*, hostName, int, port, bool, isSecure);

#ifdef __cplusplus
}
#endif

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/httpapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"

static int copy_and_split(const char *input, int splitValue, const char **first, const char **second)
{
    if (input == NULL || first == NULL || second == NULL)
    {
        return 1;
    }

    char *separator = strchr(input, splitValue);
    if (separator == NULL)
    {
        return 1;
    }

    size_t offset = separator - input;
    size_t len = strlen(input);

    // make sure both the first and second strings have at least 1 character
    if (offset < 1 || len - offset - 1 < 1)
    {
        return 1;
    }

    char *copy = NULL;
    if (mallocAndStrcpy_s(&copy, input))
    {
        return 1;
    }
    
    copy[offset] = '\0'; // null terminate the first string
    *first = copy;
    *second = copy + offset + 1;
    return 0;
}

static void destroy_proxy_config(HTTP_PROXY_IO_CONFIG* const proxy_config)
{
    if (proxy_config == NULL)
    {
        return;
    }

    if (proxy_config->proxy_hostname)
    {
        free((void *)proxy_config->proxy_hostname);
        proxy_config->proxy_hostname = NULL;
        proxy_config->proxy_port = 0;
    }

    if (proxy_config->username)
    {
        free((void *)proxy_config->username);
        proxy_config->username = NULL;
        proxy_config->password = NULL;
    }
}

static int populate_proxy_config(HTTP_PROXY_IO_CONFIG* const proxy_config)
{
    if (proxy_config == NULL)
    {
        return 1;
    }

    const char* proxy = NULL;
    const char* username_pass = NULL;

    // we trust that the returned strings are properly null terminated by the library
    // (see connection_string_parser.c)
    platform_get_http_proxy(&proxy, &username_pass);

    if (proxy == NULL)
    {
        return 1;
    }

    proxy_config->hostname = NULL;
    proxy_config->password = NULL;
    proxy_config->port = 0;
    proxy_config->proxy_hostname = NULL;
    proxy_config->proxy_port = 0;
    proxy_config->username = NULL;

    // the proxy is host:port so we need to parse out the host and port
    const char *portString = NULL;
    if (copy_and_split(proxy, ':', &proxy_config->proxy_hostname, &portString))
    {
        return 1;
    }

    proxy_config->proxy_port = (int)strtol(portString, NULL, 10);
    if (errno == ERANGE || proxy_config->proxy_port <= 0)
    {
        destroy_proxy_config(proxy_config);
        return 1;
    }

    copy_and_split(username_pass, ':', &proxy_config->username, &proxy_config->password);
    return 0;
}

HTTP_HANDLE HTTPAPI_CreateConnection_With_Platform_Proxy(const char* hostName, int port, bool isSecure)
{
    HTTP_PROXY_IO_CONFIG proxy_config;
    HTTP_HANDLE handle;
    
    // check if there is a system proxy set and try to use that
    if (populate_proxy_config(&proxy_config) == 0)
    {
        handle = HTTPAPI_CreateConnection_Advanced(
            hostName,
            port,
            isSecure,
            proxy_config.proxy_hostname,
            proxy_config.proxy_port,
            proxy_config.username,
            proxy_config.password);
    }
    else
    {
        handle = HTTPAPI_CreateConnection_Advanced(
            hostName, 443, true, NULL, 0, NULL, NULL);
    }

    destroy_proxy_config(&proxy_config);
    return handle;
}

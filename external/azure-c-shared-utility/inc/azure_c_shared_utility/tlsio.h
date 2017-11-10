// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef TLSIO_H
#define TLSIO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct TLSIO_CONFIG_TAG
{
	const char* hostname;
	int port;
} TLSIO_CONFIG;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TLSIO_H */

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef STORAGE_H
#define STORAGE_H

#include "azure_c_shared_utility/buffer_.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reads a named persistent storage data blob.
 * @param name The name of the data blob entry.
 * @return A handle to a buffer representing the data blob or NULL if not found.
 */
BUFFER_HANDLE cortana_storage_read(const char *name);

/**
 * Writes a named persistant storage data blob.
 * @param name The name of the data blob entry.
 * @param data The data to be written.
 * @param size The size of 'data'
 * @return A return code or zero if successful.
 */
int cortana_storage_write(const char *name, const void* data, size_t size); 

/**
* Remove a named persistant storage data blob.
* @param name The name of the data blob entry.
* @return A return code or zero if successful.
*/
int cortana_storage_remove(const char *name);

#ifdef __cplusplus
}
#endif

#endif
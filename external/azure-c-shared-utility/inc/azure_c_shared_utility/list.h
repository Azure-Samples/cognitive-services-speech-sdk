// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef LIST_H
#define LIST_H

#ifdef __cplusplus
extern "C" {
#include <cstdbool>
#else
#include "stdbool.h"
#endif /* __cplusplus */

#include "azure_c_shared_utility/umock_c_prod.h"

typedef struct LIST_INSTANCE_TAG* LIST_HANDLE;
typedef struct LIST_ITEM_INSTANCE_TAG* LIST_ITEM_HANDLE;
typedef bool (*LIST_MATCH_FUNCTION)(LIST_ITEM_HANDLE list_item, const void* match_context);

MOCKABLE_FUNCTION(, LIST_HANDLE, list_create);
MOCKABLE_FUNCTION(, void, list_destroy, LIST_HANDLE, list);
MOCKABLE_FUNCTION(, LIST_ITEM_HANDLE, list_add, LIST_HANDLE, list, const void*, item);
MOCKABLE_FUNCTION(, int, list_remove, LIST_HANDLE, list, LIST_ITEM_HANDLE, item_handle);
MOCKABLE_FUNCTION(, LIST_ITEM_HANDLE, list_get_head_item, LIST_HANDLE, list);
MOCKABLE_FUNCTION(, LIST_ITEM_HANDLE, list_get_next_item, LIST_ITEM_HANDLE, item_handle);
MOCKABLE_FUNCTION(, LIST_ITEM_HANDLE, list_find, LIST_HANDLE, list, LIST_MATCH_FUNCTION, match_function, const void*, match_context);
MOCKABLE_FUNCTION(, const void*, list_item_get_value, LIST_ITEM_HANDLE, item_handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIST_H */

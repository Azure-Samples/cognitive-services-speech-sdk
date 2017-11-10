// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif

#include "azure_c_shared_utility/umock_c_prod.h"

typedef struct DLIST_ENTRY_TAG
{
    struct DLIST_ENTRY_TAG *Flink;
    struct DLIST_ENTRY_TAG *Blink;
} DLIST_ENTRY, *PDLIST_ENTRY;

MOCKABLE_FUNCTION(, void, DList_InitializeListHead, PDLIST_ENTRY, listHead);
MOCKABLE_FUNCTION(, int, DList_IsListEmpty, const PDLIST_ENTRY, listHead);
MOCKABLE_FUNCTION(, void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
MOCKABLE_FUNCTION(, void, DList_InsertHeadList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
MOCKABLE_FUNCTION(, void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend);
MOCKABLE_FUNCTION(, int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry);
MOCKABLE_FUNCTION(, PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead);

//
// Calculate the address of the base of the structure given its type, and an
// address of a field within the structure.
//
#define containingRecord(address, type, field) ((type *)((uintptr_t)(address) - offsetof(type,field)))

#ifdef __cplusplus
}
#else
#endif

#endif /* DOUBLYLINKEDLIST_H */

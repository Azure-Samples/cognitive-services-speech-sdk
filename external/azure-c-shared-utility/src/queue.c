// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/queue.h"

void *queue_peek(LIST_HANDLE hQueue)
{
    LIST_ITEM_HANDLE hItem;
    void * pItem = NULL;

    hItem = list_get_head_item(hQueue);
    if (hItem != NULL)
    {
        pItem = (void*)list_item_get_value(hItem);
    }

    return pItem;
}

void *queue_dequeue(LIST_HANDLE hQueue)
{
    LIST_ITEM_HANDLE hItem;
    void * pItem = NULL;

    hItem = list_get_head_item(hQueue);
    if (hItem != NULL)
    {
        pItem = (void*)list_item_get_value(hItem);
        (void)list_remove(hQueue, hItem);
    }

    return pItem;
}

int queue_enqueue(LIST_HANDLE hQueue, void *pData)
{
    return (NULL == list_add(hQueue, pData)) ? -1 : 0;
}

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdbool.h>

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/list.h"

typedef struct LIST_ITEM_INSTANCE_TAG
{
    const void* item;
    void* next;
} LIST_ITEM_INSTANCE;

typedef struct LIST_INSTANCE_TAG
{
    LIST_ITEM_INSTANCE* head;
} LIST_INSTANCE;

LIST_HANDLE list_create(void)
{
    LIST_INSTANCE* result;

    /* Codes_SRS_LIST_01_001: [list_create shall create a new list and return a non-NULL handle on success.] */
    result = (LIST_INSTANCE*)malloc(sizeof(LIST_INSTANCE));
    if (result != NULL)
    {
        /* Codes_SRS_LIST_01_002: [If any error occurs during the list creation, list_create shall return NULL.] */
        result->head = NULL;
    }

    return result;
}

void list_destroy(LIST_HANDLE list)
{
    /* Codes_SRS_LIST_01_004: [If the list argument is NULL, no freeing of resources shall occur.] */
    if (list != NULL)
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;

        while (list_instance->head != NULL)
        {
            LIST_ITEM_INSTANCE* current_item = list_instance->head;
            list_instance->head = current_item->next;
            free(current_item);
        }

        /* Codes_SRS_LIST_01_003: [list_destroy shall free all resources associated with the list identified by the handle argument.] */
        free(list_instance);
    }
}

LIST_ITEM_HANDLE list_add(LIST_HANDLE list, const void* item)
{
    LIST_ITEM_INSTANCE* result;

    /* Codes_SRS_LIST_01_006: [If any of the arguments is NULL, list_add shall not add the item to the list and return NULL.] */
    if ((list == NULL) ||
        (item == NULL))
    {
        result = NULL;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        result = (LIST_ITEM_INSTANCE*)malloc(sizeof(LIST_ITEM_INSTANCE));

        if (result == NULL)
        {
            /* Codes_SRS_LIST_01_007: [If allocating the new list node fails, list_add shall return NULL.] */
            result = NULL;
        }
        else
        {
            /* Codes_SRS_LIST_01_005: [list_add shall add one item to the tail of the list and on success it shall return a handle to the added item.] */
            result->next = NULL;
            result->item = item;

            if (list_instance->head == NULL)
            {
                list_instance->head = result;
            }
            else
            {
                LIST_ITEM_INSTANCE* current = list_instance->head;
                while (current->next != NULL)
                {
                    current = current->next;
                }

                current->next = result;
            }
        }
    }

    return result;
}

int list_remove(LIST_HANDLE list, LIST_ITEM_HANDLE item)
{
    int result;

	/* Codes_SRS_LIST_01_024: [If any of the arguments list or item_handle is NULL, list_remove shall fail and return a non-zero value.] */
	if ((list == NULL) ||
        (item == NULL))
    {
        result = __LINE__;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        LIST_ITEM_INSTANCE* current_item = list_instance->head;
        LIST_ITEM_INSTANCE* previous_item = NULL;

        while (current_item != NULL)
        {
            if (current_item == item)
            {
                if (previous_item != NULL)
                {
                    previous_item->next = current_item->next;
                }
                else
                {
                    list_instance->head = current_item->next;
                }

                free(current_item);

                break;
            }
            previous_item = current_item;
			current_item = current_item->next;
        }

		if (current_item == NULL)
		{
			/* Codes_SRS_LIST_01_025: [If the item item_handle is not found in the list, then list_remove shall fail and return a non-zero value.] */
			result = __LINE__;
		}
		else
		{
			/* Codes_SRS_LIST_01_023: [list_remove shall remove a list item from the list and on success it shall return 0.] */
			result = 0;
		}
    }

    return result;
}

LIST_ITEM_HANDLE list_get_head_item(LIST_HANDLE list)
{
    LIST_ITEM_HANDLE result;
    
    if (list == NULL)
    {
        /* Codes_SRS_LIST_01_009: [If the list argument is NULL, list_get_head_item shall return NULL.] */
        result = NULL;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;

        /* Codes_SRS_LIST_01_008: [list_get_head_item shall return the head of the list.] */
        /* Codes_SRS_LIST_01_010: [If the list is empty, list_get_head_item_shall_return NULL.] */
        result = list_instance->head;
    }

    return result;
}

LIST_ITEM_HANDLE list_get_next_item(LIST_ITEM_HANDLE item_handle)
{
    LIST_ITEM_HANDLE result;

    if (item_handle == NULL)
    {
        /* Codes_SRS_LIST_01_019: [If item_handle is NULL then list_get_next_item shall return NULL.] */
        result = NULL;
    }
    else
    {
        /* Codes_SRS_LIST_01_018: [list_get_next_item shall return the next item in the list following the item item_handle.] */
        result = ((LIST_ITEM_INSTANCE*)item_handle)->next;
    }

    return result;
}

const void* list_item_get_value(LIST_ITEM_HANDLE item_handle)
{
    const void* result;

    if (item_handle == NULL)
    {
        /* Codes_SRS_LIST_01_021: [If item_handle is NULL, list_item_get_value shall return NULL.] */
        result = NULL;
    }
    else
    {
        /* Codes_SRS_LIST_01_020: [list_item_get_value shall return the value associated with the list item identified by the item_handle argument.] */
        result = ((LIST_ITEM_INSTANCE*)item_handle)->item;
    }

    return result;
}

LIST_ITEM_HANDLE list_find(LIST_HANDLE list, LIST_MATCH_FUNCTION match_function, const void* match_context)
{
    LIST_ITEM_HANDLE result;

    if ((list == NULL) ||
        (match_function == NULL))
    {
        /* Codes_SRS_LIST_01_012: [If the list or the match_function argument is NULL, list_find shall return NULL.] */
        result = NULL;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        LIST_ITEM_INSTANCE* current = list_instance->head;

        /* Codes_SRS_LIST_01_011: [list_find shall iterate through all items in a list and return the first one that satisfies a certain match function.] */
        while (current != NULL)
        {
            /* Codes_SRS_LIST_01_014: [list find shall determine whether an item satisfies the match criteria by invoking the match function for each item in the list until a matching item is found.] */
            /* Codes_SRS_LIST_01_013: [The match_function shall get as arguments the list item being attempted to be matched and the match_context as is.] */
            if (match_function((LIST_ITEM_HANDLE)current, match_context) == true)
            {
                /* Codes_SRS_LIST_01_017: [If the match function returns true, list_find shall consider that item as matching.] */
                break;
            }

            /* Codes_SRS_LIST_01_016: [If the match function returns false, list_find shall consider that item as not matching.] */
            current = current->next;
        }

        if (current == NULL)
        {
            /* Codes_SRS_LIST_01_015: [If the list is empty, list_find shall return NULL.] */
            result = NULL;
        }
        else
        {
            result = current;
        }
    }

    return result;
}

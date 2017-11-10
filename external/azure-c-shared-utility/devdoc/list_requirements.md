list requirements
================
 
##Overview

List is module that provides the functionality of a singly linked list, allowing its user to add, remove and iterate the list elements.

##Exposed API

```c
typedef struct LIST_INSTANCE_TAG* LIST_HANDLE;
typedef struct LIST_ITEM_INSTANCE_TAG* LIST_ITEM_HANDLE;
typedef bool (*LIST_MATCH_FUNCTION)(LIST_ITEM_HANDLE list_item, const void* match_context);

extern LIST_HANDLE list_create(void);
extern void list_destroy(LIST_HANDLE list);
extern LIST_ITEM_HANDLE list_add(LIST_HANDLE list, const void* item);
extern int list_remove(LIST_HANDLE list, LIST_ITEM_HANDLE item_handle);
extern LIST_ITEM_HANDLE list_get_head_item(LIST_HANDLE list);
extern LIST_ITEM_HANDLE list_get_next_item(LIST_ITEM_HANDLE item_handle);
extern LIST_ITEM_HANDLE list_find(LIST_HANDLE list, LIST_MATCH_FUNCTION match_function, const void* match_context);
extern int list_remove_matching_item(LIST_HANDLE list, LIST_MATCH_FUNCTION match_function, const void* match_context);

extern const void* list_item_get_value(LIST_ITEM_HANDLE item_handle);
```

###list_create
```c
extern LIST_HANDLE list_create(void);
```

**SRS_LIST_01_001: [**list_create shall create a new list and return a non-NULL handle on success.**]**
**SRS_LIST_01_002: [**If any error occurs during the list creation, list_create shall return NULL.**]**
 
###list_destroy
```c
extern void list_destroy(LIST_HANDLE list);
```
**SRS_LIST_01_003: [**list_destroy shall free all resources associated with the list identified by the handle argument.**]**
**SRS_LIST_01_004: [**If the list argument is NULL, no freeing of resources shall occur.**]**
 
###list_add
```c
extern int list_add(LIST_HANDLE list, const void* item);
```

**SRS_LIST_01_005: [**list_add shall add one item to the tail of the list and on success it shall return a handle to the added item.**]**
**SRS_LIST_01_006: [**If any of the arguments is NULL, list_add shall not add the item to the list and return NULL.**]**
**SRS_LIST_01_007: [**If allocating the new list node fails, list_add shall return NULL.**]**
 
###list_get_head_item
```c
extern const void* list_get_head_item(LIST_HANDLE list);
```
**SRS_LIST_01_008: [**list_get_head_item shall return the head of the list.**]**
**SRS_LIST_01_009: [**If the list argument is NULL, list_get_head_item shall return NULL.**]**
**SRS_LIST_01_010: [**If the list is empty, list_get_head_item_shall_return NULL.**]**
 
###list_get_next_item
```c
extern LIST_ITEM_HANDLE list_get_next_item(LIST_ITEM_HANDLE item_handle);
```

**SRS_LIST_01_018: [**list_get_next_item shall return the next item in the list following the item item_handle.**]**
**SRS_LIST_01_019: [**If item_handle is NULL then list_get_next_item shall return NULL.**]**
**SRS_LIST_01_022: [**If no more items exist in the list after the item_handle item, list_get_next_item shall return NULL.**]** 

###list_find
```c
extern const void* list_find(LIST_HANDLE list, const void* match_context, LIST_MATCH_FUNCTION match_function);
```

**SRS_LIST_01_011: [**list_find shall iterate through all items in a list and return the first one that satisfies a certain match function.**]**
**SRS_LIST_01_012: [**If the list or the match_function argument is NULL, list_find shall return NULL.**]**
**SRS_LIST_01_014: [**list find shall determine whether an item satisfies the match criteria by invoking the match function for each item in the list until a matching item is found.**]**
**SRS_LIST_01_013: [**The match_function shall get as arguments the list item being attempted to be matched and the match_context as is.**]**
**SRS_LIST_01_016: [**If the match function returns false, list_find shall consider that item as not matching.**]**
**SRS_LIST_01_017: [**If the match function returns true, list_find shall consider that item as matching.**]**
**SRS_LIST_01_015: [**If the list is empty, list_find shall return NULL.**]**
 
###list_remove
```c
extern int list_remove(LIST_HANDLE list, LIST_ITEM_HANDLE item_handle);
```

**SRS_LIST_01_023: [**list_remove shall remove a list item from the list and on success it shall return 0.**]**
**SRS_LIST_01_024: [**If any of the arguments list or item_handle is NULL, list_remove shall fail and return a non-zero value.**]**
**SRS_LIST_01_025: [**If the item item_handle is not found in the list, then list_remove shall fail and return a non-zero value.**]**
 
###list_item_get_value
```c
extern const void* list_item_get_value(LIST_ITEM_HANDLE item_handle);
```

**SRS_LIST_01_020: [**list_item_get_value shall return the value associated with the list item identified by the item_handle argument.**]**
**SRS_LIST_01_021: [**If item_handle is NULL, list_item_get_value shall return NULL.**]**

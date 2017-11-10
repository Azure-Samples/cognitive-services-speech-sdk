// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdbool.h>
#include <stdlib.h>
#include "testrunnerswitcher.h"

static size_t currentmalloc_call = 0;
static size_t whenShallmalloc_fail = 0;

static size_t currentrealloc_call = 0;
static size_t whenShallrealloc_fail = 0;

void* my_gballoc_malloc(size_t size)
{
    void* result;
    currentmalloc_call++;
    if (whenShallmalloc_fail > 0)
    {
        if (currentmalloc_call == whenShallmalloc_fail)
        {
            result = NULL;
        }
        else
        {
            result = malloc(size);
        }
    }
    else
    {
        result = malloc(size);
    }
    return result;
}

void my_gballoc_free(void* ptr)
{
    free(ptr);
}

#include "umock_c.h"
#include "umocktypes_bool.h"
#include "azure_c_shared_utility/list.h"

#define ENABLE_MOCKS

/* test match function mock */
MOCK_FUNCTION_WITH_CODE(, bool, test_match_function, LIST_ITEM_HANDLE, list_item, const void*, match_context)
MOCK_FUNCTION_END(true);

#include "azure_c_shared_utility/gballoc.h"

#undef ENABLE_MOCKS

static TEST_MUTEX_HANDLE test_serialize_mutex;
static TEST_MUTEX_HANDLE g_dllByDll;

#define TEST_CONTEXT ((const void*)0x4242)

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(list_unittests)

TEST_SUITE_INITIALIZE(suite_init)
{
    int result;

    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

    test_serialize_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_serialize_mutex);

    umock_c_init(on_umock_c_error);

    result = umocktypes_bool_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_UMOCK_ALIAS_TYPE(LIST_ITEM_HANDLE, void*);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(test_serialize_mutex);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(method_init)
{
    if (TEST_MUTEX_ACQUIRE(test_serialize_mutex) != 0)
    {
        ASSERT_FAIL("Could not acquire test serialization mutex.");
    }

    umock_c_reset_all_calls();
}

TEST_FUNCTION_CLEANUP(method_cleanup)
{
    TEST_MUTEX_RELEASE(test_serialize_mutex);
}

/* list_create */

/* Tests_SRS_LIST_01_001: [list_create shall create a new list and return a non-NULL handle on success.] */
TEST_FUNCTION(when_underlying_calls_succeed_list_create_succeeds)
{
    // arrange
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));

    // act
    LIST_HANDLE result = list_create();

    // assert
    ASSERT_IS_NOT_NULL(result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(result);
}

/* Tests_SRS_LIST_01_002: [If any error occurs during the list creation, list_create shall return NULL.] */
TEST_FUNCTION(when_underlying_malloc_fails_list_create_fails)
{
    // arrange
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .SetReturn((void*)NULL);

    // act
    LIST_HANDLE result = list_create();

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* list_destroy */

/* Tests_SRS_LIST_01_003: [list_destroy shall free all resources associated with the list identified by the handle argument.] */
TEST_FUNCTION(list_destroy_on_a_non_null_handle_frees_resources)
{
    // arrange
    LIST_HANDLE handle = list_create();
    umock_c_reset_all_calls();

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    // act
    list_destroy(handle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_LIST_01_004: [If the list argument is NULL, no freeing of resources shall occur.] */
TEST_FUNCTION(list_destroy_on_a_null_list_frees_nothing)
{
    // arrange

    // act
    list_destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* list_add */

/* Tests_SRS_LIST_01_006: [If any of the arguments is NULL, list_add shall not add the item to the list and return NULL.] */
TEST_FUNCTION(list_add_with_NULL_handle_fails)
{
    // arrange
    int x = 42;

    // act
    LIST_ITEM_HANDLE result = list_add(NULL, &x);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_LIST_01_006: [If any of the arguments is NULL, list_add shall not add the item to the list and return NULL.] */
TEST_FUNCTION(list_add_with_NULL_item_fails)
{
    // arrange
    LIST_HANDLE list = list_create();
    umock_c_reset_all_calls();

    // act
	LIST_ITEM_HANDLE result = list_add(list, NULL);

    // assert
    ASSERT_IS_NULL(result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_005: [list_add shall add one item to the tail of the list and on success it shall return a handle to the added item.] */
/* Tests_SRS_LIST_01_008: [list_get_head_item shall return the head of the list.] */
TEST_FUNCTION(list_add_adds_the_item_and_returns_a_non_NULL_handle)
{
    // arrange
    LIST_HANDLE list = list_create();
    umock_c_reset_all_calls();
    int x = 42;

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));

    // act
    LIST_ITEM_HANDLE result = list_add(list, &x);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    LIST_ITEM_HANDLE head = list_get_head_item(list);
    ASSERT_IS_NOT_NULL(head);
    ASSERT_ARE_EQUAL(int, x, *(const int*)list_item_get_value(head));
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_005: [list_add shall add one item to the tail of the list and on success it shall return a handle to the added item.] */
/* Tests_SRS_LIST_01_008: [list_get_head_item shall return the head of the list.] */
TEST_FUNCTION(list_add_when_an_item_is_in_the_list_adds_at_the_end)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x1 = 42;
    int x2 = 43;

    (void)list_add(list, &x1);
    umock_c_reset_all_calls();

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));

    // act
    LIST_ITEM_HANDLE result = list_add(list, &x2);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    LIST_ITEM_HANDLE list_item = list_get_head_item(list);
    ASSERT_IS_NOT_NULL(list_item);
    ASSERT_ARE_EQUAL(int, x1, *(const int*)list_item_get_value(list_item));
    list_item = list_get_next_item(list_item);
    ASSERT_IS_NOT_NULL(list_item);
    ASSERT_ARE_EQUAL(int, x2, *(const int*)list_item_get_value(list_item));
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_007: [If allocating the new list node fails, list_add shall return NULL.] */
TEST_FUNCTION(when_the_underlying_malloc_fails_list_add_fails)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x = 42;
    umock_c_reset_all_calls();

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .SetReturn((void*)NULL);

    // act
    LIST_ITEM_HANDLE result = list_add(list, &x);

    // assert
    ASSERT_IS_NULL(result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* list_get_head_item */

/* Tests_SRS_LIST_01_010: [If the list is empty, list_get_head_item_shall_return NULL.] */
TEST_FUNCTION(when_the_list_is_empty_list_get_head_item_yields_NULL)
{
    // arrange
    LIST_HANDLE list = list_create();
    umock_c_reset_all_calls();

    // act
    LIST_ITEM_HANDLE result = list_get_head_item(list);

    // assert
    ASSERT_IS_NULL(result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_009: [If the list argument is NULL, list_get_head_item shall return NULL.] */
TEST_FUNCTION(list_get_head_item_with_NULL_list_yields_NULL)
{
    // arrange

    // act
    LIST_ITEM_HANDLE result = list_get_head_item(NULL);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_LIST_01_008: [list_get_head_item shall return the head of the list.] */
TEST_FUNCTION(list_get_head_item_removes_the_item)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x = 42;
    (void)list_add(list, &x);
    umock_c_reset_all_calls();

    // act
    LIST_ITEM_HANDLE head = list_get_head_item(list);

    // assert
    ASSERT_IS_NOT_NULL(head);
    ASSERT_ARE_EQUAL(int, x, *(const int*)list_item_get_value(head));
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* list_get_next_item */

/* Tests_SRS_LIST_01_018: [list_get_next_item shall return the next item in the list following the item item_handle.] */
TEST_FUNCTION(list_get_next_item_gets_the_next_item)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x1 = 42;
    int x2 = 43;
    (void)list_add(list, &x1);
    (void)list_add(list, &x2);
    umock_c_reset_all_calls();
    LIST_ITEM_HANDLE item = list_get_head_item(list);

    // act
    item = list_get_next_item(item);

    // assert
    ASSERT_IS_NOT_NULL(item);
    ASSERT_ARE_EQUAL(int, x2, *(const int*)list_item_get_value(item));
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_019: [If item_handle is NULL then list_get_next_item shall return NULL.] */
TEST_FUNCTION(list_get_next_item_with_NULL_item_handle_returns_NULL)
{
    // arrange

    // act
    LIST_ITEM_HANDLE item = list_get_next_item(NULL);

    // assert
    ASSERT_IS_NULL(item);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_LIST_01_022: [If no more items exist in the list after the item_handle item, list_get_next_item shall return NULL.] */
TEST_FUNCTION(list_get_next_item_when_no_more_items_in_list_returns_NULL)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x1 = 42;
    int x2 = 43;
    (void)list_add(list, &x1);
    (void)list_add(list, &x2);
    umock_c_reset_all_calls();
    LIST_ITEM_HANDLE item = list_get_head_item(list);
    item = list_get_next_item(item);

    // act
    item = list_get_next_item(item);

    // assert
    ASSERT_IS_NULL(item);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* list_item_get_value */

/* Tests_SRS_LIST_01_020: [list_item_get_value shall return the value associated with the list item identified by the item_handle argument.] */
TEST_FUNCTION(list_item_get_value_returns_the_item_value)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x = 42;
    (void)list_add(list, &x);
    umock_c_reset_all_calls();
    LIST_ITEM_HANDLE item = list_get_head_item(list);

    // act
    int result = *(const int*)list_item_get_value(item);

    // assert
    ASSERT_ARE_EQUAL(int, x, result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_021: [If item_handle is NULL, list_item_get_value shall return NULL.] */
TEST_FUNCTION(list_item_get_value_with_NULL_item_returns_NULL)
{
    // arrange

    // act
    const void* result = list_item_get_value(NULL);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* list_find */

/* Tests_SRS_LIST_01_012: [If the list or the match_function argument is NULL, list_find shall return NULL.] */
TEST_FUNCTION(list_find_with_NULL_list_fails_with_NULL)
{
    // arrange

    // act
    LIST_ITEM_HANDLE result = list_find(NULL, test_match_function, TEST_CONTEXT);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_LIST_01_012: [If the list or the match_function argument is NULL, list_find shall return NULL.] */
TEST_FUNCTION(list_find_with_NULL_match_function_fails_with_NULL)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x = 42;
    (void)list_add(list, &x);
    umock_c_reset_all_calls();

    // act
    LIST_ITEM_HANDLE result = list_find(list, NULL, TEST_CONTEXT);

    // assert
    ASSERT_IS_NULL(result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_011: [list_find shall iterate through all items in a list and return the first one that satisfies a certain match function.] */
/* Tests_SRS_LIST_01_014: [list find shall determine whether an item satisfies the match criteria by invoking the match function for each item in the list until a matching item is found.] */
/* Tests_SRS_LIST_01_013: [The match_function shall get as arguments the list item being attempted to be matched and the match_context as is.] */
/* Tests_SRS_LIST_01_017: [If the match function returns true, list_find shall consider that item as matching.] */
TEST_FUNCTION(list_find_on_a_list_with_1_matching_item_yields_that_item)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x = 42;
    (void)list_add(list, &x);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(test_match_function(IGNORED_PTR_ARG, TEST_CONTEXT))
        .IgnoreArgument(1);

    // act
    LIST_ITEM_HANDLE result = list_find(list, test_match_function, TEST_CONTEXT);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(int, x, *(const int*)list_item_get_value(result));
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_016: [If the match function returns false, list_find shall consider that item as not matching.] */
TEST_FUNCTION(list_find_on_a_list_with_1_items_that_does_not_match_returns_NULL)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x = 42;
    (void)list_add(list, &x);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(test_match_function(IGNORED_PTR_ARG, TEST_CONTEXT))
        .IgnoreArgument(1).SetReturn(false);

    // act
    LIST_ITEM_HANDLE result = list_find(list, test_match_function, TEST_CONTEXT);

    // assert
    ASSERT_IS_NULL(result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_011: [list_find shall iterate through all items in a list and return the first one that satisfies a certain match function.] */
/* Tests_SRS_LIST_01_014: [list find shall determine whether an item satisfies the match criteria by invoking the match function for each item in the list until a matching item is found.] */
/* Tests_SRS_LIST_01_013: [The match_function shall get as arguments the list item being attempted to be matched and the match_context as is.] */
/* Tests_SRS_LIST_01_017: [If the match function returns true, list_find shall consider that item as matching.] */
TEST_FUNCTION(list_find_on_a_list_with_2_items_where_the_first_matches_yields_the_first_item)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x1 = 42;
    int x2 = 43;
    (void)list_add(list, &x1);
    (void)list_add(list, &x2);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(test_match_function(IGNORED_PTR_ARG, TEST_CONTEXT))
        .IgnoreArgument(1);

    // act
    LIST_ITEM_HANDLE result = list_find(list, test_match_function, TEST_CONTEXT);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(int, x1, *(int*)list_item_get_value(result));
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_011: [list_find shall iterate through all items in a list and return the first one that satisfies a certain match function.] */
/* Tests_SRS_LIST_01_014: [list find shall determine whether an item satisfies the match criteria by invoking the match function for each item in the list until a matching item is found.] */
/* Tests_SRS_LIST_01_013: [The match_function shall get as arguments the list item being attempted to be matched and the match_context as is.] */
/* Tests_SRS_LIST_01_017: [If the match function returns true, list_find shall consider that item as matching.] */
/* Tests_SRS_LIST_01_016: [If the match function returns false, list_find shall consider that item as not matching.] */
TEST_FUNCTION(list_find_on_a_list_with_2_items_where_the_second_matches_yields_the_second_item)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x1 = 42;
    int x2 = 43;
    (void)list_add(list, &x1);
    (void)list_add(list, &x2);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(test_match_function(IGNORED_PTR_ARG, TEST_CONTEXT))
        .IgnoreArgument(1).SetReturn(false);
    STRICT_EXPECTED_CALL(test_match_function(IGNORED_PTR_ARG, TEST_CONTEXT))
        .IgnoreArgument(1);

    // act
    LIST_ITEM_HANDLE result = list_find(list, test_match_function, TEST_CONTEXT);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(int, x2, *(int*)list_item_get_value(result));
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_011: [list_find shall iterate through all items in a list and return the first one that satisfies a certain match function.] */
TEST_FUNCTION(list_find_on_a_list_with_2_items_both_matching_yields_the_first_item)
{
	// arrange
	LIST_HANDLE list = list_create();
	int x1 = 42;
	int x2 = 42;
	(void)list_add(list, &x1);
	(void)list_add(list, &x2);
	umock_c_reset_all_calls();

	STRICT_EXPECTED_CALL(test_match_function(IGNORED_PTR_ARG, TEST_CONTEXT))
		.IgnoreArgument(1);

	// act
	LIST_ITEM_HANDLE result = list_find(list, test_match_function, TEST_CONTEXT);

	// assert
	ASSERT_IS_NOT_NULL(result);
	ASSERT_ARE_EQUAL(int, x1, *(int*)list_item_get_value(result));
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_016: [If the match function returns false, list_find shall consider that item as not matching.] */
TEST_FUNCTION(list_find_on_a_list_with_2_items_where_none_matches_returns_NULL)
{
    // arrange
    LIST_HANDLE list = list_create();
    int x1 = 42;
    int x2 = 43;
    (void)list_add(list, &x1);
    (void)list_add(list, &x2);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(test_match_function(IGNORED_PTR_ARG, TEST_CONTEXT))
        .IgnoreArgument(1).SetReturn(false);
    STRICT_EXPECTED_CALL(test_match_function(IGNORED_PTR_ARG, TEST_CONTEXT))
        .IgnoreArgument(1).SetReturn(false);

    // act
    LIST_ITEM_HANDLE result = list_find(list, test_match_function, TEST_CONTEXT);

    // assert
    ASSERT_IS_NULL(result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_015: [If the list is empty, list_find shall return NULL.] */
TEST_FUNCTION(list_find_on_a_list_with_no_items_yields_NULL)
{
    // arrange
	LIST_HANDLE list = list_create();
    umock_c_reset_all_calls();

    // act
	LIST_ITEM_HANDLE result = list_find(list, test_match_function, TEST_CONTEXT);

    // assert
    ASSERT_IS_NULL(result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* list_remove */

/* Tests_SRS_LIST_01_023: [list_remove shall remove a list item from the list and on success it shall return 0.] */
TEST_FUNCTION(list_remove_when_one_item_is_in_the_list_succeeds)
{
	// arrange
	int x1 = 0x42;
	LIST_HANDLE list = list_create();
	list_add(list, &x1);
	LIST_ITEM_HANDLE item = list_find(list, test_match_function, TEST_CONTEXT);
	umock_c_reset_all_calls();

	EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

	// act
	int result = list_remove(list, item);

	// assert
	ASSERT_ARE_EQUAL(int, 0, result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_024: [If any of the arguments list or item_handle is NULL, list_remove shall fail and return a non-zero value.] */
TEST_FUNCTION(list_remove_with_NULL_list_fails)
{
	// arrange
	int x1 = 0x42;
	LIST_HANDLE list = list_create();
	LIST_ITEM_HANDLE item = list_add(list, &x1);
	umock_c_reset_all_calls();

	// act
	int result = list_remove(NULL, item);

	// assert
	ASSERT_ARE_NOT_EQUAL(int, 0, result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_024: [If any of the arguments list or item_handle is NULL, list_remove shall fail and return a non-zero value.] */
TEST_FUNCTION(list_remove_with_NULL_item_fails)
{
	// arrange
	int x1 = 0x42;
	LIST_HANDLE list = list_create();
	LIST_ITEM_HANDLE item = list_add(list, &x1);
	umock_c_reset_all_calls();

	// act
	int result = list_remove(list, NULL);

	// assert
	ASSERT_ARE_NOT_EQUAL(int, 0, result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_025: [If the item item_handle is not found in the list, then list_remove shall fail and return a non-zero value.] */
TEST_FUNCTION(list_remove_with_an_item_that_has_already_been_removed_fails)
{
	// arrange
	int x1 = 0x42;
	LIST_HANDLE list = list_create();
	LIST_ITEM_HANDLE item = list_add(list, &x1);
	list_remove(list, item);
	umock_c_reset_all_calls();

	// act
	int result = list_remove(list, item);

	// assert
	ASSERT_ARE_NOT_EQUAL(int, 0, result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_023: [list_remove shall remove a list item from the list and on success it shall return 0.] */
TEST_FUNCTION(list_remove_first_of_2_items_succeeds)
{
	// arrange
	int x1 = 0x42;
	int x2 = 0x43;
	LIST_HANDLE list = list_create();
	LIST_ITEM_HANDLE item1 = list_add(list, &x1);
	LIST_ITEM_HANDLE item2 = list_add(list, &x2);
	umock_c_reset_all_calls();

	EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

	// act
	int result = list_remove(list, item1);

	// assert
	ASSERT_ARE_EQUAL(int, 0, result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

/* Tests_SRS_LIST_01_023: [list_remove shall remove a list item from the list and on success it shall return 0.] */
TEST_FUNCTION(list_remove_second_of_2_items_succeeds)
{
	// arrange
	int x1 = 0x42;
	int x2 = 0x43;
	LIST_HANDLE list = list_create();
	LIST_ITEM_HANDLE item1 = list_add(list, &x1);
	LIST_ITEM_HANDLE item2 = list_add(list, &x2);
	umock_c_reset_all_calls();

	EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

	// act
	int result = list_remove(list, item2);

	// assert
	ASSERT_ARE_EQUAL(int, 0, result);
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	list_destroy(list);
}

END_TEST_SUITE(list_unittests)

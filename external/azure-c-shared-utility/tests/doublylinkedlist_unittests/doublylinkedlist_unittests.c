// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/doublylinkedlist.h"
#include "testrunnerswitcher.h"

typedef struct simpleItem_tag
{
    unsigned char index;
    DLIST_ENTRY link;
} simpleItem,*pSimpleItem;

static simpleItem simp1 = { 1 };
static simpleItem simp2 = { 2 };
static simpleItem simp3 = { 3 };
static simpleItem simp4 = { 4 };
static simpleItem simp5 = { 5 };

static TEST_MUTEX_HANDLE g_dllByDll;
static TEST_MUTEX_HANDLE g_testByTest;

BEGIN_TEST_SUITE(doublylinkedlist_unittests)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    TEST_MUTEX_DESTROY(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest) != 0)
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

    /* Tests_SRS_DLIST_06_005: [DList_InitializeListHead will initialize the Flink & Blink to the address of the DLIST_ENTRY.] */
    TEST_FUNCTION(DList_InitializeListHead_with_a_list_head_points_Flink_and_Blink_to_its_address)
    {
        // arrange
        DLIST_ENTRY head = { 0 };

        // act
        DList_InitializeListHead(&head);

        // assert
        ASSERT_ARE_EQUAL(void_ptr, &head, head.Flink);
        ASSERT_ARE_EQUAL(void_ptr, &head, head.Blink);
    }

    /* Tests_SRS_DLIST_06_003: [DList_IsListEmpty shall return a non-zero value if there are no DLIST_ENTRY's on this list other than the list head.] */
    TEST_FUNCTION(DList_IsListEmpty_with_a_list_head_only_returns_empty)
    {
        // arrange
        DLIST_ENTRY local = { 0 };
        int result;

        DList_InitializeListHead(&local);
        // act
        result = DList_IsListEmpty(&local);

        // assert
        ASSERT_ARE_NOT_EQUAL(int, 0, result);
    }

    /* Tests_SRS_DLIST_06_004: [DList_IsListEmpty shall return 0 if there is one or more items in the list.]*/
    TEST_FUNCTION(DList_IsListEmpty_with_a_list_head_and_items_returns_empty)
    {
        // arrange
        DLIST_ENTRY head = { 0 };
        int result;

        DList_InitializeListHead(&head);
        DList_InsertTailList(&head, &(simp1.link));
        // act
        result = DList_IsListEmpty(&head);

        // assert
        ASSERT_ARE_EQUAL(int, 0, result);
    }

    /* Tests_SRS_DLIST_06_006: [DListInsertTailList shall place the DLIST_ENTRY at the end of the list defined by the listHead parameter.] */
    TEST_FUNCTION(DList_InsertTailList_with_a_list_head_inserts_1st_item_at_the_end_of_the_list)
    {
        // arrange
        DLIST_ENTRY head = { 0 };

        DList_InitializeListHead(&head);
        // act
        DList_InsertTailList(&head, &(simp1.link));

        // assert
        ASSERT_ARE_EQUAL(void_ptr, head.Flink, &(simp1.link));
        ASSERT_ARE_EQUAL(void_ptr, head.Blink, &(simp1.link));
    }

    /* Tests_SRS_DLIST_06_006: [DListInsertTailList shall place the DLIST_ENTRY at the end of the list defined by the listHead parameter.] */
    TEST_FUNCTION(DList_InsertTailList_with_a_list_head_inserts_2nd_item_at_the_end_of_the_list)
    {
        // arrange
        DLIST_ENTRY head = { 0 };

        DList_InitializeListHead(&head);
        DList_InsertTailList(&head, &(simp1.link));
        // act
        DList_InsertTailList(&head, &(simp2.link));

        // assert
        ASSERT_ARE_EQUAL(void_ptr, head.Flink, &(simp1.link));
        ASSERT_ARE_EQUAL(void_ptr, simp1.link.Flink, &(simp2.link));
        ASSERT_ARE_EQUAL(void_ptr, simp2.link.Flink, &head);
        ASSERT_ARE_EQUAL(void_ptr, head.Blink, &(simp2.link));
        ASSERT_ARE_EQUAL(void_ptr, simp1.link.Blink, &head);
        ASSERT_ARE_EQUAL(void_ptr, simp2.link.Blink, &(simp1.link));
    }

    /* Tests_SRS_DLIST_06_007: [DList_AppendTailList shall place the list defined by ListToAppend at the end of the list defined by the listHead parameter.] */
    TEST_FUNCTION(DList_AppendTailList_adds_listToAppend_after_listHead)
    {
        // arrange
        DLIST_ENTRY listHead = { 0 };
        PDLIST_ENTRY currentEntry;

        DList_InitializeListHead(&listHead);
        DList_InsertTailList(&listHead, &(simp1.link));

        DList_InitializeListHead(&simp2.link);
        DList_InsertTailList(&simp2.link, &simp4.link);
        DList_InsertTailList(&simp2.link, &simp5.link);
        // act
        DList_AppendTailList(&listHead, &simp2.link);

        // assert
        // Go forwards
        ASSERT_ARE_EQUAL(int, 0, DList_IsListEmpty(&listHead));
        currentEntry = listHead.Flink;
        ASSERT_ARE_EQUAL(void_ptr, currentEntry, &simp1.link);
        ASSERT_ARE_EQUAL(short, (short)1, containingRecord(currentEntry, simpleItem, link)->index);
        currentEntry = currentEntry->Flink;
        ASSERT_ARE_EQUAL(void_ptr, currentEntry, &simp2.link);
        ASSERT_ARE_EQUAL(short, (short)2, containingRecord(currentEntry, simpleItem, link)->index);
        currentEntry = currentEntry->Flink;
        ASSERT_ARE_EQUAL(void_ptr, currentEntry, &simp4.link);
        ASSERT_ARE_EQUAL(short, (short)4, containingRecord(currentEntry, simpleItem, link)->index);
        currentEntry = currentEntry->Flink;
        ASSERT_ARE_EQUAL(void_ptr, currentEntry, &simp5.link);
        ASSERT_ARE_EQUAL(short, (short)5, containingRecord(currentEntry, simpleItem, link)->index);
        currentEntry = currentEntry->Flink;
        ASSERT_ARE_EQUAL(void_ptr, currentEntry, &listHead);

        // Now back
        currentEntry = listHead.Blink;
        ASSERT_ARE_EQUAL(void_ptr, currentEntry, &simp5.link);
        ASSERT_ARE_EQUAL(short, (short)5, containingRecord(currentEntry, simpleItem, link)->index);
#ifdef _MSC_VER
#pragma warning(suppress: 6011) /* test code, should crash if this is truly NULL */
#endif
        currentEntry = currentEntry->Blink;
        ASSERT_ARE_EQUAL(void_ptr, currentEntry, &simp4.link);
        ASSERT_ARE_EQUAL(short, (short)4, containingRecord(currentEntry, simpleItem, link)->index);
        currentEntry = currentEntry->Blink;
        ASSERT_ARE_EQUAL(void_ptr, currentEntry, &simp2.link);
        ASSERT_ARE_EQUAL(short, (short)2, containingRecord(currentEntry, simpleItem, link)->index);
        currentEntry = currentEntry->Blink;
        ASSERT_ARE_EQUAL(void_ptr, currentEntry, &simp1.link);
        ASSERT_ARE_EQUAL(short, (short)1, containingRecord(currentEntry, simpleItem, link)->index);
        currentEntry = currentEntry->Blink;
        ASSERT_ARE_EQUAL(void_ptr, currentEntry, &listHead);
    }

    /* Tests_SRS_DLIST_06_010: [DList_RemoveEntryList shall return non-zero if the remaining list is empty.] */
    TEST_FUNCTION(DList_RemoveEntryList_with_head_only_in_list_shall_return_non_zero)
    {
        // arrange
        DLIST_ENTRY listHead = { 0 };
        int resultOfRemove;

        DList_InitializeListHead(&listHead);

        // act
        resultOfRemove = DList_RemoveEntryList(&listHead);

        // assert
        ASSERT_ARE_NOT_EQUAL(int, 0, resultOfRemove);
    }

    /* Tests_SRS_DLIST_06_008: [DList_RemoveEntryList shall remove a listEntry from whatever list it is properly part of.] */
    /* Tests_SRS_DLIST_06_010: [DList_RemoveEntryList shall return non-zero if the remaining list is empty.] */
    /* Tests_SRS_DLIST_06_009: [The remaining list is properly formed.] */
    TEST_FUNCTION(DList_RemoveEntryList_with_one_element_and_removing_that_one_element_shall_return_non_zero)
    {
        // arrange
        DLIST_ENTRY listHead = { 0 };
        int resultOfRemove;

        DList_InitializeListHead(&listHead);
        DList_InsertTailList(&listHead, &(simp1.link));

        // act
        resultOfRemove = DList_RemoveEntryList(&(simp1.link));

        // assert
        ASSERT_ARE_NOT_EQUAL(int, 0, resultOfRemove);
        ASSERT_ARE_EQUAL(void_ptr, &listHead, listHead.Blink);
        ASSERT_ARE_EQUAL(void_ptr, &listHead, listHead.Flink);
    }

    /* Tests_SRS_DLIST_06_008: [DList_RemoveEntryList shall remove a listEntry from whatever list it is properly part of.] */
    /* Tests_SRS_DLIST_06_010: [DList_RemoveEntryList shall return non-zero if the remaining list is empty.] */
    TEST_FUNCTION(DList_RemoveEntryList_with_one_element_and_removing_the_head_shall_return_non_zero)
    {
        // arrange
        DLIST_ENTRY listHead = { 0 };
        int resultOfRemove;

        DList_InitializeListHead(&listHead);
        DList_InsertTailList(&listHead, &(simp1.link));

        // act
        resultOfRemove = DList_RemoveEntryList(&listHead);

        // assert
        ASSERT_ARE_NOT_EQUAL(int, 0, resultOfRemove);
        ASSERT_ARE_EQUAL(void_ptr, &(simp1.link), simp1.link.Blink);
        ASSERT_ARE_EQUAL(void_ptr, &(simp1.link), simp1.link.Flink);
    }

    /* Tests_SRS_DLIST_06_008: [DList_RemoveEntryList shall remove a listEntry from whatever list it is properly part of.] */
    /* Tests_SRS_DLIST_06_009: [The remaining list is properly formed.] */
    /* Tests_SRS_DLIST_06_010: [DList_RemoveEntryList shall return non-zero if the remaining list is empty.] */
    /* Tests_SRS_DLIST_06_011: [DList_RemoveEntryList shall return zero if the remaining list is NOT empty.] */
    TEST_FUNCTION(DList_RemoveEntryList_with_three_elements_and_removing_the_first_return_zero)
    {
        // arrange
        DLIST_ENTRY listHead = { 0 };
        int resultOfRemove;

        DList_InitializeListHead(&listHead);
        DList_InsertTailList(&listHead, &(simp1.link));
        DList_InsertTailList(&listHead, &(simp2.link));
        DList_InsertTailList(&listHead, &(simp3.link));

        // act
        resultOfRemove = DList_RemoveEntryList(&(simp1.link));

        // assert
        ASSERT_ARE_EQUAL(int, 0, resultOfRemove);
        ASSERT_ARE_EQUAL(void_ptr, listHead.Flink, &(simp2.link));
        ASSERT_ARE_EQUAL(void_ptr, simp2.link.Flink, &(simp3.link));
        ASSERT_ARE_EQUAL(void_ptr, simp3.link.Flink, &listHead);
        ASSERT_ARE_EQUAL(void_ptr, listHead.Blink, &(simp3.link));
        ASSERT_ARE_EQUAL(void_ptr, simp3.link.Blink, &(simp2.link));
        ASSERT_ARE_EQUAL(void_ptr, simp2.link.Blink, &(listHead));
    }
    
    /* Tests_SRS_DLIST_06_008: [DList_RemoveEntryList shall remove a listEntry from whatever list it is properly part of.] */
    /* Tests_SRS_DLIST_06_009: [The remaining list is properly formed.] */
    /* Tests_SRS_DLIST_06_010: [DList_RemoveEntryList shall return non-zero if the remaining list is empty.] */
    /* Tests_SRS_DLIST_06_011: [DList_RemoveEntryList shall return zero if the remaining list is NOT empty.] */
    TEST_FUNCTION(DList_RemoveEntryList_with_three_elements_and_removing_the_last_return_zero)
    {
        // arrange
        DLIST_ENTRY listHead = { 0 };
        int resultOfRemove;

        DList_InitializeListHead(&listHead);
        DList_InsertTailList(&listHead, &(simp1.link));
        DList_InsertTailList(&listHead, &(simp2.link));
        DList_InsertTailList(&listHead, &(simp3.link));

        // act
        resultOfRemove = DList_RemoveEntryList(&(simp3.link));

        // assert
        ASSERT_ARE_EQUAL(int, 0, resultOfRemove);
        ASSERT_ARE_EQUAL(void_ptr, listHead.Flink, &(simp1.link));
        ASSERT_ARE_EQUAL(void_ptr, simp1.link.Flink, &(simp2.link));
        ASSERT_ARE_EQUAL(void_ptr, simp2.link.Flink, &listHead);
        ASSERT_ARE_EQUAL(void_ptr, listHead.Blink, &(simp2.link));
        ASSERT_ARE_EQUAL(void_ptr, simp2.link.Blink, &(simp1.link));
        ASSERT_ARE_EQUAL(void_ptr, simp1.link.Blink, &(listHead));
    }

    /* Tests_SRS_DLIST_06_008: [DList_RemoveEntryList shall remove a listEntry from whatever list it is properly part of.] */
    /* Tests_SRS_DLIST_06_009: [The remaining list is properly formed.] */
    /* Tests_SRS_DLIST_06_010: [DList_RemoveEntryList shall return non-zero if the remaining list is empty.] */
    /* Tests_SRS_DLIST_06_011: [DList_RemoveEntryList shall return zero if the remaining list is NOT empty.] */
    TEST_FUNCTION(DList_RemoveEntryList_with_three_elements_and_removing_the_middle_return_zero)
    {
        // arrange
        DLIST_ENTRY listHead = { 0 };
        int resultOfRemove;

        DList_InitializeListHead(&listHead);
        DList_InsertTailList(&listHead, &(simp1.link));
        DList_InsertTailList(&listHead, &(simp2.link));
        DList_InsertTailList(&listHead, &(simp3.link));

        // act
        resultOfRemove = DList_RemoveEntryList(&(simp2.link));

        // assert
        ASSERT_ARE_EQUAL(int, 0, resultOfRemove);
        ASSERT_ARE_EQUAL(void_ptr, listHead.Flink, &(simp1.link));
        ASSERT_ARE_EQUAL(void_ptr, simp1.link.Flink, &(simp3.link));
        ASSERT_ARE_EQUAL(void_ptr, simp3.link.Flink, &listHead);
        ASSERT_ARE_EQUAL(void_ptr, listHead.Blink, &(simp3.link));
        ASSERT_ARE_EQUAL(void_ptr, simp3.link.Blink, &(simp1.link));
        ASSERT_ARE_EQUAL(void_ptr, simp1.link.Blink, &(listHead));
    }

    /* Tests_SRS_DLIST_06_013: [DList_RemoveHeadList shall return listHead if that's the only item in the list.] */
    TEST_FUNCTION(DList_RemoveHeadList_with_only_head_shall_return_the_head)
    {
        // arrange
        DLIST_ENTRY head = { 0 };
        PDLIST_ENTRY returnedEntry;

        DList_InitializeListHead(&head);

        // act
        returnedEntry = DList_RemoveHeadList(&head);

        // assert
        ASSERT_ARE_EQUAL(void_ptr, &head, returnedEntry);
    }

    /* Tests_SRS_DLIST_06_012: [DList_RemoveHeadList removes the oldest entry from the list defined by the listHead parameter and returns a pointer to that entry.] */
    TEST_FUNCTION(DList_RemoveHeadList_with_one_entry_returns_entry)
    {
        // arrange
        DLIST_ENTRY listHead = { 0 };
        PDLIST_ENTRY returnedEntry;

        DList_InitializeListHead(&listHead);
        DList_InsertTailList(&listHead, &(simp1.link));

        // act
        returnedEntry = DList_RemoveHeadList(&listHead);

        // assert
        ASSERT_ARE_NOT_EQUAL(int, 0, DList_IsListEmpty(&listHead));
        ASSERT_ARE_EQUAL(void_ptr, &(simp1.link), returnedEntry);
    }

    /* Tests_SRS_DLIST_06_012: [DList_RemoveHeadList removes the oldest entry from the list defined by the listHead parameter and returns a pointer to that entry.] */
    TEST_FUNCTION(DList_RemoveHeadList_with_two_entries_returns_first_entry)
    {
        // arrange
        DLIST_ENTRY listHead = { 0 };
        PDLIST_ENTRY returnedEntry;

        DList_InitializeListHead(&listHead);
        DList_InsertTailList(&listHead, &(simp2.link));
        DList_InsertTailList(&listHead, &(simp1.link));

        // act
        returnedEntry = DList_RemoveHeadList(&listHead);

        // assert
        ASSERT_ARE_EQUAL(int, 0, DList_IsListEmpty(&listHead));
        ASSERT_ARE_EQUAL(void_ptr, &(simp2.link), returnedEntry);
    }

    /*Tests_SRS_DLIST_02_002: [DList_InsertHeadList inserts a singular entry in the list having as head listHead after "head".]*/
    TEST_FUNCTION(DList_InsertHeadList_with_empty_list_succeeds)
    {
        ///arrange
        DLIST_ENTRY listHead;
        DList_InitializeListHead(&listHead);

        DLIST_ENTRY toBeInserted;

        ///act
        DList_InsertHeadList(&listHead, &toBeInserted);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, listHead.Flink, &toBeInserted);
        ASSERT_ARE_EQUAL(void_ptr, listHead.Blink, &toBeInserted);
        ASSERT_ARE_EQUAL(void_ptr, toBeInserted.Flink , &listHead);
        ASSERT_ARE_EQUAL(void_ptr, toBeInserted.Blink, &listHead);
    }

    /*Tests_SRS_DLIST_02_002: [DList_InsertHeadList inserts a singular entry in the list having as head listHead after "head".]*/
    TEST_FUNCTION(DList_InsertHeadList_with_1_item_in_list_succeeds)
    {
        ///arrange
        DLIST_ENTRY listHead;
        DList_InitializeListHead(&listHead);
        DLIST_ENTRY existingInList;
        DLIST_ENTRY toBeInserted;
        DList_InsertTailList(&listHead, &existingInList); /*would be same as insertHead when it is the first item... */

        ///act
        DList_InsertHeadList(&listHead, &toBeInserted);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, listHead.Flink, &toBeInserted);
        ASSERT_ARE_EQUAL(void_ptr, listHead.Blink, &existingInList);
        ASSERT_ARE_EQUAL(void_ptr, existingInList.Flink, &listHead);
        ASSERT_ARE_EQUAL(void_ptr, existingInList.Blink, &toBeInserted);
        ASSERT_ARE_EQUAL(void_ptr, toBeInserted.Flink, &existingInList);
        ASSERT_ARE_EQUAL(void_ptr, toBeInserted.Blink, &listHead);

    }

    
END_TEST_SUITE(doublylinkedlist_unittests)

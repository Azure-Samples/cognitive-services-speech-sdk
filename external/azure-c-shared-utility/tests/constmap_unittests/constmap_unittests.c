// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <string.h>

#include "testrunnerswitcher.h"

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

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

#define ENABLE_MOCKS
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/gballoc.h"

#undef ENABLE_MOCKS
#include "azure_c_shared_utility/constmap.h"

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

TEST_DEFINE_ENUM_TYPE(CONSTMAP_RESULT, CONSTMAP_RESULT_VALUES);

#define VALID_MAP_HANDLE    (MAP_HANDLE)0xDEAF
#define VALID_CONST_CHAR_POINTER (const char*const*)0xDADA
#define VALID_MAP_CLONE1     (MAP_HANDLE)0xDEDE
#define VALID_MAP_CLONE2     (MAP_HANDLE)0xDEDD
#define INVALID_MAP_HANDLE  (MAP_HANDLE)0xDEAD
#define INVALID_CLONE_HANDLE  (MAP_HANDLE)0xDEAE
#define VALID_KV_COUNT		(size_t)100
#define VALID_VALUE			"value"

static MAP_RESULT currentMapResult;

TEST_DEFINE_ENUM_TYPE(MAP_RESULT, MAP_RESULT_VALUES);

MAP_HANDLE my_Map_Clone(MAP_HANDLE sourceMap)
{
    MAP_HANDLE result;

    if (sourceMap == VALID_MAP_HANDLE)
    {
        result = VALID_MAP_CLONE1;
    }
    else if (sourceMap == VALID_MAP_CLONE1)
    {
        result = VALID_MAP_CLONE2;
    }
    else if (sourceMap == INVALID_CLONE_HANDLE)
    {
        result = INVALID_MAP_HANDLE;
    }
    else if (sourceMap == INVALID_MAP_HANDLE)
    {
        result = NULL;
    }

    return result;
}

MAP_RESULT my_Map_ContainsKey(MAP_HANDLE handle, const char* key, bool* keyExists)
{
    MAP_RESULT result = currentMapResult;
    if (result == MAP_OK)
    {
        *keyExists = true;
    }
    return result;
}

MAP_RESULT my_Map_ContainsValue(MAP_HANDLE handle, const char* value, bool* valueExists)
{
    MAP_RESULT result = currentMapResult;
    if (result == MAP_OK)
    {
        *valueExists = true;
    }
    return result;
}

const char* my_Map_GetValueFromKey(MAP_HANDLE sourceMap, const char* key)
{
    const char* result;
    if (currentMapResult == MAP_OK)
    {
        result = VALID_VALUE;
    }
    else
    {
        result = NULL;
    }
    return result;
}

MAP_RESULT my_Map_GetInternals(MAP_HANDLE handle, const char*const** keys, const char*const** values, size_t* count)
{
    MAP_RESULT result = currentMapResult;
    *keys = VALID_CONST_CHAR_POINTER;
    *values = VALID_CONST_CHAR_POINTER;
    *count = VALID_KV_COUNT;
    return result;
}

void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(constmap_unittests)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        int result;
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        umock_c_init(on_umock_c_error);
    
        REGISTER_UMOCK_ALIAS_TYPE(CONSTMAP_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(MAP_HANDLE, void*);
        result = umocktypes_charptr_register_types();
        ASSERT_ARE_EQUAL(int, 0, result);

        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
        REGISTER_GLOBAL_MOCK_HOOK(Map_Clone, my_Map_Clone);
        REGISTER_GLOBAL_MOCK_HOOK(Map_ContainsKey, my_Map_ContainsKey);
        REGISTER_GLOBAL_MOCK_HOOK(Map_ContainsValue, my_Map_ContainsValue);
        REGISTER_GLOBAL_MOCK_HOOK(Map_GetValueFromKey, my_Map_GetValueFromKey);
        REGISTER_GLOBAL_MOCK_HOOK(Map_GetInternals, my_Map_GetInternals);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        umock_c_deinit();

        TEST_MUTEX_DESTROY(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        if (TEST_MUTEX_ACQUIRE(g_testByTest) != 0)
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }
        currentmalloc_call = 0;
        whenShallmalloc_fail = 0;
		currentMapResult = MAP_OK;

        umock_c_reset_all_calls();
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        TEST_MUTEX_RELEASE(g_testByTest);
    }

	/*Tests_SRS_CONSTMAP_17_001: [ConstMap_Create shall create an immutable map, populated by the key, value pairs in the source map.]*/
	/*Tests_SRS_CONSTMAP_17_048: [ConstMap_Create shall accept any non-NULL MAP_HANDLE as input.]*/
	/*Tests_SRS_CONSTMAP_17_003: [Otherwise, it shall return a non-NULL handle that can be used in subsequent calls.]*/
	/*Tests_SRS_CONSTMAP_17_004: [If the reference count is zero, ConstMap_Destroy shall release all resources associated with the immutable map.]*/
    TEST_FUNCTION(ConstMap_Create_Destroy_Success)
    {
		// Arrange
		STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(Map_Clone(VALID_MAP_HANDLE));

		STRICT_EXPECTED_CALL(Map_Destroy(VALID_MAP_CLONE1));
		STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
			.IgnoreArgument(1);

        MAP_HANDLE sourceMap = VALID_MAP_HANDLE;

		///Act
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);

		ASSERT_IS_NOT_NULL(aHandle);

		ConstMap_Destroy(aHandle);

		///Assert
		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution                

    }

	/* Tests_SRS_CONSTMAP_17_002: [If during creation there are any errors, then ConstMap_Create shall return NULL.]*/
    TEST_FUNCTION(ConstMap_Create_Malloc_Failed)
    {
		// Arrange
		STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
			.IgnoreArgument(1);
		whenShallmalloc_fail = 1;

		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;

		///Act
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);

		///Assert
		ASSERT_IS_NULL(aHandle);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution                

    }

	/*Tests_SRS_CONSTMAP_17_002: [If during creation there are any errors, then ConstMap_Create shall return NULL.] */
    TEST_FUNCTION(ConstMap_Clone_Map_Failed)
    {
		// Arrange
		STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(Map_Clone(INVALID_MAP_HANDLE));
		STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
			.IgnoreArgument(1);

		MAP_HANDLE sourceMap = INVALID_MAP_HANDLE;

		///Act
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);

		///Assert
		ASSERT_IS_NULL(aHandle);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution                

    }

	/*Tests_SRS_CONSTMAP_17_039: [ConstMap_Clone shall increase the internal reference count of the immutable map indicated by parameter handle] */
	TEST_FUNCTION(ConstMap_Clone_Destroy_Success)
	{
		// Arrange
		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);
		CONSTMAP_HANDLE aClone = ConstMap_Clone(aHandle);

		umock_c_reset_all_calls();

		///Act
		ConstMap_Destroy(aClone);

		///Assert
		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aHandle);

	}
	/*Tests_SRS_CONSTMAP_17_005: [If parameter handle is NULL then ConstMap_Destroy shall take no action.]*/
	TEST_FUNCTION(ConstMap_Destroy_Null)
	{
		///Arrange
		CONSTMAP_HANDLE handle = NULL;

		///Act
		ConstMap_Destroy(handle);

		///Assert
		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		///Ablution
	}

	/*Tests_SRS_CONSTMAP_17_039: [ConstMap_Clone shall increase the internal reference count of the immutable map indicated by parameter handle]*/
	/*Tests_SRS_CONSTMAP_17_050: [ConstMap_Clone shall return the non-NULL handle. ]*/
	TEST_FUNCTION(ConstMap_Clone_Success)
	{
		// Arrange
		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);

		umock_c_reset_all_calls();

		///Act
		CONSTMAP_HANDLE aClone = ConstMap_Clone(aHandle);

		///Assert
		ASSERT_ARE_EQUAL(void_ptr, aHandle, aClone);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aClone);
		ConstMap_Destroy(aHandle);

	}

	/*Tests_SRS_CONSTMAP_17_038: [ConstMap_Clone returns NULL if parameter handle is NULL.] */
	TEST_FUNCTION(ConstMap_Clone_Null)
	{
		// Arrange
		CONSTMAP_HANDLE aHandle = NULL;

		///Act
		CONSTMAP_HANDLE aClone = ConstMap_Clone(aHandle);

		///Assert
		ASSERT_IS_NULL(aClone);
		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution       
	}

	/*Tests_SRS_CONSTMAP_17_052: [ConstMap_CloneWriteable shall create a new, writeable map, populated by the key, value pairs in the parameter defined by handle.]*/
	/*Tests_SRS_CONSTMAP_17_054: [Otherwise, ConstMap_CloneWriteable shall return a non-NULL handle that can be used in subsequent calls.]*/
	TEST_FUNCTION(ConstMap_CloneWritable_Success)
	{
		// Arrange
		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);
		MAP_HANDLE newMap = NULL;

		umock_c_reset_all_calls();

		STRICT_EXPECTED_CALL(Map_Clone(VALID_MAP_CLONE1)).IgnoreArgument(1);

		//Act 
		newMap = ConstMap_CloneWriteable(aHandle);

		//Assert
		ASSERT_ARE_EQUAL(void_ptr, VALID_MAP_CLONE2, newMap);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution
		ConstMap_Destroy(aHandle);
		Map_Destroy(sourceMap);
		Map_Destroy(newMap);
	}

	/*Tests_SRS_CONSTMAP_17_053: [If during cloning, any operation fails, then ConstMap_CloneWriteableap_Clone shall return NULL.]*/
	TEST_FUNCTION(ConstMap_CloneWritable_Fail)
	{
		// Arrange
		MAP_HANDLE sourceMap = INVALID_CLONE_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);
		MAP_HANDLE newMap = NULL;

		umock_c_reset_all_calls();

		STRICT_EXPECTED_CALL(Map_Clone(INVALID_MAP_HANDLE)).IgnoreArgument(1);

		//Act 
		newMap = ConstMap_CloneWriteable(aHandle);

		//Assert
		ASSERT_IS_NULL(newMap);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution
		ConstMap_Destroy(aHandle);
		Map_Destroy(sourceMap);
	}

	/*Tests_SRS_CONSTMAP_17_051: [ConstMap_CloneWriteable returns NULL if parameter handle is NULL. ]*/
	TEST_FUNCTION(ConstMap_CloneWritable_NULL)
	{
		// Arrange
		MAP_HANDLE newMap = NULL;

		//Act 
		newMap = ConstMap_CloneWriteable(NULL);

		//Assert
		ASSERT_IS_NULL(newMap);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution
	}


	/*Tests_SRS_CONSTMAP_17_025: [Otherwise if a key exists then ConstMap_ContainsKey shall return true.]*/
	TEST_FUNCTION(ConstMap_ContainsKey_Success)
	{
		// Arrange
		const char * key = "aKey";
		bool keyExists;

		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);

		umock_c_reset_all_calls();

		// Call to Map
		STRICT_EXPECTED_CALL(Map_ContainsKey(IGNORED_PTR_ARG, key, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);

		///Act
		keyExists = ConstMap_ContainsKey(aHandle, key);


		///Assert
		ASSERT_IS_TRUE(keyExists);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aHandle);
	}

	/*Tests_SRS_CONSTMAP_17_024: [If parameter handle or key are NULL then ConstMap_ContainsKey shall return false.]*/
	TEST_FUNCTION(ConstMap_ContainsKey_Null)
	{
		// Arrange
		const char * key1 = "aKey";
		const char * key2 = NULL;
		bool keyExists1;
		bool keyExists2;
		CONSTMAP_HANDLE aHandle1 = NULL;
		CONSTMAP_HANDLE aHandle2;
		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;

		aHandle2 = ConstMap_Create(sourceMap);
		umock_c_reset_all_calls();

		///Act
		// NULL Handle
		keyExists1 = ConstMap_ContainsKey(aHandle1, key1);

		// NULL key
		keyExists2 = ConstMap_ContainsKey(aHandle2, key2);

		///Assert
		ASSERT_IS_FALSE(keyExists1);
		ASSERT_IS_FALSE(keyExists2);
		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aHandle2);

	}

	/*Tests_SRS_CONSTMAP_17_026: [If a key doesn't exist, then ConstMap_ContainsKey shall return false.]*/
	TEST_FUNCTION(ConstMap_ContainsKey_Failures)
	{
		// Arrange
		const char * key = "aKey";
		bool keyExists;

		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);
		umock_c_reset_all_calls();

		// Call to Map_ContainsKey (match with mapErrorList size)
		STRICT_EXPECTED_CALL(Map_ContainsKey(IGNORED_PTR_ARG, key, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);
		STRICT_EXPECTED_CALL(Map_ContainsKey(IGNORED_PTR_ARG, key, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);
		STRICT_EXPECTED_CALL(Map_ContainsKey(IGNORED_PTR_ARG, key, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);
		STRICT_EXPECTED_CALL(Map_ContainsKey(IGNORED_PTR_ARG, key, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);
		STRICT_EXPECTED_CALL(Map_ContainsKey(IGNORED_PTR_ARG, key, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);


		///Act
		MAP_RESULT mapErrorList[] = {
			MAP_ERROR,
			MAP_INVALIDARG, 
			MAP_KEYEXISTS, 
			MAP_KEYNOTFOUND, 
			MAP_FILTER_REJECT
		};
		size_t errors = sizeof(mapErrorList) / sizeof(MAP_RESULT);
		CONSTMAP_RESULT constErrorList[] = {
			CONSTMAP_ERROR,
			CONSTMAP_INVALIDARG,
			CONSTMAP_ERROR,
			CONSTMAP_KEYNOTFOUND,
			CONSTMAP_ERROR
		};

		for (size_t e = 0; e < errors; e++)
		{
			currentMapResult = mapErrorList[e];
			keyExists = ConstMap_ContainsKey(aHandle, key);
			ASSERT_IS_FALSE(keyExists);
		}

		///Assert

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aHandle);
	}

	/*Tests_SRS_CONSTMAP_17_028: [Otherwise, if a pair has its value equal to the parameter value, the ConstMap_ContainsValue shall return true.]*/
	TEST_FUNCTION(ConstMap_ContainsValue_Success)
	{
		// Arrange
		const char * value = "aValue";
		bool valueExists;

		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);

		umock_c_reset_all_calls();

		// Call to Map
		STRICT_EXPECTED_CALL(Map_ContainsValue(IGNORED_PTR_ARG, value, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);

		///Act
		valueExists = ConstMap_ContainsValue(aHandle, value);

		///Assert
		ASSERT_IS_TRUE(valueExists);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aHandle);
	}

	/*Tests_SRS_CONSTMAP_17_027: [If parameter handle or value is NULL then ConstMap_ContainsValue shall return false.]*/
	TEST_FUNCTION(ConstMap_ContainsValue_Null)
	{
		// Arrange
		const char * value1 = "aValue";
		bool valueExists1;
		const char * value2 = NULL;
		bool valueExists2;
		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle1 = NULL;
		CONSTMAP_HANDLE aHandle2 = NULL;

		aHandle2 = ConstMap_Create(sourceMap);

		umock_c_reset_all_calls();

		///Act
		valueExists1 = ConstMap_ContainsValue(aHandle1, value1);
		valueExists2 = ConstMap_ContainsValue(aHandle2, value2);


		///Assert
		ASSERT_IS_FALSE(valueExists1);
		ASSERT_IS_FALSE(valueExists2);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution   
		ConstMap_Destroy(aHandle2);
	}

	/* Tests_SRS_CONSTMAP_17_029: [Otherwise, if such a does not exist, then ConstMap_ContainsValue shall return false.]*/
	TEST_FUNCTION(ConstMap_ContainsValue_Failures)
	{
		// Arrange
		const char * value = "aValue";
		bool valueExists;

		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);
		umock_c_reset_all_calls();

		// Call to Map_ContainsValue (match with mapErrorList size)
		STRICT_EXPECTED_CALL(Map_ContainsValue(IGNORED_PTR_ARG, value, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);
		STRICT_EXPECTED_CALL(Map_ContainsValue(IGNORED_PTR_ARG, value, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);
		STRICT_EXPECTED_CALL(Map_ContainsValue(IGNORED_PTR_ARG, value, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);
		STRICT_EXPECTED_CALL(Map_ContainsValue(IGNORED_PTR_ARG, value, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);
		STRICT_EXPECTED_CALL(Map_ContainsValue(IGNORED_PTR_ARG, value, IGNORED_PTR_ARG))
			.IgnoreArgument(1).IgnoreArgument(3);

		///Act
		MAP_RESULT mapErrorList[] = {
			MAP_ERROR,
			MAP_INVALIDARG,
			MAP_KEYEXISTS,
			MAP_KEYNOTFOUND,
			MAP_FILTER_REJECT
		};
		size_t errors = sizeof(mapErrorList) / sizeof(MAP_RESULT);
		CONSTMAP_RESULT constErrorList[] = {
			CONSTMAP_ERROR,
			CONSTMAP_INVALIDARG,
			CONSTMAP_ERROR,
			CONSTMAP_KEYNOTFOUND,
			CONSTMAP_ERROR
		};

		for (size_t e = 0; e < errors; e++)
		{
			currentMapResult = mapErrorList[e];
			valueExists = ConstMap_ContainsValue(aHandle, value);
			ASSERT_IS_FALSE(valueExists);
		}

		///Assert

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aHandle);
	}

	/* Tests_SRS_CONSTMAP_17_042: [Otherwise, ConstMap_GetValue returns the key's value.]*/
	TEST_FUNCTION(ConstMap_GetValue_Success)
	{
		// Arrange
		const char * key = "aKey";
		const char * value;

		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);
		umock_c_reset_all_calls();

		// Call to Map
		STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, key))
			.IgnoreArgument(1);

		///Act
		value = ConstMap_GetValue(aHandle, key);


		///Assert
		ASSERT_ARE_EQUAL(char_ptr, VALID_VALUE, value);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aHandle);

	}

	TEST_FUNCTION(ConstMap_GetValue_Null)
	{
		// Arrange
		const char * key1 = "aKey";
		const char * value1;
		CONSTMAP_HANDLE aHandle1 = NULL;
		const char * key2 = NULL;
		const char * value2;
		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle2 = ConstMap_Create(sourceMap);

		umock_c_reset_all_calls();

		///Act
		value1 = ConstMap_GetValue(aHandle1, key1);
		value2 = ConstMap_GetValue(aHandle2, key2);

		///Assert
		ASSERT_IS_NULL(value1);
		ASSERT_IS_NULL(value2);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution   
		ConstMap_Destroy(aHandle2);

	}

	/*Tests_SRS_CONSTMAP_17_040: [If parameter handle or key is NULL then ConstMap_GetValue returns NULL.] */
	/*Tests_SRS_CONSTMAP_17_041: [If the key is not found, then ConstMap_GetValue returns NULL.]*/
	TEST_FUNCTION(ConstMap_GetValue_Failures)
	{
		// Arrange
		const char * key = "aKey";
		const char * value;

		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);
		umock_c_reset_all_calls();

		// Call to Map_ContainsKey (match with mapErrorList size)
		STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, key))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, key))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, key))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, key))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(Map_GetValueFromKey(IGNORED_PTR_ARG, key))
			.IgnoreArgument(1);


		///Act
		MAP_RESULT mapErrorList[] = {
			MAP_ERROR,
			MAP_INVALIDARG,
			MAP_KEYEXISTS,
			MAP_KEYNOTFOUND,
			MAP_FILTER_REJECT
		};
		size_t errors = sizeof(mapErrorList) / sizeof(MAP_RESULT);
		CONSTMAP_RESULT constErrorList[] = {
			CONSTMAP_ERROR,
			CONSTMAP_INVALIDARG,
			CONSTMAP_ERROR,
			CONSTMAP_KEYNOTFOUND,
			CONSTMAP_ERROR
		};

		// Errors from Map_GetValueFromKey
		for (size_t e = 0; e < errors; e++)
		{
			currentMapResult = mapErrorList[e];
			value = ConstMap_GetValue(aHandle, key);
			ASSERT_IS_NULL(value);
		}

		///Assert

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aHandle);
	}

	/*Tests_SRS_CONSTMAP_17_043: [ConstMap_GetInternals shall produce in *keys a pointer to an array of const char* having all the keys stored so far by the map.] */
	/*Tests_SRS_CONSTMAP_17_044: [ConstMap_GetInternals shall produce in *values a pointer to an array of const char* having all the values stored so far by the map.] */
	/*Tests_SRS_CONSTMAP_17_045: [ ConstMap_GetInternals shall produce in *count the number of stored keys and values.]*/
	TEST_FUNCTION(ConstMap_GetInternals_Success)
	{
		// Arrange
		const char*const* keys;
		const char*const* values;
		size_t count;

		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);
		umock_c_reset_all_calls();

		// Call to Map
		STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, &keys, &values, &count))
			.IgnoreArgument(1);

		///Act
		CONSTMAP_RESULT result = ConstMap_GetInternals(aHandle, &keys, &values, &count);

		///Assert
		ASSERT_ARE_EQUAL(CONSTMAP_RESULT, CONSTMAP_OK, result);
		ASSERT_ARE_EQUAL(void_ptr, VALID_CONST_CHAR_POINTER, keys);
		ASSERT_ARE_EQUAL(void_ptr, VALID_CONST_CHAR_POINTER, values);
		ASSERT_ARE_EQUAL(int, VALID_KV_COUNT, count);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aHandle);
	}

	/*Tests_SRS_CONSTMAP_17_046: [If parameter handle, keys, values or count is NULL then ConstMap_GetInternals shall return CONSTMAP_INVALIDARG.]*/
	TEST_FUNCTION(ConstMap_GetInternals_Null)
	{
		// Arrange
		const char*const* keys;
		const char*const* values;
		size_t count;

		CONSTMAP_HANDLE aHandle = NULL;

		///Act
		CONSTMAP_RESULT result = ConstMap_GetInternals(aHandle, &keys, &values, &count);

		///Assert
		ASSERT_ARE_EQUAL(CONSTMAP_RESULT, CONSTMAP_INVALIDARG, result);

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
	}

	TEST_FUNCTION(ConstMap_GetInternals_Failures)
	{
		// Arrange
		const char*const* keys;
		const char*const* values;
		size_t count;

		MAP_HANDLE sourceMap = VALID_MAP_HANDLE;
		CONSTMAP_HANDLE aHandle = ConstMap_Create(sourceMap);

		umock_c_reset_all_calls();

		// Call to Map_GetInternals (match with mapErrorList size)
		STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, &keys, &values, &count))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, &keys, &values, &count))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, &keys, &values, &count))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, &keys, &values, &count))
			.IgnoreArgument(1);
		STRICT_EXPECTED_CALL(Map_GetInternals(IGNORED_PTR_ARG, &keys, &values, &count))
			.IgnoreArgument(1);

		///Act
		MAP_RESULT mapErrorList[] = {
			MAP_ERROR,
			MAP_INVALIDARG,
			MAP_KEYEXISTS,
			MAP_KEYNOTFOUND,
			MAP_FILTER_REJECT
		};
		size_t errors = sizeof(mapErrorList) / sizeof(MAP_RESULT);
		CONSTMAP_RESULT constErrorList[] = {
			CONSTMAP_ERROR,
			CONSTMAP_INVALIDARG,
			CONSTMAP_ERROR,
			CONSTMAP_KEYNOTFOUND,
			CONSTMAP_ERROR
		};

		for (size_t e = 0; e < errors; e++)
		{
			currentMapResult = mapErrorList[e];
			CONSTMAP_RESULT result = ConstMap_GetInternals(aHandle, &keys, &values, &count);
			ASSERT_ARE_EQUAL(CONSTMAP_RESULT, constErrorList[e], result);
		}

		///Assert

		ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

		//Ablution    
		ConstMap_Destroy(aHandle);
	}

END_TEST_SUITE(constmap_unittests)

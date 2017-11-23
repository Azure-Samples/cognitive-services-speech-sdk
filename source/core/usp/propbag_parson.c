// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/gballoc.h"
#include "iot-cortana-sdk.h"
#include "parson.h"

int propertybag_initialize()
{
    static int isInit = 0;
    if (!isInit)
    {
        isInit = 1;
#if defined(GB_DEBUG_ALLOC)
        json_set_allocation_functions(gballoc_malloc, gballoc_free);
#endif
    }

    return 0;
}

void propertybag_shutdown()
{
}

int propertybag_deserialize_json(
	const char*						pszJson,
	size_t							size,
	PPROPERTYBAG_OBJECT_CALLBACK	pfncallback,
	void*                           pContext
)
{
	JSON_Value * pVal;

	(void)size;

    propertybag_initialize();

	if (NULL == pszJson ||
		NULL == pfncallback)
	{
		return -1;
	}

	pVal = json_parse_string(pszJson);
	if (!pVal)
	{
		return -1;
	}

	pfncallback(
		(PROPERTYBAG_HANDLE)json_value_get_object(pVal),
		pContext);

	json_value_free(pVal);

	return 0;
}

int propertybag_enumarray(
	PROPERTYBAG_HANDLE           hProperty,
	const char*                  pszName,
	PPROPERTYBAG_OBJECT_CALLBACK pfncallback,
	void*                        pContext
)
{
	if (NULL == hProperty ||
		NULL == pfncallback ||
		NULL == pszName)
	{
		return -1;
	}

	JSON_Array *pArray = json_object_get_array((JSON_Object*)hProperty, pszName);
	if (pArray)
	{
		size_t count = json_array_get_count(pArray);
		while (count--)
		{
			pfncallback(
				(PROPERTYBAG_HANDLE)json_array_get_object(pArray, count), 
				pContext);
		}
	}

	return 0;
}

int propertybag_getchildvalue(
	PROPERTYBAG_HANDLE              hProperty,
	const char*                     pszName,
    PPROPERTYBAG_OBJECT_CALLBACK	pfncallback,
    void*                           pContext
)
{
	if (NULL == pfncallback)
	{
		return -1;
	}

    PROPERTYBAG_HANDLE prop = propertybag_getchildvalue2(hProperty, pszName);
	if (!prop)
	{
		return -1;
	}

	pfncallback(prop, pContext);

	return 0;
}

PROPERTYBAG_HANDLE propertybag_getchildvalue2(
    PROPERTYBAG_HANDLE              hProperty,
    const char*                     pszName
)
{
    if (NULL == hProperty ||
        NULL == pszName)
    {
        return NULL;
    }

    const JSON_Value *pVal = json_object_dotget_value((JSON_Object*)hProperty, pszName);
    if (!pVal)
    {
        return NULL;
    }

    return (PROPERTYBAG_HANDLE)json_value_get_object(pVal);
}

const char* propertybag_getstringvalue(
	PROPERTYBAG_HANDLE	hProperty,
	const char*			pszName
	)
{
	if (NULL == hProperty ||
		NULL == pszName)
	{
		return NULL;
	}

	return json_object_get_string((JSON_Object*)hProperty, pszName);
}

double propertybag_getnumbervalue(
    PROPERTYBAG_HANDLE               hProperty,
    const char*                      pszName)
{
	if (NULL == hProperty ||
		NULL == pszName)
	{
		return 0.0f;
	}

	return json_object_get_number((JSON_Object*)hProperty, pszName);
}

unsigned int propertybag_getbooleanvalue(
    PROPERTYBAG_HANDLE               hProperty,
    const char*                      pszName)
{
    if (NULL == hProperty ||
        NULL == pszName)
    {
        return 0;
    }

    return json_object_get_boolean((JSON_Object*)hProperty, pszName);
}

STRING_HANDLE propertybag_serialize(
    PPROPERTYBAG_OBJECT_CALLBACK pfncallback, 
    void*                        pContext)
{
    STRING_HANDLE ret = NULL;
    JSON_Value *rootValue;
    char *pszText;

    propertybag_initialize();

    if (pfncallback)
    {
        rootValue = json_value_init_object();
        if (rootValue)
        {
            (void)pfncallback(json_value_get_object(rootValue), pContext);

            pszText = json_serialize_to_string(rootValue);
            ret = STRING_construct(pszText);
            json_free_serialized_string(pszText);
            json_value_free(rootValue);
        }
    }

    return ret;
}

int propertybag_serialize_object(
    PROPERTYBAG_HANDLE           hProperty,
    const char*                  pszName,
    PPROPERTYBAG_OBJECT_CALLBACK pfncallback,
    void*                        pContext)
{
    JSON_Object* pParent = (JSON_Object*)hProperty;
    JSON_Value*  rootValue;

    if (hProperty &&
        pszName   &&
        pfncallback)
    {
        rootValue = json_value_init_object();
        if (rootValue)
        {
            json_object_set_value(pParent, pszName, rootValue);
            (void)pfncallback(json_value_get_object(rootValue), pContext);
        }
    }

    return 0;
}

int propertybag_serialize_array(
    PROPERTYBAG_HANDLE           hProperty,
    const char*                  pszName,
    PPROPERTYBAG_OBJECT_CALLBACK pfncallback,
    void*                        pContext)
{
    JSON_Value*  rootValue;
    JSON_Array*  pArray;

    if (hProperty && 
        pszName   &&
        pfncallback)
    {
        rootValue = json_value_init_array();
        if (rootValue)
        {
            int status = 0;
            json_object_set_value((JSON_Object*)hProperty, pszName, rootValue);
            pArray = json_value_get_array(rootValue);

            do
            {
                // new up an object
                rootValue = json_value_init_object();
                if (!rootValue)
                {
                    break;
                }

                status = pfncallback(json_value_get_object(rootValue), pContext);
                if (!status)
                {
                    json_array_append_value(pArray, rootValue);
                }
                else
                {
                    json_value_free(rootValue);
                }

            } while (!status);
        }
    }

	return 0;
}

void propertybag_setnumbervalue(
    PROPERTYBAG_HANDLE   hProperty,
    const char*          pszName,
    double               value)
{
    if (hProperty && pszName)
    {
        json_object_set_number((JSON_Object*)hProperty, pszName, value);
    }
}

void propertybag_setbooleanvalue(
    PROPERTYBAG_HANDLE   hProperty,
    const char*          pszName,
    int               value)
{
    if (hProperty && pszName)
    {
        json_object_set_boolean((JSON_Object*)hProperty, pszName, value);
    }
}

void propertybag_setstringvalue(
    PROPERTYBAG_HANDLE   hProperty,
    const char*          pszName,
    const char*          value)
{
    if (hProperty && pszName)
    {
        if (value)
        {
            json_object_set_string((JSON_Object*)hProperty, pszName, value);
        }
        else
        {
            json_object_set_null((JSON_Object*)hProperty, pszName);
        }
    }
}

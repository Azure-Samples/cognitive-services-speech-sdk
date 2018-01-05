//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// propbag.c: functions for handling property bag.
//

#include "azure_c_shared_utility/gballoc.h"
#include "propbag.h"
#include "parson.h"

int PropertybagInitialize()
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

void PropertybagShutdown()
{
}

int PropertybagDeserializeJson(const char* sourceString, size_t size, PPROPERTYBAG_OBJECT_CALLBACK callback, void* context)
{
    JSON_Value * val;

    (void)size;

    PropertybagInitialize();

    if (NULL == sourceString ||
        NULL == callback)
    {
        return -1;
    }

    val = json_parse_string(sourceString);
    if (!val)
    {
        return -1;
    }

    callback(
        (PROPERTYBAG_HANDLE)json_value_get_object(val),
        context);

    json_value_free(val);

    return 0;
}

int PropertybagEnumarray(PROPERTYBAG_HANDLE propertyHandle, const char* name, PPROPERTYBAG_OBJECT_CALLBACK callback, void* context)
{
    if (NULL == propertyHandle ||
        NULL == callback ||
        NULL == name)
    {
        return -1;
    }

    JSON_Array *array = json_object_get_array((JSON_Object*)propertyHandle, name);
    if (array)
    {
        size_t count = json_array_get_count(array);
        while (count--)
        {
            callback(
                (PROPERTYBAG_HANDLE)json_array_get_object(array, count), 
                context);
        }
    }

    return 0;
}

int PropertybagGetChildValue(PROPERTYBAG_HANDLE propertyHandle, const char* name, PPROPERTYBAG_OBJECT_CALLBACK callback, void* context)
{
    if (NULL == callback)
    {
        return -1;
    }

    PROPERTYBAG_HANDLE prop = PropertybagGetChildValue2(propertyHandle, name);
    if (!prop)
    {
        return -1;
    }

    callback(prop, context);

    return 0;
}

PROPERTYBAG_HANDLE PropertybagGetChildValue2(PROPERTYBAG_HANDLE propertyHandle, const char* name)
{
    if (NULL == propertyHandle ||
        NULL == name)
    {
        return NULL;
    }

    const JSON_Value *val = json_object_dotget_value((JSON_Object*)propertyHandle, name);
    if (!val)
    {
        return NULL;
    }

    return (PROPERTYBAG_HANDLE)json_value_get_object(val);
}

const char* PropertybagGetStringValue(PROPERTYBAG_HANDLE propertyHandle, const char* name)
{
    if (NULL == propertyHandle ||
        NULL == name)
    {
        return NULL;
    }

    return json_object_get_string((JSON_Object*)propertyHandle, name);
}

double PropertybagGetNumberValue(PROPERTYBAG_HANDLE propertyHandle, const char* name)
{
    if (NULL == propertyHandle ||
        NULL == name)
    {
        return 0.0f;
    }

    return json_object_get_number((JSON_Object*)propertyHandle, name);
}

unsigned int PropertybagGetBooleanValue(PROPERTYBAG_HANDLE propertyHandle, const char* name)
{
    if (NULL == propertyHandle ||
        NULL == name)
    {
        return 0;
    }

    return json_object_get_boolean((JSON_Object*)propertyHandle, name);
}

STRING_HANDLE PropertybagSerialize(PPROPERTYBAG_OBJECT_CALLBACK callback, void* context)
{
    STRING_HANDLE ret = NULL;
    JSON_Value *rootValue;
    char *text;

    PropertybagInitialize();

    if (callback)
    {
        rootValue = json_value_init_object();
        if (rootValue)
        {
            (void)callback(json_value_get_object(rootValue), context);

            text = json_serialize_to_string(rootValue);
            ret = STRING_construct(text);
            json_free_serialized_string(text);
            json_value_free(rootValue);
        }
    }

    return ret;
}

int PropertybagSerializeObject(PROPERTYBAG_HANDLE propertyHandle, const char* name, PPROPERTYBAG_OBJECT_CALLBACK callback, void* context)
{
    JSON_Object* parent = (JSON_Object*)propertyHandle;
    JSON_Value*  rootValue;

    if (propertyHandle &&
        name   &&
        callback)
    {
        rootValue = json_value_init_object();
        if (rootValue)
        {
            json_object_set_value(parent, name, rootValue);
            (void)callback(json_value_get_object(rootValue), context);
        }
    }

    return 0;
}

int PropertybagSerializeArray(PROPERTYBAG_HANDLE propertyHandle, const char* name, PPROPERTYBAG_OBJECT_CALLBACK callback, void* context)
{
    JSON_Value*  rootValue;
    JSON_Array*  array;

    if (propertyHandle && 
        name   &&
        callback)
    {
        rootValue = json_value_init_array();
        if (rootValue)
        {
            int status = 0;
            json_object_set_value((JSON_Object*)propertyHandle, name, rootValue);
            array = json_value_get_array(rootValue);

            do
            {
                // new up an object
                rootValue = json_value_init_object();
                if (!rootValue)
                {
                    break;
                }

                status = callback(json_value_get_object(rootValue), context);
                if (!status)
                {
                    json_array_append_value(array, rootValue);
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

void PropertybagSetNumberValue(PROPERTYBAG_HANDLE propertyHandle, const char* name, double value)
{
    if (propertyHandle && name)
    {
        json_object_set_number((JSON_Object*)propertyHandle, name, value);
    }
}

void PropertybagSetBooleanValue(PROPERTYBAG_HANDLE propertyHandle, const char* name, int value)
{
    if (propertyHandle && name)
    {
        json_object_set_boolean((JSON_Object*)propertyHandle, name, value);
    }
}

void PropertybagSetStringValue(PROPERTYBAG_HANDLE propertyHandle, const char* name, const char* value)
{
    if (propertyHandle && name)
    {
        if (value)
        {
            json_object_set_string((JSON_Object*)propertyHandle, name, value);
        }
        else
        {
            json_object_set_null((JSON_Object*)propertyHandle, name);
        }
    }
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// propbag.h: defines functions provided by the propbag_parson.
//

#pragma once


#ifdef __cplusplus
extern "C" {
#endif

typedef struct STRING_TAG* STRING_HANDLE;

typedef void* PROPERTYBAG_HANDLE;

/**
* The PPROPERTYBAG_OBJECT_CALLBACK type represents an application-defined
* callback function for handling property object nodes.
* @param propertyHandle The property node handle.
* @param context A pointer to the application-defined callback context.
* @return A return code or zero if successful.  In property bag enumeration calls,
* any non-zero value will termination the enumeration.
*/
typedef int(*PPROPERTYBAG_OBJECT_CALLBACK)(PROPERTYBAG_HANDLE propertyHandle, void* context);

/**
* Initializes the propertybag adapter.
* @return A return code or zero if successful.
*/
int PropertybagInitialize();

/**
* Shutsdown the propertybag adapter.
*/
void PropertybagShutdown();

/**
* Deserializes a JSON data string.
* @param sourceString The JSON string to deserialize.
* @param size The size of pszJSON
* @param callback A callback to handle the root object.
* @param context A pointer to the application-defined callback context.
* @return A return code or zero if successful.
*/
int PropertybagDeserializeJson(const char* sourceString, size_t size, PPROPERTYBAG_OBJECT_CALLBACK  callback, void* context);

/**
* Enumerates an array object.
* @param propertyHandle The parent property node handle.
* @param name The name of the array element.
* @param callback A callback to handle the enumerated object.
* @param context A pointer to the application-defined callback context.
* @return A return code or zero if successful.
*/
int PropertybagEnumarray(PROPERTYBAG_HANDLE propertyHandle, const char* name, PPROPERTYBAG_OBJECT_CALLBACK callback, void* context);

/**
* Retrieves a property object.
* @param propertyHandle The parent property node.
* @param name The name of the object element.
* @param callback A callback to handle the enumerated object.
* @param context A pointer to the application-defined callback context.
* @return A return code or zero if successful.
*/
int PropertybagGetChildValue(PROPERTYBAG_HANDLE propertyHandle, const char* name, PPROPERTYBAG_OBJECT_CALLBACK callback, void* context);

/**
* Returns a property object.
* @param propertyHandle The parent property node.
* @param name The name of the object element.
* @return The child property node or NULL on error.
*/
PROPERTYBAG_HANDLE PropertybagGetChildValue2(PROPERTYBAG_HANDLE propertyHandle, const char* name);

/**
* Retrieves a string value of an object.
* @param propertyHandle The parent property node.
* @param name The name of the object element.
* @return The string value of the property element or NULL if not found.
*/
const char* PropertybagGetStringValue(PROPERTYBAG_HANDLE propertyHandle, const char* name);

/**
* Retrieves a numerical value of an object.
* @param propertyHandle The parent property node.
* @param name The name of the object element.
* @return The numerical value of the property element or NAN if not found.
*/
double PropertybagGetNumberValue(PROPERTYBAG_HANDLE propertyHandle, const char* name);

/**
* Retrieves a numerical value of an object.
* @param propertyHandle The parent property node.
* @param name The name of the object element.
* @return The boolean value of the property where 1 is True and 0 is False
*/
unsigned int PropertybagGetBooleanValue(PROPERTYBAG_HANDLE propertyHandle, const char* name);

/**
* Returns a serialize JSON data string.
* @param callback A callback to handle the enumerated object.
* @param context A pointer to the application-defined callback context.
* @return A handle to the string object.
*/
STRING_HANDLE PropertybagSerialize(PPROPERTYBAG_OBJECT_CALLBACK callback, void* context);

/**
* Serializes an object.
* @param propertyHandle The parent property node.
* @param name The name of the object element.
* @param callback A callback to handle the enumerated object.
* @param context A pointer to the application-defined callback context.
* @return A return code or zero if successful.
*/
int PropertybagSerializeObject(PROPERTYBAG_HANDLE propertyHandle, const char* name, PPROPERTYBAG_OBJECT_CALLBACK callback, void* context);

/**
* Serializes an array.
* @param propertyHandle The parent property node handle.
* @param name The name of the array element.
* @param callback A callback to handle the enumerated object.
* @param context A pointer to the application-defined callback context.
* @return A return code or zero if successful.
*/
int PropertybagSerializeArray(PROPERTYBAG_HANDLE propertyHandle, const char* name, PPROPERTYBAG_OBJECT_CALLBACK callback, void* context);

/**
* Serializes a number value.
* @param propertyHandle The parent property node.
* @param name The name of the object element.
* @param value The value to set.
*/
void PropertybagSetNumberValue(PROPERTYBAG_HANDLE propertyHandle, const char* name, double value);

/**
* Serializes a boolean value.
* @param propertyHandle The parent property node.
* @param name The name of the object element.
* @param value The value to set.
*/
void PropertybagSetBooleanValue(PROPERTYBAG_HANDLE propertyHandle, const char* name, int value);

/**
* Serializes a string value.
* @param propertyHandle The parent property node.
* @param name The name of the object element.
* @param value The value to set.
*/
void PropertybagSetStringValue(PROPERTYBAG_HANDLE propertyHandle, const char* name, const char* value);

#ifdef __cplusplus
}
#endif

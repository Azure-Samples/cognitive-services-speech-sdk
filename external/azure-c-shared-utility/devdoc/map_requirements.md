map requirements
================
 
##Overview

Map is a module that implements a dictionary of STRING_HANDLE key to STRING_HANDLE values.

##References

[strings_requiremens.md]
[json.org](http://www.json.org)

##Exposed API

```c
#define MAP_RESULT_VALUES \
    MAP_OK, \
    MAP_ERROR, \
    MAP_INVALIDARG, \
    MAP_KEYEXISTS, \
    MAP_KEYNOTFOUND, \
    MAP_FILTER_REJECT 
 
DEFINE_ENUM(MAP_RESULT, MAP_RESULT_VALUES);
 
typedef void* MAP_HANDLE;

typedef int (*MAP_FILTER_CALLBACK)(const char* mapProperty, const char* mapValue);

 
extern MAP_HANDLE Map_Create(MAP_FILTER_CALLBACK mapFilterFunc);
extern void Map_Destroy(MAP_HANDLE handle);
extern MAP_HANDLE Map_Clone(MAP_HANDLE handle);
 
extern MAP_RESULT Map_Add(MAP_HANDLE handle, const char* key, const char* value);
extern MAP_RESULT Map_AddOrUpdate(MAP_HANDLE handle, const char* key, const char* value);
extern MAP_RESULT Map_Delete(MAP_HANDLE handle, const char* key);
 
extern MAP_RESULT Map_ContainsKey(MAP_HANDLE handle, const char* key, bool* keyExists);
extern MAP_RESULT Map_ContainsValue(MAP_HANDLE handle, const char* value, bool* valueExists);
extern STRING_HANDLE Map_GetValueFromKey(MAP_HANDLE handle, const char* key);
 
extern MAP_RESULT Map_GetInternals(MAP_HANDLE handle, const char*const** keys, const char*const** values, size_t* count);
extern STRING_HANDLE Map_ToJSON(MAP_HANDLE handle);
```

###Map_Create
```c
extern MAP_HANDLE Map_Create(MAP_FILTER_CALLBACK mapFilterFunc);
```

**SRS_MAP_02_001: [**Map_Create shall create a new, empty map.**]**
**SRS_MAP_02_002: [**If during creation there are any error, then Map_Create shall return NULL.**]**
**SRS_MAP_02_003: [**Otherwise, it shall return a non-NULL handle that can be used in subsequent calls.**]**
 
###Map_Destroy
```c
extern void Map_Destroy(MAP_HANDLE handle);
``` 
**SRS_MAP_02_004: [**Map_Destroy shall release all resources associated with the map.**]**
**SRS_MAP_02_005: [**If parameter handle is NULL then Map_Destroy shall take no action.**]**
 
###Map_Clone
```c
extern MAP_HANDLE Map_Clone(MAP_HANDLE handle);
```
**SRS_MAP_02_038: [**Map_Clone returns NULL if parameter handle is NULL.**]**
**SRS_MAP_02_039: [**Map_Clone shall make a copy of the map indicated by parameter handle and return a non-NULL handle to it.**]**
**SRS_MAP_02_047: [**If during cloning, any operation fails, then Map_Clone shall return NULL.**]** 

###Map_Add
```c
extern MAP_RESULT Map_Add(MAP_HANDLE handle, const char* key, const char* value);
```

Map_Add adds to the map a pair of key, value.
**SRS_MAP_02_006: [**If parameter handle is NULL then Map_Add shall return MAP_INVALID_ARG.**]**
**SRS_MAP_02_007: [**If parameter key is NULL then Map_Add shall return MAP_INVALID_ARG.**]**
**SRS_MAP_02_008: [**If parameter value is NULL then Map_Add shall return MAP_INVALID_ARG.**]** 
**SRS_MAP_02_009: [**If the key already exists, then Map_Add shall return MAP_KEYEXISTS.**]**
**SRS_MAP_02_010: [**Otherwise, Map_Add shall add the pair <key,value> to the map.**]**
**SRS_MAP_02_011: [**If adding the pair <key,value> fails then Map_Add shall return MAP_ERROR.**]**
**SRS_MAP_02_012: [**Otherwise, Map_Add shall return MAP_OK.**]**
**SRS_MAP_07_009: [**If the mapFilterCallback function is not NULL, then the return value will be checked and if it is not zero then Map_Add shall return MAP_FILTER_REJECT.**]** 

###Map_AddOrUpdate
```c
extern MAP_RESULT Map_AddOrUpdate(MAP_HANDLE, const char* key, const char* value);
```

**SRS_MAP_02_013: [**If parameter handle is NULL then Map_AddOrUpdate shall return MAP_INVALID_ARG.**]**
**SRS_MAP_02_014: [**If parameter key is NULL then Map_AddOrUpdate shall return MAP_INVALID_ARG.**]**
**SRS_MAP_02_015: [**If parameter value is NULL then Map_AddOrUpdate shall return MAP_INVALID_ARG.**]** 
**SRS_MAP_02_016: [**If the key already exists, then Map_AddOrUpdate shall overwrite the value of the existing key with parameter value.**]**
**SRS_MAP_02_017: [**Otherwise, Map_AddOrUpdate shall add the pair <key,value> to the map.**]**
**SRS_MAP_02_018: [**If there are any failures then Map_AddOrUpdate shall return MAP_ERROR.**]**
**SRS_MAP_02_019: [**Otherwise, Map_AddOrUpdate shall return MAP_OK.**]**
**SRS_MAP_07_008: [**If the mapFilterCallback function is not NULL, then the return value will be check and if it is not zero then Map_AddOrUpdate shall return MAP_FILTER_REJECT.**]**
 
###Map_Delete
```c
extern MAP_RESULT Map_Delete(MAP_HANDLE handle, const char* key);
```
Map_Delete removes a key and its associated value from the map.
**SRS_MAP_02_020: [**If parameter handle is NULL then Map_Delete shall return MAP_INVALIDARG.**]**
**SRS_MAP_02_021: [**If parameter key is NULL then Map_Delete shall return MAP_INVALIDARG.**]**
**SRS_MAP_02_022: [**If key does not exist then Map_Delete shall return MAP_KEYNOTFOUND.**]**
**SRS_MAP_02_023: [**Otherwise, Map_Delete shall remove the key and its associated value from the map and return MAP_OK.**]**
 
###Map_ContainsKey
```c
extern MAP_RESULT Map_ContainsKey(MAP_HANDLE handle, const char* key, bool* keyExists);
```
Map_ContainsKey returns in parameter keyExists if the map contains a key with the same value as parameter key.
**SRS_MAP_02_024: [**If parameter handle, key or keyExists are NULL then Map_ContainsKey shall return MAP_INVALIDARG.**]**
**SRS_MAP_02_025: [**Otherwise if a key exists then Map_ContainsKey shall return MAP_OK and shall write in keyExists "true".**]**
**SRS_MAP_02_026: [**If a key doesn't exist, then Map_ContainsKey shall return MAP_OK and write in keyExists "false".**]**
 
###Map_ContainsValue
```c
extern MAP_RESULT Map_ContainsValue(MAP_HANDLE handle, const char* value, bool* valueExists);
```
Map_ContainsKey returns in valueExists if at least one pair <key,value> in the map contains the value parameter.
**SRS_MAP_02_027: [**If parameter handle, value or valueExists is NULL then Map_ContainsValue shall return MAP_INVALIDARG.**]** 
**SRS_MAP_02_028: [**Otherwise, if a pair <key, value> has its value equal to the parameter value, the Map_ContainsValue shall return MAP_OK and shall write in valueExists "true".**]**
**SRS_MAP_02_029: [**Otherwise, if such a <key, value> does not exist, then Map_ContainsValue shall return MAP_OK and shall write in valueExists "false".**]** 

###Map_GetValueFromKey
```c
extern const char* Map_GetValueFromKey(MAP_HANDLE handle, const char* key);
```

Map_GetValueFromKey returns the value of a stored key.
**SRS_MAP_02_040: [**If parameter handle or key is NULL then Map_GetValueFromKey returns NULL.**]**
**SRS_MAP_02_041: [**If the key is not found, then Map_GetValueFromKey returns NULL.**]**
**SRS_MAP_02_042: [**Otherwise, Map_GetValueFromKey returns the key's value.**]** 

###Map_GetInternals
```c
extern MAP_RESULT Map_GetInternals(MAP_HANDLE handle, const char*const** keys, const char*const** values, size_t* count);
```
**SRS_MAP_02_046: [**If parameter handle, keys, values or count is NULL then Map_GetInternals shall return MAP_INVALIDARG.**]** 
**SRS_MAP_02_043: [**Map_GetInternals shall produce in *keys an pointer to an array of const char* having all the keys stored so far by the map.**]**
**SRS_MAP_02_044: [**Map_GetInternals shall produce in *values a pointer to an array of const char* having all the values stored so far by the map.**]**
**SRS_MAP_02_045: [**  Map_GetInternals shall produce in *count the number of stored keys and values.**]** 

###Map_ToJSON
```c
extern STRING_HANDLE Map_ToJSON(MAP_HANDLE handle);
```
**SRS_MAP_02_052: [**If parameter handle is NULL then Map_ToJSON shall return NULL.**]** 
**SRS_MAP_02_048: [**Map_ToJSON shall produce a STRING_HANDLE representing the content of the MAP.**]**
**SRS_MAP_02_049: [**If the MAP is empty, then Map_ToJSON shall produce the string "{}".**]**
**SRS_MAP_02_050: [**If the map has properties then Map_ToJSON shall produce the following string:{"name1":"value1", "name2":"value2" ...}**]** 
**SRS_MAP_02_051: [**If any error occurs while producing the output, then Map_ToJSON shall fail and return NULL.**]** 

ConstMap Requirements
================


 
## Overview

Const Map is a module that implements a read-only dictionary of `const char*` key to `const char*` values.  It is intially populated by a Map.

## References
[refcount](../inc/refcount.h)

[Map](map_requirements.md)

## Exposed API
```C
#define CONSTMAP_RESULT_VALUES \
    CONSTMAP_OK, \
    CONSTMAP_ERROR, \
    CONSTMAP_INVALIDARG, \
    CONSTMAP_KEYNOTFOUND
 
DEFINE_ENUM(CONSTMAP_RESULT, CONSTMAP_RESULT_VALUES);
 
typedef struct CONSTMAP_HANDLE_DATA_TAG* CONSTMAP_HANDLE;
 
extern CONSTMAP_HANDLE CONSTMAP_Create(MAP_HANDLE sourceMap);
extern void ConstMap_Destroy(CONSTMAP_HANDLE handle);
extern CONSTMAP_HANDLE ConstMap_Clone(CONSTMAP_HANDLE handle);
extern MAP_HANDLE ConstMap_CloneWriteable(CONSTMAP_HANDLE handle);

  
extern bool ConstMap_ContainsKey(CONSTMAP_HANDLE handle, const char* key);
extern bool ConstMap_ContainsValue(CONSTMAP_HANDLE handle, const char* value);
extern const char* ConstMap_GetValue(CONSTMAP_HANDLE handle, const char* key);
 
extern CONSTMAP_RESULT ConstMap_GetInternals(CONSTMAP_HANDLE handle, const char*const** keys, const char*const** values, size_t* count);
```


### ConstMap_Create
```C
extern CONSTMAP_HANDLE ConstMap_Create(MAP_HANDLE sourceMap);
```
**SRS_CONSTMAP_17_001: [**`ConstMap_Create` shall create an immutable map, populated by the key, value pairs in the source map.**]** 
**SRS_CONSTMAP_17_048: [**`ConstMap_Create` shall accept any non-`NULL` `MAP_HANDLE` as input.**]** 
**SRS_CONSTMAP_17_002: [**If during creation there are any errors, then `ConstMap_Create` shall return `NULL`.**]** 
**SRS_CONSTMAP_17_003: [**Otherwise, it shall return a non-`NULL` handle that can be used in subsequent calls.**]**

### ConstMap_Destroy
```C
extern void ConstMap_Destroy(CONSTMAP_HANDLE handle);
``` 
**SRS_CONSTMAP_17_005: [**If parameter handle is `NULL` then `ConstMap_Destroy` shall take no action.**]**
**SRS_CONSTMAP_17_049: [**`ConstMap_Destroy` shall decrement the internal reference count of the immutable map.**]**
**SRS_CONSTMAP_17_004: [**If the reference count is zero, `ConstMap_Destroy` shall release all resources associated with the immutable map.**]** 


### ConstMap_Clone
```C
extern CONSTMAP_HANDLE ConstMap_Clone(CONSTMAP_HANDLE handle);
```
**SRS_CONSTMAP_17_038: [**`ConstMap_Clone` returns `NULL` if parameter `handle` is `NULL`.**]** 
**SRS_CONSTMAP_17_039: [**`ConstMap_Clone` shall increase the internal reference count of the immutable map indicated by parameter `handle`**]**
**SRS_CONSTMAP_17_050: [**`ConstMap_Clone` shall  return the non-`NULL` handle. **]**

### ConstMap_CloneWriteable
```C
extern MAP_HANDLE ConstMap_CloneWriteable(CONSTMAP_HANDLE handle);
```

This function will return a new MAP_HANDLE populated by the key, value pairs contained in the CONSTMAP_HANDLE.  This MAP_HANDLE needs to be destroyed when it is no longer needed. 

**SRS_CONSTMAP_17_051: [**`ConstMap_CloneWriteable` returns `NULL` if parameter handle is `NULL`. **]**
**SRS_CONSTMAP_17_052: [**`ConstMap_CloneWriteable` shall create a new, writeable map, populated by the key, value pairs in the parameter defined by `handle`.**]**
**SRS_CONSTMAP_17_053: [**If during copying, any operation fails, then `ConstMap_CloneWriteableap_Clone` shall return `NULL`.**]**
**SRS_CONSTMAP_17_054: [**Otherwise, `ConstMap_CloneWriteable` shall return a non-`NULL` handle that can be used in subsequent calls.**]**


### ConstMap_ContainsKey
```C
extern bool ConstMap_ContainsKey(CONSTMAP_HANDLE handle, const char* key);
```
`ConstMap_ContainsKey` returns `true` if the map contains a key with the same value as parameter `key`.

**SRS_CONSTMAP_17_024: [**If parameter `handle` or `key` are `NULL` then `ConstMap_ContainsKey` shall return `false`.**]** 
**SRS_CONSTMAP_17_025: [**Otherwise if a key exists then `ConstMap_ContainsKey` shall return `true`.**]** 
**SRS_CONSTMAP_17_026: [**If a key doesn't exist, then `ConstMap_ContainsKey` shall return `false`.**]**

### ConstMap_ContainsValue
```C
extern bool ConstMap_ContainsValue(CONSTMAP_HANDLE handle, const char* value);
```

`ConstMap_ContainsKey` returns `true` if at least one pair <key,value> in the map contains the `value` parameter.

**SRS_CONSTMAP_17_027: [**If parameter `handle` or `value` is `NULL` then `ConstMap_ContainsValue` shall return `false`.**]** 
**SRS_CONSTMAP_17_028: [**Otherwise, if a pair <key, value> has its value equal to the parameter `value`, the `ConstMap_ContainsValue` shall return `true`.**]** 
**SRS_CONSTMAP_17_029: [**Otherwise, if such a <key, value> does not exist, then `ConstMap_ContainsValue` shall return `false`.**]**

### ConstMap_GetValue
```C
extern const char* ConstMap_GetValue(CONSTMAP_HANDLE handle, const char* key);
```
`ConstMap_GetValue` returns the value of a stored key.

**SRS_CONSTMAP_17_040: [**If parameter `handle` or `key` is `NULL` then `ConstMap_GetValue` returns `NULL`.**]** 
**SRS_CONSTMAP_17_041: [**If the key is not found, then `ConstMap_GetValue` returns `NULL`.**]** 
**SRS_CONSTMAP_17_042: [**Otherwise, `ConstMap_GetValue` returns the key's value.**]**

### ConstMap_GetInternals
```C
extern CONSTMAP_RESULT ConstMap_GetInternals(CONSTMAP_HANDLE handle, const char*const** keys, const char*const** values, size_t* count);
```
**SRS_CONSTMAP_17_046: [**If parameter `handle`, `keys`, `values` or `count` is `NULL` then `ConstMap_GetInternals` shall return `CONSTMAP_INVALIDARG`.**]** 
**SRS_CONSTMAP_17_043: [**`ConstMap_GetInternals` shall produce in `*keys` a pointer to an array of `const char*` having all the keys stored so far by the map.**]** 
**SRS_CONSTMAP_17_044: [**`ConstMap_GetInternals` shall produce in `*values` a pointer to an array of `const char*` having all the values stored so far by the map.**]** 
**SRS_CONSTMAP_17_045: [** `ConstMap_GetInternals` shall produce in `*count` the number of stored keys and values.**]**

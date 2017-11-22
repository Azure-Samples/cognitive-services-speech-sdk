#umocktypes requirements
 
#Overview

umocktypes is a module that exposes a generic type interface to be used by umockc for registering various C types. Later operations are possible with these types, specifically: converting any type value to a string, comparing values, copying and freeing values.

#Exposed API

```c
typedef char*(*UMOCKTYPE_STRINGIFY_FUNC)(const void* value);
typedef int(*UMOCKTYPE_COPY_FUNC)(void* destination, const void* source);
typedef void(*UMOCKTYPE_FREE_FUNC)(void* value);
typedef int(*UMOCKTYPE_ARE_EQUAL_FUNC)(const void* left, const void* right);

extern int umocktypes_init(void);
extern void umocktypes_deinit(void);
extern int umocktypes_register_type(const char* type, UMOCKTYPE_STRINGIFY_FUNC stringify_func, UMOCKTYPE_ARE_EQUAL_FUNC are_equal_func, UMOCKTYPE_COPY_FUNC copy_func, UMOCKTYPE_FREE_FUNC free_func);
extern int umocktypes_register_alias_type(const char* type, const char* is_type);

extern char* umocktypes_stringify(const char* type, const void* value);
extern int umocktypes_are_equal(const char* type, const void* left, const void* right);
extern int umocktypes_copy(const char* type, void* destination, const void* source);
extern void umocktypes_free(const char* type, void* value);
```

##umocktypes_init

```c
extern int umocktypes_init(void);
```

**SRS_UMOCKTYPES_01_001: [** umocktypes_init shall initialize the umocktypes module. **]**
**SRS_UMOCKTYPES_01_002: [** After initialization the list of registered type shall be empty. **]**
**SRS_UMOCKTYPES_01_003: [** On success umocktypes_init shall return 0. **]**
**SRS_UMOCKTYPES_01_004: [** umocktypes_init after another umocktypes_init without deinitializing the module shall fail and return a non-zero value. **]**

##umocktypes_deinit

```c
extern void umocktypes_deinit(void);
```

**SRS_UMOCKTYPES_01_005: [** umocktypes_deinit shall free all resources associated with the registered types and shall leave the module in a state where another init is possible. **]**
**SRS_UMOCKTYPES_01_006: [** If the module was not initialized, umocktypes_deinit shall do nothing. **]**
**SRS_UMOCKTYPES_01_040: [** An umocktypes_init call after deinit shall succeed provided all underlying calls succeed. **]**

##umocktypes_register_type

```c
extern int umocktypes_register_type(const char* type, UMOCKTYPE_STRINGIFY_FUNC stringify_func, UMOCKTYPE_ARE_EQUAL_FUNC are_equal_func, UMOCKTYPE_COPY_FUNC copy_func, UMOCKTYPE_FREE_FUNC free_func);
```

**SRS_UMOCKTYPES_01_007: [** umocktypes_register_type shall register an interface made out of the stringify, are equal, copy and free functions for the type identified by the argument type. **]**
**SRS_UMOCKTYPES_01_008: [** On success umocktypes_register_type shall return 0. **]**
**SRS_UMOCKTYPES_01_009: [** If any of the arguments is NULL, umocktypes_register_type shall fail and return a non-zero value. **]**
**SRS_UMOCKTYPES_01_010: [** If the type has already been registered with the same function pointers then umocktypes_register_type shall succeed and return 0. **]**
**SRS_UMOCKTYPES_01_011: [** If the type has already been registered but at least one of the function pointers is different, umocktypes_register_type shall fail and return a non-zero value. **]**
**SRS_UMOCKTYPES_01_012: [** If an error occurs allocating memory for the newly registered type, umocktypes_register_type shall fail and return a non-zero value. **]**
**SRS_UMOCKTYPES_01_034: [** Before registering, the type string shall be normalized by calling umocktypename_normalize. **]**
**SRS_UMOCKTYPES_01_045: [** If normalizing the typename fails, umocktypes_register_type shall fail and return a non-zero value. **]**
**SRS_UMOCKTYPES_01_050: [** If umocktypes_register_type is called when the module is not initialized, umocktypes_register_type shall fail and return a non zero value. **]**

##umocktypes_register_alias_type

```c
int umocktypes_register_alias_type(const char* type, const char* is_type)
```

**SRS_UMOCKTYPES_01_053: [** umocktypes_register_alias_type shall register a new alias type for the type "is_type". **]**
**SRS_UMOCKTYPES_01_054: [** On success, umocktypes_register_alias_type shall return 0. **]**
**SRS_UMOCKTYPES_01_055: [** If any of the arguments is NULL, umocktypes_register_alias_type shall fail and return a non-zero value. **]**
**SRS_UMOCKTYPES_01_057: [** If is_type was not already registered, umocktypes_register_alias_type shall fail and return a non-zero value. **]**
**SRS_UMOCKTYPES_01_058: [** Before looking it up, is_type shall be normalized by using umocktypename_normalize. **]**
**SRS_UMOCKTYPES_01_059: [** Before adding it as alias, type shall be normalized by using umocktypename_normalize. **]**
**SRS_UMOCKTYPES_01_060: [** If umocktypename_normalize fails, umocktypes_register_alias_type shall fail and return a non-zero value. **]**
**SRS_UMOCKTYPES_01_061: [** If umocktypes_register_alias_type is called when the module is not initialized, umocktypes_register_type shall fail and return a non zero value. **]**
**SRS_UMOCKTYPES_01_062: [** If type and is_type are the same, umocktypes_register_alias_type shall succeed and return 0. **]**

##umocktypes_stringify
 
```c
extern char* umocktypes_stringify(const char* type, const void* value);
```

**SRS_UMOCKTYPES_01_013: [** umocktypes_stringify shall return a char\* with the string representation of the value argument. **]**
**SRS_UMOCKTYPES_01_014: [** The string representation shall be obtained by calling the stringify function registered for the type identified by the argument type. **]**
**SRS_UMOCKTYPES_01_015: [** On success umocktypes_stringify shall return the char\* produced by the underlying stringify function for type (passed in umocktypes_register_type). **]**
**SRS_UMOCKTYPES_01_016: [** If any of the arguments is NULL, umocktypes_stringify shall fail and return NULL. **]**
**SRS_UMOCKTYPES_01_017: [** If type can not be found in the registered types list maintained by the module, umocktypes_stringify shall fail and return NULL. **]**
**SRS_UMOCKTYPES_01_035: [** Before looking it up, the type string shall be normalized by calling umocktypename_normalize. **]**
**SRS_UMOCKTYPES_01_044: [** If normalizing the typename fails, umocktypes_stringify shall fail and return NULL. **]**
**SRS_UMOCKTYPES_01_049: [** If umocktypes_stringify is called when the module is not initialized, umocktypes_stringify shall return NULL. **]**
**SRS_UMOCKTYPES_01_063: [** If type is a pointer type and type was not registered then umocktypes_stringify shall execute as if type is void*. **]**

##umocktypes_are_equal

```c
extern int umocktypes_are_equal(const char* type, const void* left, const void* right);
```

**SRS_UMOCKTYPES_01_018: [** umocktypes_are_equal shall evaluate whether 2 values are equal. **]**
**SRS_UMOCKTYPES_01_019: [** umocktypes_are_equal shall call the underlying are_equal function for the type identified by the argument type (passed in umocktypes_register_type). **]**
**SRS_UMOCKTYPES_01_020: [** If the underlying are_equal function fails,, umocktypes_are_equal shall fail and return -1. **]** 
**SRS_UMOCKTYPES_01_021: [** If the underlying are_equal function indicates the types are equal, umocktypes_are_equal shall return 1. **]**
**SRS_UMOCKTYPES_01_022: [** If the underlying are_equal function indicates the types are not equal, umocktypes_are_equal shall return 0. **]** 
**SRS_UMOCKTYPES_01_023: [** If any of the arguments is NULL, umocktypes_are_equal shall fail and return -1. **]**
**SRS_UMOCKTYPES_01_024: [** If type can not be found in the registered types list maintained by the module, umocktypes_are_equal shall fail and return -1. **]**
**SRS_UMOCKTYPES_01_036: [** Before looking it up, the type string shall be normalized by calling umocktypename_normalize. **]**
**SRS_UMOCKTYPES_01_043: [** If normalizing the typename fails, umocktypes_are_equal shall fail and return -1. **]**
**SRS_UMOCKTYPES_01_046: [** If umocktypes_are_equal is called when the module is not initialized, umocktypes_are_equal shall return -1. **]**
**SRS_UMOCKTYPES_01_051: [** If the pointer values for left and right are equal, umocktypes_are_equal shall return 1 without calling the underlying are_equal function. **]** 
**SRS_UMOCKTYPES_01_064: [** If type is a pointer type and type was not registered then umocktypes_are_equal shall execute as if type is void*. **]**

##umocktypes_copy

```c
extern int umocktypes_copy(const char* type, void* destination, const void* source);
```

**SRS_UMOCKTYPES_01_025: [** umocktypes_copy shall copy the value of the source into the destination argument. **]**
**SRS_UMOCKTYPES_01_026: [** The copy shall be done by calling the underlying copy function (passed in umocktypes_register_type) for the type identified by the type argument. **]**
**SRS_UMOCKTYPES_01_052: [** On success, umocktypes_copy shall return 0. **]**
**SRS_UMOCKTYPES_01_027: [** If any of the arguments is NULL, umocktypes_copy shall return -1. **]**
**SRS_UMOCKTYPES_01_028: [** If the underlying copy fails, umocktypes_copy shall return -1. **]**
**SRS_UMOCKTYPES_01_029: [** If type can not be found in the registered types list maintained by the module, umocktypes_copy shall fail and return -1. **]**
**SRS_UMOCKTYPES_01_037: [** Before looking it up, the type string shall be normalized by calling umocktypename_normalize. **]**
**SRS_UMOCKTYPES_01_042: [** If normalizing the typename fails, umocktypes_copy shall fail and return a non-zero value. **]**
**SRS_UMOCKTYPES_01_047: [** If umocktypes_copy is called when the module is not initialized, umocktypes_copy shall fail and return a non zero value. **]**
**SRS_UMOCKTYPES_01_065: [** If type is a pointer type and type was not registered then umocktypes_copy shall execute as if type is void*. **]**

##umocktypes_free

```c
extern void umocktypes_free(const char* type, void* value);
```

**SRS_UMOCKTYPES_01_030: [** umocktypes_free shall free a value previously allocated with umocktypes_copy. **]**
**SRS_UMOCKTYPES_01_031: [** If any of the arguments is NULL, umocktypes_free shall do nothing. **]**
**SRS_UMOCKTYPES_01_033: [** The free shall be done by calling the underlying free function (passed in umocktypes_register_type) for the type identified by the type argument. **]** 
**SRS_UMOCKTYPES_01_032: [** If type can not be found in the registered types list maintained by the module, umocktypes_free shall do nothing. **]**
**SRS_UMOCKTYPES_01_038: [** Before looking it up, the type string shall be normalized by calling umocktypename_normalize. **]**
**SRS_UMOCKTYPES_01_041: [** If normalizing the typename fails, umocktypes_free shall do nothing. **]**
**SRS_UMOCKTYPES_01_048: [** If umocktypes_free is called when the module is not initialized, umocktypes_free shall do nothing. **]**
**SRS_UMOCKTYPES_01_066: [** If type is a pointer type and type was not registered then umocktypes_free shall execute as if type is void*. **]**
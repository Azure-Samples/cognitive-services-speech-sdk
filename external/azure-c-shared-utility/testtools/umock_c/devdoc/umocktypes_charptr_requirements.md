#umocktypes_charptr requirements
 
#Overview

umocktypes_charptr is a module that exposes out of the box functionality for char\* and const char\* types for umockc.

#Exposed API

```c
extern int umocktypes_charptr_register_types(void);

extern char* umocktypes_stringify_charptr(const char** value);
extern int umocktypes_are_equal_charptr(const char** left, const char** right);
extern int umocktypes_copy_charptr(char** destination, const char** source);
extern void umocktypes_free_charptr(char** value);

extern char* umocktypes_stringify_const_charptr(const char** value);
extern int umocktypes_are_equal_const_charptr(const char** left, const char** right);
extern int umocktypes_copy_const_charptr(const char** destination, const char** source);
extern void umocktypes_free_const_charptr(const char** value);
```

##umocktypes_charptr_register_types

```c
extern int umocktypes_charptr_register_types(void);
```

**SRS_UMOCKTYPES_CHARPTR_01_001: [** umocktypes_charptr_register_types shall register support for the types char\* and const char\* by using the REGISTER_UMOCK_VALUE_TYPE macro provided by umockc. **]**
**SRS_UMOCKTYPES_CHARPTR_01_038: [** On success, umocktypes_charptr_register_types shall return 0. **]**
**SRS_UMOCKTYPES_CHARPTR_01_039: [** If registering any of the types fails, umocktypes_charptr_register_types shall fail and return a non-zero value. **]**

##umocktypes_stringify_charptr

```c
extern char* umocktypes_stringify_charptr(const char** value);
```

**SRS_UMOCKTYPES_CHARPTR_01_002: [** umocktypes_stringify_charptr shall return a string containing the string representation of value, enclosed by quotes ("value"). **]**
**SRS_UMOCKTYPES_CHARPTR_01_004: [** If value is NULL, umocktypes_stringify_charptr shall return NULL. **]**
**SRS_UMOCKTYPES_CHARPTR_01_003: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_charptr shall return NULL. **]**

##umocktypes_are_equal_charptr

```c
extern int umocktypes_are_equal_charptr(const char** left, const char** right);
```

**SRS_UMOCKTYPES_CHARPTR_01_005: [** umocktypes_are_equal_charptr shall compare the 2 strings pointed to by left and right. **]**
**SRS_UMOCKTYPES_CHARPTR_01_006: [** The comparison shall be case sensitive. **]**
**SRS_UMOCKTYPES_CHARPTR_01_007: [** If left and right are equal, umocktypes_are_equal_charptr shall return 1. **]**
**SRS_UMOCKTYPES_CHARPTR_01_008: [** If only one of the left and right argument is NULL, umocktypes_are_equal_charptr shall return 0. **]**
**SRS_UMOCKTYPES_CHARPTR_01_009: [** If the string pointed to by left is equal to the string pointed to by right, umocktypes_are_equal_charptr shall return 1. **]**
**SRS_UMOCKTYPES_CHARPTR_01_010: [** If the string pointed to by left is different than the string pointed to by right, umocktypes_are_equal_charptr shall return 0. **]**

##umocktypes_copy_charptr

```c
extern int umocktypes_copy_charptr(char** destination, const char** source);
```

**SRS_UMOCKTYPES_CHARPTR_01_011: [** umocktypes_copy_charptr shall allocate a new sequence of chars by using umockalloc_malloc. **]**
**SRS_UMOCKTYPES_CHARPTR_01_012: [** The number of bytes allocated shall accomodate the string pointed to by source. **]**
**SRS_UMOCKTYPES_CHARPTR_01_014: [** umocktypes_copy_charptr shall copy the string pointed to by source to the newly allocated memory. **]**
**SRS_UMOCKTYPES_CHARPTR_01_015: [** The newly allocated string shall be returned in the destination argument. **]**
**SRS_UMOCKTYPES_CHARPTR_01_016: [** On success umocktypes_copy_charptr shall return 0. **]**
**SRS_UMOCKTYPES_CHARPTR_01_013: [** If source or destination are NULL, umocktypes_copy_charptr shall return a non-zero value. **]**
**SRS_UMOCKTYPES_CHARPTR_01_036: [** If allocating the memory for the new string fails, umocktypes_copy_charptr shall fail and return a non-zero value. **]**

##umocktypes_free_charptr

```c
extern void umocktypes_free_charptr(char** value);
```

**SRS_UMOCKTYPES_CHARPTR_01_017: [** umocktypes_free_charptr shall free the string pointed to by value. **]**
**SRS_UMOCKTYPES_CHARPTR_01_018: [** If value is NULL, umocktypes_free_charptr shall do nothing. **]**

##umocktypes_stringify_const_charptr

```c
extern char* umocktypes_stringify_const_charptr(const char** value);
```

**SRS_UMOCKTYPES_CHARPTR_01_019: [** umocktypes_stringify_const_charptr shall return a string containing the string representation of value, enclosed by quotes ("value"). **]**
**SRS_UMOCKTYPES_CHARPTR_01_020: [** If value is NULL, umocktypes_stringify_const_charptr shall return NULL. **]**
**SRS_UMOCKTYPES_CHARPTR_01_021: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_const_charptr shall return NULL. **]**

##umocktypes_are_equal_const_charptr

```c
extern int umocktypes_are_equal_const_charptr(const char** left, const char** right);
```

**SRS_UMOCKTYPES_CHARPTR_01_022: [** umocktypes_are_equal_const_charptr shall compare the 2 strings pointed to by left and right. **]**
**SRS_UMOCKTYPES_CHARPTR_01_023: [** The comparison shall be case sensitive. **]**
**SRS_UMOCKTYPES_CHARPTR_01_024: [** If left and right are equal, umocktypes_are_equal_const_charptr shall return 1. **]**
**SRS_UMOCKTYPES_CHARPTR_01_025: [** If only one of the left and right argument is NULL, umocktypes_are_equal_const_charptr shall return 0. **]**
**SRS_UMOCKTYPES_CHARPTR_01_026: [** If the string pointed to by left is equal to the string pointed to by right, umocktypes_are_equal_const_charptr shall return 1. **]**
**SRS_UMOCKTYPES_CHARPTR_01_027: [** If the string pointed to by left is different than the string pointed to by right, umocktypes_are_equal_const_charptr shall return 0. **]**

##umocktypes_copy_const_charptr

```c
extern int umocktypes_copy_const_charptr(const char** destination, const char** source);
```

**SRS_UMOCKTYPES_CHARPTR_01_028: [** umocktypes_copy_const_charptr shall allocate a new sequence of chars by using umockalloc_malloc. **]**
**SRS_UMOCKTYPES_CHARPTR_01_029: [** The number of bytes allocated shall accomodate the string pointed to by source. **]**
**SRS_UMOCKTYPES_CHARPTR_01_030: [** umocktypes_copy_const_charptr shall copy the string pointed to by source to the newly allocated memory. **]**
**SRS_UMOCKTYPES_CHARPTR_01_031: [** The newly allocated string shall be returned in the destination argument. **]**
**SRS_UMOCKTYPES_CHARPTR_01_032: [** On success umocktypes_copy_const_charptr shall return 0. **]**
**SRS_UMOCKTYPES_CHARPTR_01_033: [** If source or destination are NULL, umocktypes_copy_const_charptr shall return a non-zero value. **]**
**SRS_UMOCKTYPES_CHARPTR_01_037: [** If allocating the memory for the new string fails, umocktypes_copy_const_charptr shall fail and return a non-zero value. **]**

##umocktypes_free_const_charptr

```c
extern void umocktypes_free_const_charptr(const char** value);
```

**SRS_UMOCKTYPES_CHARPTR_01_034: [** umocktypes_free_const_charptr shall free the string pointed to by value. **]**
**SRS_UMOCKTYPES_CHARPTR_01_035: [** If value is NULL, umocktypes_free_const_charptr shall do nothing. **]**

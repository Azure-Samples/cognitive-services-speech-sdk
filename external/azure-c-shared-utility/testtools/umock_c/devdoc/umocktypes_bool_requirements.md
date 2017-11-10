#umocktypes_bool requirements
 
#Overview

umocktypes_c is a module that exposes out of the box functionality for bool and _Bool for umock_c.

#Exposed API

```c
    extern int umocktypes_bool_register_types(void);

    extern char* umocktypes_stringify_bool(const bool* value); \
    extern int umocktypes_are_equal_bool(const bool* left, const bool* right); \
    extern int umocktypes_copy_bool(bool* destination, const bool* source); \
    extern void umocktypes_free_bool(bool* value);
```

##umocktypes_bool_register_types

```c
extern int umocktypes_bool_register_types(void);
```

**SRS_UMOCKTYPES_BOOL_01_001: [** umocktypes_bool_register_types shall register support for the C99 bool type. **]**
**SRS_UMOCKTYPES_BOOL_01_014: [** On success, umocktypes_bool_register_types shall return 0. **]**
**SRS_UMOCKTYPES_BOOL_01_015: [** If registering fails, umocktypes_bool_register_types shall fail and return a non-zero value. **]**

##umocktypes_stringify_char

```c
extern char* umocktypes_stringify_bool(const bool* value);
```

**SRS_UMOCKTYPES_BOOL_01_002: [** umocktypes_stringify_bool shall return the string representation of value. **]**
**SRS_UMOCKTYPES_BOOL_01_003: [** If value is NULL, umocktypes_stringify_bool shall return NULL. **]**
**SRS_UMOCKTYPES_BOOL_01_004: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_bool shall return NULL. **]**
**SRS_UMOCKTYPES_BOOL_01_005: [** If any other error occurs when creating the string representation, umocktypes_stringify_bool shall return NULL. **]**

##umocktypes_are_equal_bool

```c
extern int umocktypes_are_equal_char(bool* left, bool* right);
```

**SRS_UMOCKTYPES_BOOL_01_006: [** umocktypes_are_equal_bool shall compare the 2 bools pointed to by left and right. **]**
**SRS_UMOCKTYPES_BOOL_01_007: [** If any of the arguments is NULL, umocktypes_are_equal_bool shall return -1. **]**
**SRS_UMOCKTYPES_BOOL_01_008: [** If the values pointed to by left and right are equal, umocktypes_are_equal_bool shall return 1. **]**
**SRS_UMOCKTYPES_BOOL_01_009: [** If the values pointed to by left and right are different, umocktypes_are_equal_bool shall return 0. **]**

##umocktypes_copy_bool

```c
extern int umocktypes_copy_bool(bool* destination, const bool* source);
```

**SRS_UMOCKTYPES_BOOL_01_010: [** umocktypes_copy_bool shall copy the bool value from source to destination. **]**
**SRS_UMOCKTYPES_BOOL_01_011: [** On success umocktypes_copy_bool shall return 0. **]**
**SRS_UMOCKTYPES_BOOL_01_012: [** If source or destination are NULL, umocktypes_copy_bool shall return a non-zero value. **]**

##umocktypes_free_bool

```c
extern void umocktypes_free_bool(bool* value);
```

**SRS_UMOCKTYPES_BOOL_01_013: [** umocktypes_free_bool shall do nothing. **]**

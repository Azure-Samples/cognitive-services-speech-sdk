#umocktypes_c requirements
 
#Overview

umocktypes_c is a module that exposes out of the box functionality for most C supported types. This includes integers, floats, size_t etc.

#Exposed API

```c
extern int umocktypes_c_register_types(void);

#define UMOCKTYPES_HANDLERS(type, function_postfix) \
    extern char* C2(umocktypes_stringify_,function_postfix)(const type* value); \
    extern int C2(umocktypes_are_equal_, function_postfix)(const type* left, const type* right); \
    extern int C2(umocktypes_copy_, function_postfix)(type* destination, const type* source); \
    extern void C2(umocktypes_free_, function_postfix)(type* value);

UMOCKTYPES_HANDLERS(char, char)
UMOCKTYPES_HANDLERS(unsigned char, unsignedchar)
UMOCKTYPES_HANDLERS(short, short)
UMOCKTYPES_HANDLERS(unsigned short, unsignedshort)
UMOCKTYPES_HANDLERS(int, int)
UMOCKTYPES_HANDLERS(unsigned int, unsignedint)
UMOCKTYPES_HANDLERS(long, long)
UMOCKTYPES_HANDLERS(unsigned long, unsignedlong)
UMOCKTYPES_HANDLERS(long long, longlong)
UMOCKTYPES_HANDLERS(unsigned long long, unsignedlonglong)
UMOCKTYPES_HANDLERS(float, float)
UMOCKTYPES_HANDLERS(double, double)
UMOCKTYPES_HANDLERS(long double, longdouble)
UMOCKTYPES_HANDLERS(size_t, size_t)
UMOCKTYPES_HANDLERS(void*, void_ptr)
UMOCKTYPES_HANDLERS(const void*, void_ptr)

```

##umocktypes_c_register_types

```c
extern int umocktypes_c_register_types(void);
```

**SRS_UMOCKTYPES_C_01_001: [** umocktypes_c_register_types shall register support for all the types in the module. **]**
**SRS_UMOCKTYPES_C_01_170: [** On success, umocktypes_c_register_types shall return 0. **]**
**SRS_UMOCKTYPES_C_01_171: [** If registering any of the types fails, umocktypes_c_register_types shall fail and return a non-zero value. **]**

##umocktypes_stringify_char

```c
extern char* umocktypes_stringify_char(const char* value);
```

**SRS_UMOCKTYPES_C_01_002: [** umocktypes_stringify_char shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_003: [** If value is NULL, umocktypes_stringify_char shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_004: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_char shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_005: [** If any other error occurs when creating the string representation, umocktypes_stringify_char shall return NULL. **]**

##umocktypes_are_equal_char

```c
extern int umocktypes_are_equal_char(const char* left, const char* right);
```

**SRS_UMOCKTYPES_C_01_006: [** umocktypes_are_equal_char shall compare the 2 chars pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_007: [** If any of the arguments is NULL, umocktypes_are_equal_char shall return -1. **]**
**SRS_UMOCKTYPES_C_01_008: [** If the values pointed to by left and right are equal, umocktypes_are_equal_char shall return 1. **]**
**SRS_UMOCKTYPES_C_01_009: [** If the values pointed to by left and right are different, umocktypes_are_equal_char shall return 0. **]**

##umocktypes_copy_char

```c
extern int umocktypes_copy_char(char* destination, const char* source);
```

**SRS_UMOCKTYPES_C_01_010: [** umocktypes_copy_char shall copy the char value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_011: [** On success umocktypes_copy_char shall return 0. **]**
**SRS_UMOCKTYPES_C_01_012: [** If source or destination are NULL, umocktypes_copy_char shall return a non-zero value. **]**

##umocktypes_free_char

```c
extern void umocktypes_free_char(char* value);
```

**SRS_UMOCKTYPES_C_01_013: [** umocktypes_free_char shall do nothing. **]**

##umocktypes_stringify_unsignedchar

```c
extern char* umocktypes_stringify_unsignedchar(const unsigned char* value);
```

**SRS_UMOCKTYPES_C_01_014: [** umocktypes_stringify_unsignedchar shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_015: [** If value is NULL, umocktypes_stringify_unsignedchar shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_016: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedchar shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_017: [** If any other error occurs when creating the string representation, umocktypes_stringify_unsignedchar shall return NULL. **]**

##umocktypes_are_equal_unsignedchar

```c
extern int umocktypes_are_equal_unsignedchar(const unsigned char* left, const unsigned char* right);
```

**SRS_UMOCKTYPES_C_01_018: [** umocktypes_are_equal_unsignedchar shall compare the 2 unsigned chars pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_019: [** If any of the arguments is NULL, umocktypes_are_equal_unsignedchar shall return -1. **]**
**SRS_UMOCKTYPES_C_01_020: [** If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedchar shall return 1. **]**
**SRS_UMOCKTYPES_C_01_021: [** If the values pointed to by left and right are different, umocktypes_are_equal_unsignedchar shall return 0. **]**

##umocktypes_copy_unsignedchar

```c
extern int umocktypes_copy_unsignedchar(unsigned char* destination, const unsigned char* source);
```

**SRS_UMOCKTYPES_C_01_022: [** umocktypes_copy_unsignedchar shall copy the unsigned char value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_023: [** On success umocktypes_copy_unsignedchar shall return 0. **]**
**SRS_UMOCKTYPES_C_01_024: [** If source or destination are NULL, umocktypes_copy_unsignedchar shall return a non-zero value. **]**

##umocktypes_free_unsignedchar

```c
extern void umocktypes_free_unsignedchar(unsigned char* value);
```

**SRS_UMOCKTYPES_C_01_025: [** umocktypes_free_unsignedchar shall do nothing. **]**

##umocktypes_stringify_short

```c
extern char* umocktypes_stringify_short(const short* value);
```

**SRS_UMOCKTYPES_C_01_026: [** umocktypes_stringify_short shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_027: [** If value is NULL, umocktypes_stringify_short shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_028: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_short shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_029: [** If any other error occurs when creating the string representation, umocktypes_stringify_short shall return NULL. **]**

##umocktypes_are_equal_short

```c
extern int umocktypes_are_equal_short(const short* left, const short* right);
```

**SRS_UMOCKTYPES_C_01_030: [** umocktypes_are_equal_short shall compare the 2 shorts pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_031: [** If any of the arguments is NULL, umocktypes_are_equal_short shall return -1. **]**
**SRS_UMOCKTYPES_C_01_032: [** If the values pointed to by left and right are equal, umocktypes_are_equal_short shall return 1. **]**
**SRS_UMOCKTYPES_C_01_033: [** If the values pointed to by left and right are different, umocktypes_are_equal_short shall return 0. **]**

##umocktypes_copy_short

```c
extern int umocktypes_copy_short(short* destination, const short* source);
```

**SRS_UMOCKTYPES_C_01_034: [** umocktypes_copy_short shall copy the short value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_035: [** On success umocktypes_copy_short shall return 0. **]**
**SRS_UMOCKTYPES_C_01_036: [** If source or destination are NULL, umocktypes_copy_short shall return a non-zero value. **]**

##umocktypes_free_short

```c
extern void umocktypes_free_short(short* value);
```

**SRS_UMOCKTYPES_C_01_037: [** umocktypes_free_short shall do nothing. **]**

##umocktypes_stringify_unsignedshort

```c
extern char* umocktypes_stringify_unsignedshort(const unsigned short* value);
```

**SRS_UMOCKTYPES_C_01_038: [** umocktypes_stringify_unsignedshort shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_039: [** If value is NULL, umocktypes_stringify_unsignedshort shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_040: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedshort shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_041: [** If any other error occurs when creating the string representation, umocktypes_stringify_unsignedshort shall return NULL. **]**

##umocktypes_are_equal_unsignedshort

```c
extern int umocktypes_are_equal_unsignedshort(const unsigned short* left, const unsigned short* right);
```

**SRS_UMOCKTYPES_C_01_042: [** umocktypes_are_equal_unsignedshort shall compare the 2 unsigned shorts pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_043: [** If any of the arguments is NULL, umocktypes_are_equal_unsignedshort shall return -1. **]**
**SRS_UMOCKTYPES_C_01_044: [** If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedshort shall return 1. **]**
**SRS_UMOCKTYPES_C_01_045: [** If the values pointed to by left and right are different, umocktypes_are_equal_unsignedshort shall return 0. **]**

##umocktypes_copy_unsignedshort

```c
extern int umocktypes_copy_unsignedshort(unsigned short* destination, const unsigned short* source);
```

**SRS_UMOCKTYPES_C_01_046: [** umocktypes_copy_unsignedshort shall copy the unsigned short value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_047: [** On success umocktypes_copy_unsignedshort shall return 0. **]**
**SRS_UMOCKTYPES_C_01_048: [** If source or destination are NULL, umocktypes_copy_unsignedshort shall return a non-zero value. **]**

##umocktypes_free_unsignedshort

```c
extern void umocktypes_free_unsignedshort(unsigned short* value);
```

**SRS_UMOCKTYPES_C_01_049: [** umocktypes_free_unsignedshort shall do nothing. **]**

##umocktypes_stringify_int

```c
extern char* umocktypes_stringify_int(const int* value);
```

**SRS_UMOCKTYPES_C_01_050: [** umocktypes_stringify_int shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_051: [** If value is NULL, umocktypes_stringify_int shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_052: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_int shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_053: [** If any other error occurs when creating the string representation, umocktypes_stringify_int shall return NULL. **]**

##umocktypes_are_equal_int

```c
extern int umocktypes_are_equal_int(const int* left, const int* right);
```

**SRS_UMOCKTYPES_C_01_054: [** umocktypes_are_equal_int shall compare the 2 ints pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_055: [** If any of the arguments is NULL, umocktypes_are_equal_int shall return -1. **]**
**SRS_UMOCKTYPES_C_01_056: [** If the values pointed to by left and right are equal, umocktypes_are_equal_int shall return 1. **]**
**SRS_UMOCKTYPES_C_01_057: [** If the values pointed to by left and right are different, umocktypes_are_equal_int shall return 0. **]**

##umocktypes_copy_int

```c
extern int umocktypes_copy_int(int* destination, const int* source);
```

**SRS_UMOCKTYPES_C_01_058: [** umocktypes_copy_int shall copy the int value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_059: [** On success umocktypes_copy_int shall return 0. **]**
**SRS_UMOCKTYPES_C_01_060: [** If source or destination are NULL, umocktypes_copy_int shall return a non-zero value. **]**

##umocktypes_free_int

```c
extern void umocktypes_free_int(int* value);
```

**SRS_UMOCKTYPES_C_01_061: [** umocktypes_free_int shall do nothing. **]**

##umocktypes_stringify_unsignedint

```c
extern char* umocktypes_stringify_unsignedint(const unsigned int* value);
```

**SRS_UMOCKTYPES_C_01_062: [** umocktypes_stringify_unsignedint shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_063: [** If value is NULL, umocktypes_stringify_unsignedint shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_064: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedint shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_065: [** If any other error occurs when creating the string representation, umocktypes_stringify_unsignedint shall return NULL. **]**

##umocktypes_are_equal_unsignedint

```c
extern int umocktypes_are_equal_unsignedint(const unsigned int* left, const unsigned int* right);
```

**SRS_UMOCKTYPES_C_01_066: [** umocktypes_are_equal_unsignedint shall compare the 2 unsigned ints pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_067: [** If any of the arguments is NULL, umocktypes_are_equal_unsignedint shall return -1. **]**
**SRS_UMOCKTYPES_C_01_068: [** If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedint shall return 1. **]**
**SRS_UMOCKTYPES_C_01_069: [** If the values pointed to by left and right are different, umocktypes_are_equal_unsignedint shall return 0. **]**

##umocktypes_copy_unsignedint

```c
extern int umocktypes_copy_unsignedint(unsigned int* destination, const unsigned int* source);
```

**SRS_UMOCKTYPES_C_01_070: [** umocktypes_copy_unsignedint shall copy the unsigned int value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_071: [** On success umocktypes_copy_unsignedint shall return 0. **]**
**SRS_UMOCKTYPES_C_01_072: [** If source or destination are NULL, umocktypes_copy_unsignedint shall return a non-zero value. **]**

##umocktypes_free_unsignedint

```c
extern void umocktypes_free_unsignedint(unsigned int* value);
```

**SRS_UMOCKTYPES_C_01_073: [** umocktypes_free_unsignedint shall do nothing. **]**

##umocktypes_stringify_long

```c
extern char* umocktypes_stringify_long(const long* value);
```

**SRS_UMOCKTYPES_C_01_074: [** umocktypes_stringify_long shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_075: [** If value is NULL, umocktypes_stringify_long shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_076: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_long shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_077: [** If any other error occurs when creating the string representation, umocktypes_stringify_long shall return NULL. **]**

##umocktypes_are_equal_long

```c
extern int umocktypes_are_equal_long(const long* left, const long* right);
```

**SRS_UMOCKTYPES_C_01_078: [** umocktypes_are_equal_long shall compare the 2 longs pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_079: [** If any of the arguments is NULL, umocktypes_are_equal_long shall return -1. **]**
**SRS_UMOCKTYPES_C_01_080: [** If the values pointed to by left and right are equal, umocktypes_are_equal_long shall return 1. **]**
**SRS_UMOCKTYPES_C_01_081: [** If the values pointed to by left and right are different, umocktypes_are_equal_long shall return 0. **]**

##umocktypes_copy_long

```c
extern int umocktypes_copy_long(long* destination, const long* source);
```

**SRS_UMOCKTYPES_C_01_082: [** umocktypes_copy_long shall copy the long value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_083: [** On success umocktypes_copy_long shall return 0. **]**
**SRS_UMOCKTYPES_C_01_084: [** If source or destination are NULL, umocktypes_copy_long shall return a non-zero value. **]**

##umocktypes_free_long

```c
extern void umocktypes_free_long(long* value);
```

**SRS_UMOCKTYPES_C_01_085: [** umocktypes_free_long shall do nothing. **]**

##umocktypes_stringify_unsignedlong

```c
extern char* umocktypes_stringify_unsignedlong(const unsigned long* value);
```

**SRS_UMOCKTYPES_C_01_086: [** umocktypes_stringify_unsignedlong shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_087: [** If value is NULL, umocktypes_stringify_unsignedlong shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_088: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedlong shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_089: [** If any other error occurs when creating the string representation, umocktypes_stringify_unsignedlong shall return NULL. **]**

##umocktypes_are_equal_unsignedlong

```c
extern int umocktypes_are_equal_unsignedlong(const unsigned long* left, const unsigned long* right);
```

**SRS_UMOCKTYPES_C_01_090: [** umocktypes_are_equal_unsignedlong shall compare the 2 unsigned longs pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_091: [** If any of the arguments is NULL, umocktypes_are_equal_unsignedlong shall return -1. **]**
**SRS_UMOCKTYPES_C_01_092: [** If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedlong shall return 1. **]**
**SRS_UMOCKTYPES_C_01_093: [** If the values pointed to by left and right are different, umocktypes_are_equal_unsignedlong shall return 0. **]**

##umocktypes_copy_unsignedlong

```c
extern int umocktypes_copy_unsignedlong(unsigned long* destination, const unsigned long* source);
```

**SRS_UMOCKTYPES_C_01_094: [** umocktypes_copy_unsignedlong shall copy the unsigned long value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_095: [** On success umocktypes_copy_unsignedlong shall return 0. **]**
**SRS_UMOCKTYPES_C_01_096: [** If source or destination are NULL, umocktypes_copy_unsignedlong shall return a non-zero value. **]**

##umocktypes_free_unsignedlong

```c
extern void umocktypes_free_unsignedlong(unsigned long* value);
```

**SRS_UMOCKTYPES_C_01_097: [** umocktypes_free_unsignedlong shall do nothing. **]**

##umocktypes_stringify_longlong

```c
extern char* umocktypes_stringify_longlong(const long long* value);
```

**SRS_UMOCKTYPES_C_01_098: [** umocktypes_stringify_longlong shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_099: [** If value is NULL, umocktypes_stringify_longlong shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_100: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_longlong shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_101: [** If any other error occurs when creating the string representation, umocktypes_stringify_longlong shall return NULL. **]**

##umocktypes_are_equal_longlong

```c
extern int umocktypes_are_equal_longlong(const long long* left, const long long* right);
```

**SRS_UMOCKTYPES_C_01_102: [** umocktypes_are_equal_longlong shall compare the 2 long longs pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_103: [** If any of the arguments is NULL, umocktypes_are_equal_longlong shall return -1. **]**
**SRS_UMOCKTYPES_C_01_104: [** If the values pointed to by left and right are equal, umocktypes_are_equal_longlong shall return 1. **]**
**SRS_UMOCKTYPES_C_01_105: [** If the values pointed to by left and right are different, umocktypes_are_equal_longlong shall return 0. **]**

##umocktypes_copy_longlong

```c
extern int umocktypes_copy_longlong(long long* destination, const long long* source);
```

**SRS_UMOCKTYPES_C_01_106: [** umocktypes_copy_longlong shall copy the long long value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_107: [** On success umocktypes_copy_longlong shall return 0. **]**
**SRS_UMOCKTYPES_C_01_108: [** If source or destination are NULL, umocktypes_copy_longlong shall return a non-zero value. **]**

##umocktypes_free_longlong

```c
extern void umocktypes_free_longlong(long long* value);
```

**SRS_UMOCKTYPES_C_01_109: [** umocktypes_free_longlong shall do nothing. **]**

##umocktypes_stringify_unsignedlonglong

```c
extern char* umocktypes_stringify_unsignedlonglong(const unsigned long long* value);
```

**SRS_UMOCKTYPES_C_01_110: [** umocktypes_stringify_unsignedlonglong shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_111: [** If value is NULL, umocktypes_stringify_unsignedlonglong shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_112: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_unsignedlonglong shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_113: [** If any other error occurs when creating the string representation, umocktypes_stringify_unsignedlonglong shall return NULL. **]**

##umocktypes_are_equal_unsignedlonglong

```c
extern int umocktypes_are_equal_unsignedlonglong(const unsigned long long* left, const unsigned long long* right);
```

**SRS_UMOCKTYPES_C_01_114: [** umocktypes_are_equal_unsignedlonglong shall compare the 2 unsigned long longs pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_115: [** If any of the arguments is NULL, umocktypes_are_equal_unsignedlonglong shall return -1. **]**
**SRS_UMOCKTYPES_C_01_116: [** If the values pointed to by left and right are equal, umocktypes_are_equal_unsignedlonglong shall return 1. **]**
**SRS_UMOCKTYPES_C_01_117: [** If the values pointed to by left and right are different, umocktypes_are_equal_unsignedlonglong shall return 0. **]**

##umocktypes_copy_unsignedlonglong

```c
extern int umocktypes_copy_unsignedlonglong(unsigned long long* destination, const unsigned long long* source);
```

**SRS_UMOCKTYPES_C_01_118: [** umocktypes_copy_unsignedlonglong shall copy the unsigned long long value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_119: [** On success umocktypes_copy_unsignedlonglong shall return 0. **]**
**SRS_UMOCKTYPES_C_01_120: [** If source or destination are NULL, umocktypes_copy_unsignedlonglong shall return a non-zero value. **]**

##umocktypes_free_unsignedlonglong

```c
extern void umocktypes_free_unsignedlonglong(unsigned long long* value);
```

**SRS_UMOCKTYPES_C_01_121: [** umocktypes_free_unsignedlonglong shall do nothing. **]**

##umocktypes_stringify_float

```c
extern char* umocktypes_stringify_float(const float* value);
```

**SRS_UMOCKTYPES_C_01_122: [** umocktypes_stringify_float shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_123: [** If value is NULL, umocktypes_stringify_float shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_124: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_float shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_125: [** If any other error occurs when creating the string representation, umocktypes_stringify_float shall return NULL. **]**

##umocktypes_are_equal_float

```c
extern int umocktypes_are_equal_float(const float* left, const float* right);
```

**SRS_UMOCKTYPES_C_01_126: [** umocktypes_are_equal_float shall compare the 2 floats pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_127: [** If any of the arguments is NULL, umocktypes_are_equal_float shall return -1. **]**
**SRS_UMOCKTYPES_C_01_128: [** If the values pointed to by left and right are equal, umocktypes_are_equal_float shall return 1. **]**
**SRS_UMOCKTYPES_C_01_129: [** If the values pointed to by left and right are different, umocktypes_are_equal_float shall return 0. **]**

##umocktypes_copy_float

```c
extern int umocktypes_copy_float(float* destination, const float* source);
```

**SRS_UMOCKTYPES_C_01_130: [** umocktypes_copy_float shall copy the float value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_131: [** On success umocktypes_copy_float shall return 0. **]**
**SRS_UMOCKTYPES_C_01_132: [** If source or destination are NULL, umocktypes_copy_float shall return a non-zero value. **]**

##umocktypes_free_float

```c
extern void umocktypes_free_float(float* value);
```

**SRS_UMOCKTYPES_C_01_133: [** umocktypes_free_float shall do nothing. **]**

##umocktypes_stringify_double

```c
extern char* umocktypes_stringify_double(const double* value);
```

**SRS_UMOCKTYPES_C_01_134: [** umocktypes_stringify_double shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_135: [** If value is NULL, umocktypes_stringify_double shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_136: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_double shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_137: [** If any other error occurs when creating the string representation, umocktypes_stringify_double shall return NULL. **]**

##umocktypes_are_equal_double

```c
extern int umocktypes_are_equal_double(const double* left, const double* right);
```

**SRS_UMOCKTYPES_C_01_138: [** umocktypes_are_equal_double shall compare the 2 doubles pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_139: [** If any of the arguments is NULL, umocktypes_are_equal_double shall return -1. **]**
**SRS_UMOCKTYPES_C_01_140: [** If the values pointed to by left and right are equal, umocktypes_are_equal_double shall return 1. **]**
**SRS_UMOCKTYPES_C_01_141: [** If the values pointed to by left and right are different, umocktypes_are_equal_double shall return 0. **]**

##umocktypes_copy_double

```c
extern int umocktypes_copy_double(double* destination, const double* source);
```

**SRS_UMOCKTYPES_C_01_142: [** umocktypes_copy_double shall copy the double value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_143: [** On success umocktypes_copy_double shall return 0. **]**
**SRS_UMOCKTYPES_C_01_144: [** If source or destination are NULL, umocktypes_copy_double shall return a non-zero value. **]**

##umocktypes_free_double

```c
extern void umocktypes_free_double(double* value);
```

**SRS_UMOCKTYPES_C_01_145: [** umocktypes_free_double shall do nothing. **]**

##umocktypes_stringify_longdouble

```c
extern char* umocktypes_stringify_longdouble(const long double* value);
```

**SRS_UMOCKTYPES_C_01_146: [** umocktypes_stringify_longdouble shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_147: [** If value is NULL, umocktypes_stringify_longdouble shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_148: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_longdouble shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_149: [** If any other error occurs when creating the string representation, umocktypes_stringify_longdouble shall return NULL. **]**

##umocktypes_are_equal_longdouble

```c
extern int umocktypes_are_equal_longdouble(const long double* left, const long double* right);
```

**SRS_UMOCKTYPES_C_01_150: [** umocktypes_are_equal_longdouble shall compare the 2 long doubles pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_151: [** If any of the arguments is NULL, umocktypes_are_equal_longdouble shall return -1. **]**
**SRS_UMOCKTYPES_C_01_152: [** If the values pointed to by left and right are equal, umocktypes_are_equal_longdouble shall return 1. **]**
**SRS_UMOCKTYPES_C_01_153: [** If the values pointed to by left and right are different, umocktypes_are_equal_longdouble shall return 0. **]**

##umocktypes_copy_longdouble

```c
extern int umocktypes_copy_longdouble(long double* destination, const long double* source);
```

**SRS_UMOCKTYPES_C_01_154: [** umocktypes_copy_longdouble shall copy the long double value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_155: [** On success umocktypes_copy_longdouble shall return 0. **]**
**SRS_UMOCKTYPES_C_01_156: [** If source or destination are NULL, umocktypes_copy_longdouble shall return a non-zero value. **]**

##umocktypes_free_longdouble

```c
extern void umocktypes_free_longdouble(long double* value);
```

**SRS_UMOCKTYPES_C_01_157: [** umocktypes_free_longdouble shall do nothing. **]**

##umocktypes_stringify_size_t

```c
extern char* umocktypes_stringify_size_t(const size_t* value);
```

**SRS_UMOCKTYPES_C_01_158: [** umocktypes_stringify_size_t shall return the string representation of value. **]**
**SRS_UMOCKTYPES_C_01_159: [** If value is NULL, umocktypes_stringify_size_t shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_160: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_size_t shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_161: [** If any other error occurs when creating the string representation, umocktypes_stringify_size_t shall return NULL. **]**

##umocktypes_are_equal_size_t

```c
extern int umocktypes_are_equal_size_t(const size_t* left, const size_t* right);
```

**SRS_UMOCKTYPES_C_01_162: [** umocktypes_are_equal_size_t shall compare the 2 size_ts pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_163: [** If any of the arguments is NULL, umocktypes_are_equal_size_t shall return -1. **]**
**SRS_UMOCKTYPES_C_01_164: [** If the values pointed to by left and right are equal, umocktypes_are_equal_size_t shall return 1. **]**
**SRS_UMOCKTYPES_C_01_165: [** If the values pointed to by left and right are different, umocktypes_are_equal_size_t shall return 0. **]**

##umocktypes_copy_size_t

```c
extern int umocktypes_copy_size_t(size_t* destination, const size_t* source);
```

**SRS_UMOCKTYPES_C_01_166: [** umocktypes_copy_size_t shall copy the size_t value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_167: [** On success umocktypes_copy_size_t shall return 0. **]**
**SRS_UMOCKTYPES_C_01_168: [** If source or destination are NULL, umocktypes_copy_size_t shall return a non-zero value. **]**

##umocktypes_free_size_t

```c
extern void umocktypes_free_size_t(size_t* value);
```

**SRS_UMOCKTYPES_C_01_169: [** umocktypes_free_size_t shall do nothing. **]**

##umocktypes_stringify_void_ptr

```c
extern char* umocktypes_stringify_void_ptr(const void** value);
```

**SRS_UMOCKTYPES_C_01_170: [** umocktypes_stringify_void_ptr shall return the string representation of the value pointer. **]**
**SRS_UMOCKTYPES_C_01_171: [** If value is NULL, umocktypes_stringify_void_ptr shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_172: [** If allocating a new string to hold the string representation fails, umocktypes_stringify_void_ptr shall return NULL. **]**
**SRS_UMOCKTYPES_C_01_173: [** If any other error occurs when creating the string representation, umocktypes_stringify_void_ptr shall return NULL. **]**

##umocktypes_are_equal_void_ptr

```c
extern int umocktypes_are_equal_void_ptr(const void** left, const void** right);
```

**SRS_UMOCKTYPES_C_01_174: [** umocktypes_are_equal_void_ptr shall compare the 2 void_ptrs pointed to by left and right. **]**
**SRS_UMOCKTYPES_C_01_175: [** If any of the arguments is NULL, umocktypes_are_equal_void_ptr shall return -1. **]**
**SRS_UMOCKTYPES_C_01_176: [** If the values pointed to by left and right are equal, umocktypes_are_equal_void_ptr shall return 1. **]**
**SRS_UMOCKTYPES_C_01_177: [** If the values pointed to by left and right are different, umocktypes_are_equal_void_ptr shall return 0. **]**

##umocktypes_copy_void_ptr

```c
extern int umocktypes_copy_void_ptr(void** destination, const void** source);
```

**SRS_UMOCKTYPES_C_01_178: [** umocktypes_copy_void_ptr shall copy the void_ptr value from source to destination. **]**
**SRS_UMOCKTYPES_C_01_179: [** On success umocktypes_copy_void_ptr shall return 0. **]**
**SRS_UMOCKTYPES_C_01_180: [** If source or destination are NULL, umocktypes_copy_void_ptr shall return a non-zero value. **]**

##umocktypes_free_void_ptr

```c
extern void umocktypes_free_void_ptr(void** value);
```

**SRS_UMOCKTYPES_C_01_181: [** umocktypes_free_void_ptr shall do nothing. **]**

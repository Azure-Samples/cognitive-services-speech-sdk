strings requirements
================

##Overview

The STRING object encapsulates a char* variable.  This interface is access by STRING_HANDLE variables that provide further encapsulation of the interface.

##Exposed API
```c
typedef void* STRING_HANDLE;


typedef void* STRING_HANDLE;
 
extern STRING_HANDLE STRING_new(void);
extern STRING_HANDLE STRING_clone(STRING_HANDLE handle);
extern STRING_HANDLE STRING_construct(const char* psz);
extern STRING_HANDLE STRING_construct_n(const char* psz, size_t n);
extern STRING_HANDLE STRING_new_with_memory(const char* memory);
extern STRING_HANDLE STRING_new_quoted(const char* source);
extern STRING_HANDLE STRING_new_JSON(const char* source);
extern STRING_HANDLE STRING_from_byte_array(const unsigned char* source, size_t size);
extern void STRING_delete(STRING_HANDLE handle);
extern int STRING_concat(STRING_HANDLE handle, const char* s2);
extern int STRING_concat_with_STRING(STRING_HANDLE s1, STRING_HANDLE s2);
extern int STRING_quote(STRING_HANDLE handle);
extern int STRING_copy(STRING_HANDLE s1, const char* s2);
extern int STRING_copy_n(STRING_HANDLE s1, const char* s2, size_t n);
extern const char* STRING_c_str(STRING_HANDLE handle);
extern int STRING_empty(STRING_HANDLE handle);
extern size_t STRING_length(STRING_HANDLE handle);
extern int STRING_compare(STRING_HANDLE h1, STRING_HANDLE h2);

```

###STRING_new
```c
extern STRING_HANDLE STRING_new(void);
```

**SRS_STRING_07_001: [**STRING_new shall allocate a new STRING_HANDLE pointing to an empty string.**]** 
**SRS_STRING_07_002: [**STRING_new shall return an NULL STRING_HANDLE on any error that is encountered.**]**

###STRING_clone
```c
extern STRING_HANDLE STRING_clone(STRING_HANDLE handle);
```

**SRS_STRING_02_001: [**STRING_clone shall produce a new string having the same content as the handle string.**]**
**SRS_STRING_02_002: [**If parameter handle is NULL then STRING_clone shall return NULL.**]**
**SRS_STRING_02_003: [**If STRING_clone fails for any reason, it shall return NULL.**]** 

###STRING_construct
```c
extern STRING_HANDLE STRING_construct(const char*)
```

**SRS_STRING_07_003: [**STRING_construct shall allocate a new string with the value of the specified const char*.**]** 
**SRS_STRING_07_005: [**If the supplied const char* is NULL STRING_construct shall return a NULL value.**]** 
**SRS_STRING_07_032: [**STRING_construct encounters any error it shall return a NULL value.**]**

###STRING_new_with_memory
```c
extern STRING_HANDLE STRING_new_with_memory(char*)
```

**SRS_STRING_07_006: [**STRING_new_with_memory shall return a STRING_HANDLE by using the supplied char* memory.**]** 
**SRS_STRING_07_007: [**STRING_new_with_memory shall return a NULL STRING_HANDLE if the supplied char* is NULL.**]**

###STRING_new_quoted
```c
extern STRING_HANDLE STRING_new_quoted(const char*)
```
**SRS_STRING_07_008: [**STRING_new_quoted shall return a valid STRING_HANDLE Copying the supplied const char* value surrounded by quotes.**]** 
**SRS_STRING_07_009: [**STRING_new_quoted shall return a NULL STRING_HANDLE if the supplied const char* is NULL.**]** 
**SRS_STRING_07_031: [**STRING_new_quoted shall return a NULL STRING_HANDLE if any error is encountered.**]**

###STRING_new_JSON
```c
extern STRING_HANDLE STRING_new_JSON(const char* source);
```

STRING_new_JSON produces a JSON value representation of the parameter passed as argument source.
**SRS_STRING_02_011: [**If source is NULL then STRING_new_JSON shall return NULL.**]** 
STRING_new_JSON shall produce a STRING_HANDLE according to the following:
**SRS_STRING_02_012: [**The string shall begin with the quote character.**]** 
**SRS_STRING_02_013: [**The string shall copy the characters of source "as they are" (until the '\0' character) with the following exceptions:**]** 
	**SRS_STRING_02_014: [**If any character has the value outside [1...127**]** then STRING_new_JSON shall fail and return NULL.] 
	**SRS_STRING_02_016: [**If the character is " (quote) then it shall be repsented as \".**]** 
	**SRS_STRING_02_017: [**If the character is \ (backslash) then it shall represented as \\.**]** 
	**SRS_STRING_02_018: [**If the character is / (slash) then it shall be represented as \/.**]** 
	**SRS_STRING_02_019: [**If the character code is less than 0x20 then it shall be represented as \u00xx, where xx is the hex representation of the character code.**]** 
**SRS_STRING_02_020: [**The string shall end with " (quote).**]** 
**SRS_STRING_02_021: [**If the complete JSON representation cannot be produced, then STRING_new_JSON shall fail and return NULL.**]**

###STRING_delete
```c
extern void STRING_delete(STRING_HANDLE handle);
```

**SRS_STRING_07_010: [**STRING_delete will free the memory allocated by the STRING_HANDLE.**]** 
**SRS_STRING_07_011: [**STRING_delete will not attempt to free anything with a NULL STRING_HANDLE.**]**

###STRING_concat 
```c
extern int STRING_concat(STRING_HANDLE handle, const char* s2)
```
**SRS_STRING_07_012: [**STRING_concat shall concatenate the given STRING_HANDLE and the const char* value and place the value in the handle.**]** 
**SRS_STRING_07_013: [**STRING_concat shall return a nonzero number if an error is encountered.**]**

###STRING_concat
```c 
extern int STRING_concat(STRING_HANDLE handle, const char* s2)
```
**SRS_STRING_07_034: [**String_Concat_with_STRING shall concatenate a given STRING_HANDLE variable with a source STRING_HANDLE.**]** 
**SRS_STRING_07_035: [**String_Concat_with_STRING shall return a nonzero number if an error is encountered.**]**

###STRING_quote
```c 
extern int STRING_quote(STRING_HANDLE handle)
```
**SRS_STRING_07_014: [**STRING_quote shall �quote� the supplied STRING_HANDLE and return 0 on success.**]** 
**SRS_STRING_07_015: [**STRING_quote shall return a nonzero value if any of the supplied parameters are NULL.**]** 
**SRS_STRING_07_029: [**STRING_quote shall return a nonzero value if any error is encountered.**]**

###STRING_copy
```c 
extern int STRING_copy(STRING_HANDLE s1, const char* s2)
```
**SRS_STRING_07_016: [**STRING_copy shall copy the const char* into the supplied STRING_HANDLE.**]** 
**SRS_STRING_07_017: [**STRING_copy shall return a nonzero value if any of the supplied parameters are NULL.**]** 
**SRS_STRING_07_027: [**STRING_copy shall return a nonzero value if any error is encountered.**]** 
**SRS_STRING_07_026: [**If the underlying char* refered to by s1 handle is equal to char* s2 than STRING_copy shall be a noop and return 0.**]** 
**SRS_STRING_07_033: [**If overlapping pointer address is given to STRING_copy the behavior is undefined.**]**

###STRING_copy_n
```c 
extern int STRING_copy_n(STRING_HANDLE s1, const char* s2, size_t n)
```

**SRS_STRING_07_018: [**STRING_copy_n shall copy the number of characters in const char* or the size_t whichever is lesser.**]** 
**SRS_STRING_07_019: [**STRING_copy_n shall return a nonzero value if STRING_HANDLE or const char* is NULL.**]** 
**SRS_STRING_07_028: [**STRING_copy_n shall return a nonzero value if any error is encountered.**]**

###STRING_c_str
```c 
extern const char* STRING_c_str(STRING_HANDLE handle)
```

**SRS_STRING_07_020: [**STRING_c_str shall return the const char* associated with the given STRING_HANDLE.**]** 
**SRS_STRING_07_021: [**STRING_c_str shall return NULL if the STRING_HANDLE is NULL.**]**

### STRING_empty
```c
extern int STRING_empty(STRING_HANDLE s1)
```
**SRS_STRING_07_022: [**STRING_empty shall revert the STRING_HANDLE to an empty state.**]** 
**SRS_STRING_07_023: [**STRING_empty shall return a nonzero value if the STRING_HANDLE is NULL.**]**
**SRS_STRING_07_030: [**STRING_empty shall return a nonzero value if the STRING_HANDLE is NULL.**]**

###STRING_length
```c
extern size_t STRING_length(STRING_HANDLE handle)
```
**SRS_STRING_07_024: [**STRING_length shall return the length of the underlying char* for the given handle**]** 
**SRS_STRING_07_025: [**STRING_length shall return zero if the given handle is NULL.**]**
 
###STRING_construct_n
```c
extern STRING_HANDLE STRING_construct_n(const char* psz, size_t n);
```

**SRS_STRING_02_007: [**STRING_construct_n shall construct a STRING_HANDLE from first "n" characters of the string pointed to by psz parameter.**]** 
**SRS_STRING_02_008: [**If psz is NULL then STRING_construct_n shall return NULL.**]** 
**SRS_STRING_02_009: [**If n is bigger than the size of the string psz, then STRING_construct_n shall return NULL.**]** 
**SRS_STRING_02_010: [**In all other error cases, STRING_construct_n shall return NULL.**]** 

###STRING_compare
```c
extern int STRING_compare(STRING_HANDLE h1, STRING_HANDLE h2);
```

**SRS_STRING_07_034: [**STRING_compare returns an integer greater than, equal to, or less than zero, accordingly as the string pointed to by s1 is greater than, equal to, or less than the string s2.**]** 
**SRS_STRING_07_035: [**If h1 and h2 are both NULL then STRING_compare shall return 0.**]** 
**SRS_STRING_07_036: [**If h1 is NULL and h2 is a nonNULL value then STRING_compare shall return 1.**]** 
**SRS_STRING_07_037: [**If h2 is NULL and h1 is a nonNULL value then STRING_compare shall return -1.**]** 
**SRS_STRING_07_038: [**STRING_compare shall compare the char s variable using the strcmp function.**]**

###STRING_from_byte_array
```c
extern STRING_HANDLE STRING_from_byte_array(const unsigned char* source, size_t size)
```

STRING_from_BUFFER builds a string that has the same content (byte-by-byte) as a byte array. 

**SRS_STRING_02_022: [** If `source` is NULL and size > 0 then `STRING_from_BUFFER` shall fail and return NULL. **]**
**SRS_STRING_02_023: [** Otherwise, `STRING_from_BUFFER` shall build a string that has the same content (byte-by-byte) as source and return a non-NULL handle. **]**
**SRS_STRING_02_024: [** If building the string fails, then `STRING_from_BUFFER` shall fail and return NULL. **]**
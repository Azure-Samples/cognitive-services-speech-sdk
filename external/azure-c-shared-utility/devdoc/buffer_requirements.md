BUFFER Requirements
================

##Overview

The BUFFER object encapsulastes a unsigned char* variable.

##Exposed API
```c
typedef void* BUFFER_HANDLE;

extern BUFFER_HANDLE BUFFER_new(void);

extern void BUFFER_delete(BUFFER_HANDLE handle);
extern BUFFER_HANDLE BUFFER_create(const unsigned char* source, size_t size);
extern int BUFFER_pre_build(BUFFER_HANDLE handle, size_t size);
extern int BUFFER_build(BUFFER_HANDLE handle, const unsigned char* source, size_t size);
extern int BUFFER_unbuild(BUFFER_HANDLE handle);
extern int BUFFER_enlarge(BUFFER_HANDLE handle, size_t enlargeSize);
extern int BUFFER_content(BUFFER_HANDLE handle, const unsigned char** content);
extern int BUFFER_size(BUFFER_HANDLE handle, size_t* size);
extern int BUFFER_append(BUFFER_HANDLE handle1, BUFFER_HANDLE handle2);
extern unsigned char* BUFFER_u_char(BUFFER_HANDLE handle);
extern size_t BUFFER_length(BUFFER_HANDLE handle);
extern BUFFER_HANDLE BUFFER_clone(BUFFER_HANDLE handle);
```

###BUFFER_new
```c
BUFFER_HANDLE BUFFER_new(void)
```

**SRS_BUFFER_07_001: [**BUFFER_new shall allocate a BUFFER_HANDLE that will contain a NULL unsigned char*.**]**
 
###BUFFER_create
```c
extern BUFFER_HANDLE BUFFER_create(const unsigned char* source, size_t size);
```

BUFFER_create creates a new buffer from the memory at source, having size "size".
**SRS_BUFFER_02_001: [**If source is NULL then BUFFER_create shall return NULL.**]** 
**SRS_BUFFER_02_002: [**Otherwise, BUFFER_create shall allocate memory to hold size bytes and shall copy from source size bytes into the newly allocated memory.**]** 
**SRS_BUFFER_02_003: [**If allocating memory fails, then BUFFER_create shall return NULL.**]** 
**SRS_BUFFER_02_004: [**Otherwise, BUFFER_create shall return a non-NULL handle.**]**

###BUFFER_delete
```c
void BUFFER_delete(BUFFER_HANDLE handle)
```

**SRS_BUFFER_07_003: [**BUFFER_delete shall delete the data associated with the BUFFER_HANDLE along with the Buffer.**]** 
**SRS_BUFFER_07_004: [**BUFFER_delete shall not delete any BUFFER_HANDLE that is NULL.**]**

###BUFFER_pre_build
```c 
int BUFFER_pre_build(BUFFER_HANDLE handle, size_t size)
```

**SRS_BUFFER_07_005: [**BUFFER_pre_build allocates size_t bytes of BUFFER_HANDLE and returns zero on success.**]** 
**SRS_BUFFER_07_006: [**If handle is NULL or size is 0 then BUFFER_pre_build shall return a nonzero value.**]** 
**SRS_BUFFER_07_007: [**BUFFER_pre_build shall return nonzero if the buffer has been previously allocated and is not NULL.**]** 
**SRS_BUFFER_07_013: [**BUFFER_pre_build shall return nonzero if any error is encountered.**]**

###BUFFER_build
```c 
int BUFFER_build(BUFFER_HANDLE handle, const unsigned char* source, size_t size)
```

**SRS_BUFFER_07_008: [**BUFFER_build allocates size_t bytes, copies the unsigned char* into the buffer and returns zero on success.**]** 
**SRS_BUFFER_07_009: [**BUFFER_build shall return nonzero if handle is NULL **]**
**SRS_BUFFER_01_001: [**If size is positive and source is NULL, BUFFER_build shall return nonzero**]**
**SRS_BUFFER_01_002: [**The size argument can be zero, in which case the underlying buffer held by the buffer instance shall be freed.**]**
**SRS_BUFFER_01_003: [**If size is zero, source can be NULL.**]** 
**SRS_BUFFER_07_010: [**BUFFER_build shall return nonzero if any error is encountered.**]** 
**SRS_BUFFER_07_011: [**BUFFER_build shall overwrite previous contents if the buffer has been previously allocated.**]**

###BUFFER_unbuild
```c
int BUFFER_unbuild(BUFFER_HANDLE b)
```
**SRS_BUFFER_07_012: [**BUFFER_unbuild shall clear the underlying unsigned char* data associated with the BUFFER_HANDLE this will return zero on success.**]** 
**SRS_BUFFER_07_014: [**BUFFER_unbuild shall return a nonzero value if BUFFER_HANDLE is NULL.**]** 
**SRS_BUFFER_07_015: [**BUFFER_unbuild shall return a nonzero value if the unsigned char* referenced by BUFFER_HANDLE is NULL.**]**

###BUFFER_enlarge
```c
int BUFFER_enlarge(BUFFER_HANDLE handle, size_t enlargeSize)
```

**SRS_BUFFER_07_016: [**BUFFER_enlarge shall increase the size of the unsigned char* referenced by BUFFER_HANDLE.**]** 
**SRS_BUFFER_07_017: [**BUFFER_enlarge shall return a nonzero result if any parameters are NULL or zero.**]** 
**SRS_BUFFER_07_018: [**BUFFER_enlarge shall return a nonzero result if any error is encountered.**]**
 
###BUFFER_content
```c
int BUFFER_content(BUFFER_HANDLE handle, unsigned char** content)
```

**SRS_BUFFER_07_019: [**BUFFER_content shall return the data contained within the BUFFER_HANDLE.**]** 
**SRS_BUFFER_07_020: [**If the handle and/or content*is NULL BUFFER_content shall return nonzero.**]**
 
###BUFFER_size
```c
int BUFFER_size(BUFFER_HANDLE b, size_t* size)
```

**SRS_BUFFER_07_021: [**BUFFER_size shall place the size of the associated buffer in the size variable and return zero on success.**]** 
**SRS_BUFFER_07_022: [**BUFFER_size shall return a nonzero value for any error that is encountered.**]**
 
###BUFFER_append
```c
int BUFFER_append(BUFFER_HANDLE b1, BUFFER_HANDLE b2)
```

**SRS_BUFFER_07_024: [**BUFFER_append concatenates b2 onto b1 without modifying b2 and shall return zero on success.**]** 
**SRS_BUFFER_07_023: [**BUFFER_append shall return a nonzero upon any error that is encountered.**]**
 
###BUFFER_u_char
```c
unsigned char* BUFFER_u_char(BUFFER_HANDLE handle)
```

**SRS_BUFFER_07_025: [**BUFFER_u_char shall return a pointer to the underlying unsigned char*.**]** 
**SRS_BUFFER_07_026: [**BUFFER_u_char shall return NULL for any error that is encountered.**]**
 
###BUFFER_length
```c
size_t BUFFER_length(BUFFER_HANDLE handle)
```

**SRS_BUFFER_07_027: [**BUFFER_length shall return the size of the underlying buffer.**]** 
**SRS_BUFFER_07_028: [**BUFFER_length shall return zero for any error that is encountered.**]** 

ConstBuffer Requirements
================


## Overview

ConstBuffer is a module that implements a read-only buffer of bytes (unsigned char). 
Once created, the buffer can no longer be changed. The buffer is ref counted so further _Clone calls result in
zero copy.


## References
[refcount](../inc/refcount.h)

[buffer](buffer_requirements.md)

## Exposed API
```C
/*this is the handle*/
typedef struct CONSTBUFFER_HANDLE_DATA_TAG* CONSTBUFFER_HANDLE;

/*this is what is returned when the content of the buffer needs access*/
typedef struct CONSTBUFFER_TAG
{
    const unsigned char* buffer;
    size_t size;
} CONSTBUFFER;

/*this creates a new constbuffer from a memory area*/
extern CONSTBUFFER_HANDLE CONSTBUFFER_Create(const unsigned char* source, size_t size);

/*this creates a new constbuffer from an existing BUFFER_HANDLE*/
extern CONSTBUFFER_HANDLE CONSTBUFFER_CreateFromBuffer(BUFFER_HANDLE buffer);

extern CONSTBUFFER_HANDLE CONSTBUFFER_Clone(CONSTBUFFER_HANDLE constbufferHandle);

extern const CONSTBUFFER* CONSTBUFFER_GetContent(CONSTBUFFER_HANDLE constbufferHandle); 

extern void CONSTBUFFER_HANDLE_Destroy(CONSTBUFFER constbufferHandle);
```

### ConstBuffer_Create
```C
extern CONSTBUFFER_HANDLE CONSTBUFFER_Create(const unsigned char* source, size_t size);
```
**SRS_CONSTBUFFER_02_001: [**If `source` is NULL and `size` is different than 0 then CONSTBUFFER_Create shall fail and return NULL.**]** 
**SRS_CONSTBUFFER_02_002: [**Otherwise, `CONSTBUFFER_Create` shall create a copy of the memory area pointed to by `source` having `size` bytes.**]**
**SRS_CONSTBUFFER_02_003: [**If creating the copy fails then `CONSTBUFFER_Create` shall return NULL.**]**
**SRS_CONSTBUFFER_02_004: [**Otherwise `CONSTBUFFER_Create` shall return a non-NULL handle.**]** 
**SRS_CONSTBUFFER_02_005: [**The non-NULL handle returned by `CONSTBUFFER_Create` shall have its ref count set to "1".**]** 

###CONSTBUFFER_CreateFromBuffer
```C
extern CONSTBUFFER_HANDLE CONSTBUFFER_CreateFromBuffer(BUFFER_HANDLE buffer);
```
**SRS_CONSTBUFFER_02_006: [**If `buffer` is NULL then `CONSTBUFFER_CreateFromBuffer` shall fail and return NULL.**]**
**SRS_CONSTBUFFER_02_007: [**Otherwise, `CONSTBUFFER_CreateFromBuffer` shall copy the content of `buffer`.**]**
**SRS_CONSTBUFFER_02_008: [**If copying the content fails, then `CONSTBUFFER_CreateFromBuffer` shall fail and return NULL.**]**
**SRS_CONSTBUFFER_02_009: [**Otherwise, `CONSTBUFFER_CreateFromBuffer` shall return a non-NULL handle.**]**
**SRS_CONSTBUFFER_02_010: [**The non-NULL handle returned by `CONSTBUFFER_CreateFromBuffer` shall have its ref count set to "1".**]** 

###CONSTBUFFER_GetContent
```C
extern const CONSTBUFFER* CONSTBUFFER_GetContent(CONSTBUFFER_HANDLE constbufferHandle);
```
**SRS_CONSTBUFFER_02_011: [**If `constbufferHandle` is NULL then CONSTBUFFER_GetContent shall return NULL.**]**
**SRS_CONSTBUFFER_02_012: [**Otherwise, `CONSTBUFFER_GetContent` shall return a `const CONSTBUFFER*` that matches byte by byte the original bytes used to created the const buffer and has the same length.**]**

###CONSTBUFFER_Clone
```C
extern CONSTBUFFER_HANDLE CONSTBUFFER_Clone(CONSTBUFFER_HANDLE constbufferHandle);
```
**SRS_CONSTBUFFER_02_013: [**If `constbufferHandle` is NULL then CONSTBUFFER_Clone shall fail and return NULL.**]**
**SRS_CONSTBUFFER_02_014: [**Otherwise, `CONSTBUFFER_Clone` shall increment the reference count and return `constbufferHandle`.**]**

###CONSTBUFFER_Destroy
```C
extern void CONSTBUFFER_Destroy(CONSTBUFFER_HANDLE constbufferHandle);
```
**SRS_CONSTBUFFER_02_015: [**If `constbufferHandle` is NULL then `CONSTBUFFER_Destroy` shall do nothing.**]**
**SRS_CONSTBUFFER_02_016: [**Otherwise, `CONSTBUFFER_Destroy` shall decrement the refcount on the `constbufferHandle` handle.**]** 
**SRS_CONSTBUFFER_02_017: [**If the refcount reaches zero, then `CONSTBUFFER_Destroy` shall deallocate all resources used by the CONSTBUFFER_HANDLE.**]**





lock requirements
================


## Overview

The Lock component is implemented to be able to achieve thread synchronization, as we may have a requirement to consume locks across different platforms .This component will expose some generic APIs so that it can be extended to platform specific implementation .

## Exposed API
**SRS_LOCK_99_001: [** Lock interface exposes the following APIs **]**
```c
typedef enum LOCK_RESULT_TAG
{
    LOCK_OK,
    LOCK_ERROR
    
} LOCK_RESULT;
```

```c
typedef void* HANDLE_LOCK; 
```
**SRS_LOCK_99_015: [** This is the handle to the different lock instances **]**

```c
HANDLE_LOCK Lock_Init (void) ; 
```
**SRS_LOCK_99_002: [** This API on success will return a valid lock handle which should be a non `NULL` value **]**

**SRS_LOCK_99_003: [** On Error Should return `NULL` **]**

```c
LOCK_RESULT Lock(HANDLE_LOCK handle) ; 
```
**SRS_LOCK_99_004: [** This API should be implemented as a mutex lock on specific platforms **]**

**SRS_LOCK_99_005: [** This API on success should return `LOCK_OK` **]**

**SRS_LOCK_99_006: [** This API on error should return `LOCK_ERROR` **]**

**SRS_LOCK_99_007: [** This API on `NULL` handle passed returns `LOCK_ERROR` **]**

```c
LOCK_RESULT Unlock(HANDLE_LOCK handle) ; 
```
**SRS_LOCK_99_008: [** This API should be implemented as a mutex unlock on specific platforms **]**

**SRS_LOCK_99_009: [** This API on success should return `LOCK_OK` **]**

**SRS_LOCK_99_010: [** This API on error should return `LOCK_ERROR` **]**

**SRS_LOCK_99_011: [** This API on `NULL` handle passed returns `LOCK_ERROR` **]**

```c
LOCK_RESULT Lock_Deinit(HANDLE_LOCK  handle) ; 
```
**SRS_LOCK_99_012: [** This API frees the memory pointed by handle **]**

**SRS_LOCK_99_013: [** This API on `NULL` handle passed returns `LOCK_ERROR` **]**



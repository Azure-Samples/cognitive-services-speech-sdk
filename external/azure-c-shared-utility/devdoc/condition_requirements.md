Condition Requirements
================


## Overview

Condition is a module which allows thread synchronization to occur in a cross-platform manner.
It provides a mechanism for signalling events between threads and for waiting on events in a platform-independent manner.   

## References

[lock](lock_requirements.md)

## Exposed API
```C
/**
* @brief	This API creates and returns a valid condition handle.
*
* @return	A valid @c COND_HANDLE when successful or @c NULL otherwise.
*/
extern COND_HANDLE Condition_Init(void);

/**
* @brief	unblock all currently working condition.
*
* @param	handle	A valid handle to the lock.
*
* @return	Returns @c COND_OK when the condition object has been
* 			destroyed and @c COND_ERROR when an error occurs
* 			and @c COND_TIMEOUT when the handle times out.
*/
extern COND_RESULT Condition_Post(COND_HANDLE  handle);

/**
* @brief	block on the condition handle unti the thread is signalled
*           or until the timeout_milliseconds is reached.
*
* @param	handle	A valid handle to the lock.
*
* @return	Returns @c COND_OK when the condition object has been
* 			destroyed and @c COND_ERROR when an error occurs
* 			and @c COND_TIMEOUT when the handle times out.
*/
extern COND_RESULT Condition_Wait(COND_HANDLE  handle, LOCK_HANDLE lock, int timeout_milliseconds);

/**
* @brief	The condition instance is deinitialized.
*
* @param	handle	A valid handle to the condition.
*
* @return	Returns @c COND_OK when the condition object has been
* 			destroyed and @c COND_ERROR when an error occurs.
*/
extern void Condition_Deinit(COND_HANDLE  handle);
```

### Condition_Init
```C
extern COND_HANDLE Condition_Init(void);
```

**SRS_CONDITION_18_002: [** `Condition_Init` shall create and return a `CONDITION_HANDLE` **]**

**SRS_CONDITION_18_008: [** `Condition_Init` shall return `NULL` if it fails to allocate the `CONDITION_HANDLE` **]**


### Condition_Post
```C
extern COND_RESULT Condition_Post(COND_HANDLE  handle);
```

**SRS_CONDITION_18_003: [** `Condition_Post` shall return `COND_OK` if it succcessfully posts the condition **]** 

**SRS_CONDITION_18_001: [** `Condition_Post` shall return `COND_INVALID_ARG` if `handle` is `NULL` **]**


### Condition_Wait
```C
extern COND_RESULT Condition_Wait(COND_HANDLE  handle, LOCK_HANDLE lock, int timeout_milliseconds);
```

**SRS_CONDITION_18_004: [** `Condition_Wait` shall return `COND_INVALID_ARG` if `handle` is `NULL` **]** 

**SRS_CONDITION_18_005: [** `Condition_Wait` shall return `COND_INVALID_ARG` if `lock` is `NULL` and `timeout_milliseconds` is `0` **]**

**SRS_CONDITION_18_006: [** `Condition_Wait` shall return `COND_INVALID_ARG` if `lock` is `NULL` and `timeout_milliseconds` is not `0` **]**

**SRS_CONDITION_18_010: [** `Condition_Wait` shall return `COND_OK` if the condition is triggered and `timeout_milliseconds` is `0` **]**

**SRS_CONDITION_18_011: [** `Condition_Wait` shall return `COND_TIMEOUT` if the condition is NOT triggered and `timeout_milliseconds` is not `0` **]**

**SRS_CONDITION_18_012: [** `Condition_Wait` shall return `COND_OK` if the condition is triggered and `timeout_milliseconds` is not `0` **]**

**SRS_CONDITION_18_013: [** `Condition_Wait` shall accept relative timeouts **]**


### Condition_Deinit
```C
extern void Condition_Deinit(COND_HANDLE  handle);
```

**SRS_CONDITION_18_007: [** `Condition_Deinit` will not fail if `handle` is `NULL` **]**

**SRS_CONDITION_18_009: [** `Condition_Deinit` will deallocate `handle` if it is not `NULL` **]**

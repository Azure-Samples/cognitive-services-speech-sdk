#umockalloc requirements
 
#Overview

umockalloc is a module that wraps the C memory allocation functions (malloc, realloc and free).
This is so that umock_c can control what code is executed for allocation and there are no undesired redefinitions of malloc/realloc/free.

#Exposed API

```c
extern void* umockalloc_malloc(size_t size);
extern void* umockalloc_realloc(void* ptr, size_t size);
extern void umockalloc_free(void* ptr);
```

##umockalloc_malloc

```c
extern void* umockalloc_malloc(size_t size);
```

**SRS_UMOCKALLOC_01_001: [** umockalloc_malloc shall call malloc, while passing the size argument to malloc. **]**
**SRS_UMOCKALLOC_01_002: [** umockalloc_malloc shall return the result of malloc. **]**

##umockalloc_realloc

```c
extern void* umockalloc_realloc(void* ptr, size_t size);
```

**SRS_UMOCKALLOC_01_003: [** umockalloc_realloc shall call realloc, while passing the ptr and size arguments to realloc. **]**
**SRS_UMOCKALLOC_01_004: [** umockalloc_realloc shall return the result of realloc. **]**

##umockalloc_free

```c
extern void umockalloc_free(void* ptr);
```

**SRS_UMOCKALLOC_01_005: [** umockalloc_free shall call free, while passing the ptr argument to free. **]**

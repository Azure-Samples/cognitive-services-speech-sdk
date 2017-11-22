xio requirements
================
 
##Overview

xio is module that implements an IO interface, abstracting from upper layers the functionality of simply sending or receiving a sequence of bytes.

##Exposed API

```c
typedef struct XIO_INSTANCE_TAG* XIO_HANDLE;
typedef void* CONCRETE_IO_HANDLE;

typedef enum IO_SEND_RESULT_TAG
{
    IO_SEND_OK,
    IO_SEND_ERROR,
    IO_SEND_CANCELLED
} IO_SEND_RESULT;

typedef enum IO_OPEN_RESULT_TAG
{
    IO_OPEN_OK,
    IO_OPEN_ERROR,
    IO_OPEN_CANCELLED
} IO_OPEN_RESULT;

typedef void(*ON_BYTES_RECEIVED)(void* context, const unsigned char* buffer, size_t size);
typedef void(*ON_SEND_COMPLETE)(void* context, IO_SEND_RESULT send_result);
typedef void(*ON_IO_OPEN_COMPLETE)(void* context, IO_OPEN_RESULT open_result);
typedef void(*ON_IO_CLOSE_COMPLETE)(void* context);
typedef void(*ON_IO_ERROR)(void* context);

typedef CONCRETE_IO_HANDLE(*IO_CREATE)(void* io_create_parameters);
typedef void(*IO_DESTROY)(CONCRETE_IO_HANDLE concrete_io);
typedef int(*IO_OPEN)(CONCRETE_IO_HANDLE concrete_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context);
typedef int(*IO_CLOSE)(CONCRETE_IO_HANDLE concrete_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context);
typedef int(*IO_SEND)(CONCRETE_IO_HANDLE concrete_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context);
typedef void(*IO_DOWORK)(CONCRETE_IO_HANDLE concrete_io);
typedef int(*IO_SETOPTION)(CONCRETE_IO_HANDLE concrete_io, const char* optionName, const void* value);

typedef struct IO_INTERFACE_DESCRIPTION_TAG
{
    IO_CREATE concrete_io_create;
    IO_DESTROY concrete_io_destroy;
    IO_OPEN concrete_io_open;
    IO_CLOSE concrete_io_close;
    IO_SEND concrete_io_send;
    IO_DOWORK concrete_io_dowork;
    IO_SETOPTION concrete_io_setoption;
} IO_INTERFACE_DESCRIPTION;

extern XIO_HANDLE xio_create(const IO_INTERFACE_DESCRIPTION* io_interface_description, const void* io_create_parameters);
extern void xio_destroy(XIO_HANDLE xio);
extern int xio_open(XIO_HANDLE xio, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context);
extern int xio_close(XIO_HANDLE xio, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context);
extern int xio_send(XIO_HANDLE xio, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context);
extern void xio_dowork(XIO_HANDLE xio);
extern int xio_setoption(XIO_HANDLE xio, const char* optionName, const void* value);
```

###xio_create

```c
extern XIO_HANDLE xio_create(const IO_INTERFACE_DESCRIPTION* io_interface_description, const void* io_create_parameters);
```

**SRS_XIO_01_001: [**xio_create shall return on success a non-NULL handle to a new IO interface.**]**
**SRS_XIO_01_002: [**In order to instantiate the concrete IO implementation the function concrete_xio_create from the io_interface_description shall be called, passing the xio_create_parameters argument.**]**
**SRS_XIO_01_016: [**If the underlying concrete_xio_create call fails, xio_create shall return NULL.**]**
**SRS_XIO_01_003: [**If the argument io_interface_description is NULL, xio_create shall return NULL.**]**
**SRS_XIO_01_004: [**If any io_interface_description member is NULL, xio_create shall return NULL.**]**
**SRS_XIO_01_017: [**If allocating the memory needed for the IO interface fails then xio_create shall return NULL.**]** 

###xio_destroy

```c
extern void xio_destroy(XIO_HANDLE xio);
```

**SRS_XIO_01_005: [**xio_destroy shall free all resources associated with the IO handle.**]**
**SRS_XIO_01_006: [**xio_destroy shall also call the concrete_xio_destroy function that is member of the io_interface_description argument passed to xio_create, while passing as argument to concrete_xio_destroy the result of the underlying concrete_xio_create handle that was called as part of the xio_create call.**]**
**SRS_XIO_01_007: [**If the argument io is NULL, xio_destroy shall do nothing.**]** 

###xio_open

```c
extern int xio_open(XIO_HANDLE xio, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context);
```

**SRS_XIO_01_019: [**xio_open shall call the specific concrete_xio_open function specified in xio_create, passing callback function and context arguments for three events: open completed, bytes received, and IO error.**]**
**SRS_XIO_01_020: [**On success, xio_open shall return 0.**]**
**SRS_XIO_01_021: [**If handle is NULL, xio_open shall return a non-zero value.**]**
**SRS_XIO_01_022: [**If the underlying concrete_xio_open fails, xio_open shall return a non-zero value.**]** 

###xio_close

```c
extern int xio_close(XIO_HANDLE xio, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context);
```

**SRS_XIO_01_023: [**xio_close shall call the specific concrete_xio_close function specified in xio_create.**]**
**SRS_XIO_01_024: [**On success, xio_close shall return 0.**]**
**SRS_XIO_01_025: [**If the argument io is NULL, xio_close shall return a non-zero value.**]**
**SRS_XIO_01_026: [**If the underlying concrete_xio_close fails, xio_close shall return a non-zero value.**]**

###xio_send

```c
extern int xio_send(XIO_HANDLE xio, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context);
```

**SRS_XIO_01_008: [**xio_send shall pass the sequence of bytes pointed to by buffer to the concrete IO implementation specified in xio_create, by calling the concrete_xio_send function while passing down the buffer and size arguments to it.**]**
**SRS_XIO_01_027: [**xio_send shall pass to the concrete_xio_send function the on_send_complete and callback_context arguments.**]**
**SRS_XIO_01_009: [**On success, xio_send shall return 0.**]**
**SRS_XIO_01_010: [**If the argument xio is NULL, xio_send shall return a non-zero value.**]**
**SRS_XIO_01_015: [**If the underlying concrete_xio_send fails, xio_send shall return a non-zero value.**]**
**SRS_XIO_01_011: [**No error check shall be performed on buffer and size.**]** 

###xio_dowork

```c
extern void xio_dowork(XIO_HANDLE xio);
```

**SRS_XIO_01_012: [**xio_dowork shall call the concrete IO implementation specified in xio_create, by calling the concrete_xio_dowork function.**]**
**SRS_XIO_01_013: [**On success, xio_send shall return 0.**]**
**SRS_XIO_01_014: [**If the underlying concrete_xio_dowork fails, xio_dowork shall return a non-zero value.**]**
**SRS_XIO_01_018: [**When the io argument is NULL, xio_dowork shall do nothing.**]**

###xio_setoption

```c
extern int xio_setoption(XIO_HANDLE xio, const char* optionName, const void* value);
```

**SRS_XIO_03_028: [**xio_setoption shall pass the optionName and value to the concrete IO implementation specified in xio_create by invoking the concrete_xio_setoption function.**]**
**SRS_XIO_03_029: [**xio_setoption shall return 0 upon success.**]**
**SRS_XIO_03_030: [**If the xio argument or the optionName argument is NULL, xio_setoption shall return a non-zero value.**]**
**SRS_XIO_03_031: [**If the underlying concrete_xio_setoption fails, xio_setOption shall return a non-zero value.**]**

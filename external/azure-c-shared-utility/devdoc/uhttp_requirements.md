uhttp Requirements
================

## Overview
The uhttp module provides a platform Independent http implementation

## Reference RFC
https://tools.ietf.org/html/rfc7230

## RFC 7230 features not yet supported
8.1.0 Persistent Connections
8.2.3 100 Continue Response 
14.33 Proxy Authentication

## Exposed API
```c
#define HTTP_CLIENT_RESULT_VALUES                \
HTTP_CLIENT_OK,                                  \
HTTP_CLIENT_INVALID_ARG,                         \
HTTP_CLIENT_ERROR,                               \
HTTP_CLIENT_OPEN_REQUEST_FAILED,                 \
HTTP_CLIENT_SET_OPTION_FAILED,                   \
HTTP_CLIENT_SEND_REQUEST_FAILED,                 \
HTTP_CLIENT_ALREADY_INIT,                        \
HTTP_CLIENT_HTTP_HEADERS_FAILED,                 \
HTTP_CLIENT_INVALID_CHUNK_REQUEST                \

DEFINE_ENUM(HTTP_CLIENT_RESULT, HTTP_CLIENT_RESULT_VALUES);

#define HTTP_CLIENT_REQUEST_TYPE_VALUES \
    HTTP_CLIENT_REQUEST_OPTIONS,         \
    HTTP_CLIENT_REQUEST_GET,            \
    HTTP_CLIENT_REQUEST_POST,           \
    HTTP_CLIENT_REQUEST_PUT,            \
    HTTP_CLIENT_REQUEST_DELETE,         \
    HTTP_CLIENT_REQUEST_PATCH           \

DEFINE_ENUM(HTTP_CLIENT_REQUEST_TYPE, HTTP_CLIENT_REQUEST_TYPE_VALUES);

typedef void(*ON_HTTP_CONNECTED_CALLBACK)(HTTP_CLIENT_HANDLE handle, void* callback_ctx);
typedef void(*ON_HTTP_ERROR_CALLBACK)(HTTP_CLIENT_HANDLE handle, void* callback_ctx);
typedef void(*ON_HTTP_REPLY_RECV_CALLBACK)(HTTP_CLIENT_HANDLE handle, void* callback_ctx, const unsigned char* content, size_t contentLen, unsigned int statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle);
typedef void(*ON_HTTP_CHUNK_REPLY_CALLBACK)(HTTP_CLIENT_HANDLE handle, void* callback_ctx, const unsigned char* content, size_t contentLen, unsigned int statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle, bool replyComplete);

extern HTTP_CLIENT_HANDLE http_client_create(XIO_HANDLE xioHandle, ON_HTTP_CONNECTED_CALLBACK onConnected, ON_HTTP_REPLY_RECV_CALLBACK on_http_reply_recv, ON_HTTP_ERROR_CALLBACK on_http_error, ON_HTTP_CHUNK_REPLY_CALLBACK on_chunk_reply, void* callback_ctx);

extern void http_client_destroy(HTTP_CLIENT_HANDLE handle);

extern HTTP_CLIENT_RESULT http_client_open(HTTP_CLIENT_HANDLE handle, const char* host);

extern void http_client_close(HTTP_CLIENT_HANDLE handle);

extern HTTP_CLIENT_RESULT http_client_execute_request(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const unsigned char* content, size_t contentLength);

extern HTTP_CLIENT_RESULT http_client_execute_chunk(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath, httpHeadersHandle);
extern HTTP_CLIENT_RESULT http_client_send_data_chunk(HTTP_CLIENT_HANDLE handle, const unsigned char* content, size_t chunkSize);
extern HTTP_CLIENT_RESULT http_client_end_data_chunk(HTTP_CLIENT_HANDLE handle);

extern void http_client_dowork(HTTP_CLIENT_HANDLE handle);

extern HTTP_CLIENT_RESULT http_client_set_trace(HTTP_CLIENT_HANDLE handle, bool traceOn);
```
### http_client_create
```c
HTTP_CLIENT_HANDLE http_client_create(XIO_HANDLE xioHandle, ON_HTTP_CONNECTED_CALLBACK onConnect, ON_HTTP_REPLY_RECV_CALLBACK on_http_reply_recv, ON_HTTP_ERROR_CALLBACK on_http_error, ON_HTTP_CHUNK_REPLY_CALLBACK on_chunk_reply, void* callback_ctx);
```
http_client_create initializes the http client object.  
**SRS_UHTTP_07_001: [**http_client_create shall return an initialize the http client handle.**]**  
**SRS_UHTTP_07_002: [**If on_http_msg_recv or xioHandle is NULL then http_client_create shall return NULL**]**  
**SRS_UHTTP_07_003: [**If http_client_create encounters any error then it shall return NULL**]**  

### http_client_deinit
```c
void http_client_destroy(HTTP_CLIENT_HANDLE handle);
```

http_client_destroy cleans up all items that have been allocated.  
**SRS_UHTTP_07_004: [**If handle is NULL then http_client_destroy shall do nothing**]**  
**SRS_UHTTP_07_005: [**http_client_destroy shall free any memory that is allocated by http_client_create**]**  

### http_client_open
```c
HTTP_CLIENT_RESULT http_client_open(HTTP_CLIENT_HANDLE handle, const char* host);
```

http_client_open connects to the xioHandle calling the onConnect callback if supplied.  
**SRS_UHTTP_07_006: [**If any parameter is NULL then http_client_open shall return HTTP_CLIENT_INVALID_ARG**]**  
**SRS_UHTTP_07_007: [**http_client_open shall attempt to open the xioHandle and shall return HTTP_CLIENT_OPEN_REQUEST_FAILED if a failure is encountered**]**  
**SRS_UHTTP_07_008: [**If http_client_open succeeds then it shall return HTTP_CLIENT_OK**]**  

### http_client_close
```c
void http_client_close(HTTP_CLIENT_HANDLE handle);
```
http_client_close closes the xioHandle connection.  
**SRS_UHTTP_07_009: [**If handle is NULL then http_client_close shall do nothing**]**  
**SRS_UHTTP_07_010: [**If the xio_handle is NOT NULL http_client_close shall call xio_close to close the handle**]**  

### http_client_execute_request
```c
HTTP_CLIENT_RESULT http_client_execute_request(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath,
    HTTP_HEADERS_HANDLE httpHeadersHandle, const unsigned char* content, size_t contentLength);
```
http_client_execute_request send data to the http endpoint.  
**SRS_UHTTP_07_012: [**If handle, relativePath, or httpHeadersHandle is NULL then http_client_execute_request shall return HTTP_CLIENT_INVALID_ARG.**]**  
**SRS_UHTTP_07_013: [**if content is not NULL and contentLength is 0 or content is NULL and contentLength is not 0 then http_client_execute_request shall return HTTP_CLIENT_INVALID_ARG.**]**  
  HTTP_CLIENT_REQUEST_TYPE shall support all request types specified under section 9.1.2 in the spec.  
**SRS_UHTTP_07_014: [**If the requestType is not a valid request http_client_execute_request shall return HTTP_CLIENT_ERROR.**]**  
**SRS_UHTTP_07_015: [**http_client_execute_request shall add the Content-Length http header item to the request.**]**  
**SRS_UHTTP_07_011: [**http_client_execute_request shall add the HOST http header item to the request if not supplied (RFC 7230 - 5.4).**]**  
**SRS_UHTTP_07_016: [**http_client_execute_request shall transmit the http headers data through a call to xio_send;**]**  
**SRS_UHTTP_07_018: [**upon success http_client_execute_request shall then transmit the content data, if supplied, through a call to xio_send.**]**  
**SRS_UHTTP_07_017: [**If any failure is encountered http_client_execute_request shall return HTTP_CLIENT_ERROR.**]**  

### http_client_execute_chunk
```c
HTTP_CLIENT_RESULT http_client_execute_chunk(HTTP_CLIENT_HANDLE handle, HTTP_CLIENT_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE httpHeadersHandle);
```

http_client_execute_chunk initiates a http chunk send to the endpoint.  
**SRS_UHTTP_07_019: [**If handle, relativePath, or httpHeadersHandle then http_client_execute_request shall return HTTP_CLIENT_INVALID_ARG.**]**  
**SRS_UHTTP_07_020: [**If any failure is encountered http_client_execute_chunk shall return HTTP_CLIENT_ERROR.**]**  
**SRS_UHTTP_07_021: [**http_client_execute_chunk shall use xio_send to send the http header folloed by a \r\n.**]**   
**SRS_UHTTP_07_025: [**if http_client_execute_chunk finishes successfully then it shall return HTTP_CLIENT_OK.**]**  

### http_client_send_data_chunk
```c
HTTP_CLIENT_RESULT http_client_send_data_chunk(HTTP_CLIENT_HANDLE handle, const unsigned char* content, size_t contentLength);
```

http_client_send_data_chunk send a http data chunk to the endpoint.  
**SRS_UHTTP_07_026: [**If handle or content is NULL or contentLength is 0 then http_client_send_data_chunk shall return HTTP_CLIENT_INVALID_ARG**]**  
**SRS_UHTTP_07_027: [**http_client_send_data_chunk shall ensure that the http_client_execute_chunk was previously called.  If not true then http_client_execute_chunk shall return HTTP_CLIENT_INVALID_CHUNK_REQUEST.**]**  
**SRS_UHTTP_07_028: [**http_client_send_data_chunk shall use xio_send to send the chunked content size followed by a \r\n to the server...**]**  
**SRS_UHTTP_07_029: [**then http_client_send_data_chunk shall use xio_send to send the content to the server...**]**  
**SRS_UHTTP_07_030: [**and finally the http_client_send_data_chunk shall use xio_send to send a single \r\n to the server.**]**  
**SRS_UHTTP_07_031: [**If http_client_send_data_chunk finishes successfully then it shall return HTTP_CLIENT_OK**]**  

### http_client_end_data_chunk
```c
HTTP_CLIENT_RESULT http_client_end_data_chunk(HTTP_CLIENT_HANDLE handle);
```

http_client_end_data_chunk ends the chunk data transmission.   
**SRS_UHTTP_07_032: [**If handle is NULL then http_client_end_data_chunk shall return HTTP_CLIENT_INVALID_ARG**]**  
**SRS_UHTTP_07_033: [**If http_client_end_data_chunk is called when a request has not been started it shall fail with HTTP_CLIENT_INVALID_CHUNK_REQUEST.**]**  
**SRS_UHTTP_07_034: [**http_client_end_data_chunk shall send a single \r\n to the server indicating the end of the chunk request**]**  
**SRS_UHTTP_07_035: [**If http_client_end_data_chunk finishes successfully then it shall return HTTP_CLIENT_OK.**]**  

### http_client_dowork
```c
void http_client_dowork(HTTP_CLIENT_HANDLE handle);
```

http_client_dowork executes the http work that includes sends and receives.   
**SRS_UHTTP_07_036: [**If handle is NULL then http_client_dowork shall do nothing**]**  
**SRS_UHTTP_07_037: [**http_client_dowork shall call the underlying xio_dowork function**]**  

### http_client_set_trace
```c
HTTP_CLIENT_RESULT http_client_set_trace(HTTP_CLIENT_HANDLE handle, bool traceOn)
```

http_client_set_trace turns on or off log tracing.  
**SRS_UHTTP_07_038: [**If handle is NULL then http_client_set_trace shall return HTTP_CLIENT_INVALID_ARG**]**  
**SRS_UHTTP_07_039: [**http_client_set_trace shall set the HTTP tracing to the traceOn variable**]**  
**SRS_UHTTP_07_040: [**if http_client_set_trace finishes successfully then it shall return HTTP_CLIENT_OK.**]**  
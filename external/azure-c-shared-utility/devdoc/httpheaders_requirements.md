httpheaders requirements
================
 
##Overview
 
HttpHeaders is a utility module that handles message-headers. HttpHeaders uses Map function calls to store / retrieve http headers.

##References
[http headers: http://tools.ietf.org/html/rfc2616 , section 4.2, section 4.1](http://tools.ietf.org/html/rfc2616) 
[structure of header fields: http://tools.ietf.org/html/rfc822#section-3.1](http://tools.ietf.org/html/rfc822#section-3.1) 

##Exposed API
**SRS_HTTP_HEADERS_99_001: [** HttpHeaders shall have the following interface**]**
```c
typedef enum HTTP_HEADERS_RESULT_TAG
{
    HTTP_HEADERS_OK,
    HTTP_HEADERS_INVALID_ARG,
    HTTP_HEADERS_ALLOC_FAILED,
    HTTP_HEADERS_INSUFFICIENT_BUFFER,
    HTTP_HEADERS_ERROR
} HTTP_HEADERS_RESULT;
 
typedef void* HTTP_HEADERS_HANDLE;
 
extern HTTP_HEADERS_HANDLE HTTPHeaders_Alloc(void);
extern void HTTPHeaders_Free(HTTP_HEADERS_HANDLE httpHeadersHandle);
extern HTTP_HEADERS_RESULT HTTPHeaders_AddHeaderNameValuePair(HTTP_HEADERS_HANDLE httpHeadersHandle, const char* name, const char* value);
extern HTTP_HEADERS_RESULT HTTPHeaders_ReplaceHeaderNameValuePair(HTTP_HEADERS_HANDLE httpHeadersHandle, const char* name, const char* value);
extern const char* HTTPHeaders_FindHeaderValue(HTTP_HEADERS_HANDLE httpHeadersHandle, const char* name);
extern HTTP_HEADERS_RESULT HTTPHeaders_GetHeaderCount(HTTP_HEADERS_HANDLE httpHeadersHandle, size_t* headersCount);
extern HTTP_HEADERS_RESULT HTTPHeaders_GetHeader(HTTP_HEADERS_HANDLE handle, size_t index, char** destination);
extern HTTP_HEADERS_HANDLE HTTPHeaders_Clone(HTTP_HEADERS_HANDLE handle);
```

An application would use HTTPHeaders_Alloc to create a new set of HTTP headers. After getting the handle, the application would build in several headers by consecutive calls to HTTPHeaders_AddHeaderNameValuePair.
When the headers are constructed, the application can retrieve the stored data by calling one of the following functions:
HTTPHeaders_FindHeaderValue - when the name of the header is known and it wants to know the value of that header
HTTPHeaders_GetHeaderCount - when the application needs to know the count of all the headers
HTTPHeaders_GetHeader - when the application needs to know the retrieve name+": "+value based on an index.

###HTTPHeaders_Alloc
```c
HTTP_HEADERS_HANDLE HTTPHeaders_Alloc(void);
```

**SRS_HTTP_HEADERS_99_002: [** This API shall produce a HTTP_HANDLE that can later be used in subsequent calls to the module.**]**
**SRS_HTTP_HEADERS_99_003: [** The function shall return NULL when the function cannot execute properly**]**
**SRS_HTTP_HEADERS_99_004: [** After a successful init, HTTPHeaders_GetHeaderCount shall report 0 existing headers.**]**

###HTTPHeaders_Free
```c
HTTPHeaders_Free(HTTP_HEADERS_HANDLE httpHeadersHandle)
```

**SRS_HTTP_HEADERS_99_005: [** Calling this API shall de-allocate the data structures allocated by previous API calls to the same handle.**]**
**SRS_HTTP_HEADERS_02_001: [**If httpHeadersHandle is NULL then HTTPHeaders_Free shall perform no action.**]**
 
###HTTPHeaders_AddHeaderNameValuePair
```c
HTTP_HEADERS_RESULT HTTPHeaders_AddHeaderNameValuePair(HTTP_HEADERS_HANDLE httpHeadersHandle, const char* name, const char* value)
```

**SRS_HTTP_HEADERS_99_012: [** Calling this API shall record a header from name and value parameters.**]**
**SRS_HTTP_HEADERS_99_013: [** The function shall return HTTP_HEADERS_OK when execution is successful.**]**
**SRS_HTTP_HEADERS_99_014: [** The function shall return when the handle is not valid or when name parameter is NULL or when value parameter is NULL.**]**
**SRS_HTTP_HEADERS_99_015: [** The function shall return HTTP_HEADERS_ALLOC_FAILED when an internal request to allocate memory fails.**]**
**SRS_HTTP_HEADERS_99_016: [** The function shall store the name:value pair in such a way that when later retrieved by a call to GetHeader it will return a string that shall strcmp equal to the name+": "+value.**]**
**SRS_HTTP_HEADERS_99_017: [** If the name already exists in the collection of headers, the function shall concatenate the new value after the existing value, separated by a comma and a space as in: old-value+", "+new-value.**]**
**SRS_HTTP_HEADERS_99_031: [** If name contains the character ":" then the return value shall be HTTP_HEADERS_INVALID_ARG.**]**
**SRS_HTTP_HEADERS_99_036: [** If name contains the characters outside character codes 33 to 126 then the return value shall be HTTP_HEADERS_INVALID_ARG**]** (so says http://tools.ietf.org/html/rfc822#section-3.1)
**SRS_HTTP_HEADERS_02_002: [**The LWS from the beginning of the value shall not be stored.**]**
 
###HTTPHeaders_ReplaceHeaderNameValuePair
```c
HTTP_HEADERS_RESULT HTTPHeaders_ReplaceHeaderNameValuePair(HTTP_HEADERS_HANDLE httpHeadersHandle, const char* name, const char* value)
```

**SRS_HTTP_HEADERS_06_001: [**This API will perform exactly as HTTPHeaders_AddHeaderNameValuePair except that if the header name already exists the already existing value will be replaced as opposed to concatenated to.**]**
 
###HTTPHeaders_FindHeaderValue
```c
const char* HTTPHeaders_FindHeaderValue(HTTP_HEADERS_HANDLE httpHeadersHandle, const char* name)
```

**SRS_HTTP_HEADERS_99_018: [** Calling this API shall retrieve the value for a previously stored name.**]**
**SRS_HTTP_HEADERS_99_022: [** The return value shall be NULL if name parameter is NULL or if httpHeadersHandle is NULL**]**
**SRS_HTTP_HEADERS_99_020: [** The return value shall be different than NULL when the name matches the name of a previously stored name:value pair.**]** 
**SRS_HTTP_HEADERS_99_021: [** In this case the return value shall point to a string that shall strcmp equal to the original stored string.**]**

###HTTPHeaders_GetHeaderCount
```c
HTTP_HEADERS_RESULT HTTPHeaders_GetHeaderCount(HTTP_HEADERS_HANDLE httpHeadersHandle, size_t* headersCount)
```

**SRS_HTTP_HEADERS_99_023: [** Calling this API shall provide the number of stored headers.**]**
**SRS_HTTP_HEADERS_99_024: [** The function shall return HTTP_HEADERS_INVALID_ARG when an invalid handle is passed.**]**
**SRS_HTTP_HEADERS_99_025: [** The function shall return HTTP_HEADERS_INVALID_ARG when headersCount is NULL.**]**
**SRS_HTTP_HEADERS_99_026: [** The function shall write in *headersCount the number of currently stored headers and shall return HTTP_HEADERS_OK**]**
**SRS_HTTP_HEADERS_99_037: [** The function shall return HTTP_HEADERS_ERROR when an internal error occurs.**]**

###HTTPHeaders_GetHeader
```c
HTTP_HEADERS_RESULT HTTPHeaders_GetHeader(HTTP_HEADERS_HANDLE handle, size_t index, char** destination);
```

**SRS_HTTP_HEADERS_99_027: [** Calling this API shall produce the string value+": "+pair) for the index header in the *destination parameter.**]**
**SRS_HTTP_HEADERS_99_028: [** The function shall return HTTP_HEADERS_INVALID_ARG if the handle is invalid.**]**
**SRS_HTTP_HEADERS_99_029: [** The function shall return HTTP_HEADERS_INVALID_ARG if index is not valid (for example, out of range) for the currently stored headers.**]**
**SRS_HTTP_HEADERS_99_032: [** The function shall return HTTP_HEADERS_INVALID_ARG if the destination  is NULL**]**
**SRS_HTTP_HEADERS_99_034: [** The function shall return HTTP_HEADERS_ERROR when an internal error occurs**]**
**SRS_HTTP_HEADERS_99_035: [** The function shall return HTTP_HEADERS_OK when the function executed without error.**]**

###HTTPHeaders_Clone
```c
extern HTTP_HEADERS_HANDLE HTTPHeaders_Clone(HTTP_HEADERS_HANDLE handle);
```
HTTPHeaders_Clone produces a clone of the handle parameter.
**SRS_HTTP_HEADERS_02_003: [**If handle is NULL then HTTPHeaders_Clone shall return NULL.**]**
**SRS_HTTP_HEADERS_02_004: [**Otherwise HTTPHeaders_Clone shall clone the content of handle to a new handle.**]**
**SRS_HTTP_HEADERS_02_005: [**If cloning fails for any reason, then HTTPHeaders_Clone shall return NULL.**]** 

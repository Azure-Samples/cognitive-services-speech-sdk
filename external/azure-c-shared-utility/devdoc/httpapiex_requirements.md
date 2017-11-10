HTTPAPIEX Requirements
================
 
##Overview
HTTAPIEX is a utility module that provides HTTP requests with build-in retry capability to an HTTP server. Features over "regular" HTTPAPI include:
-	Optional parameters
-	Implementation independent
-	Retry mechanism
-	Persistent options

##References
[httpapi_requirements]

##Exposed API
```c
#define HTTPAPIEX_RESULT_VALUES \
    HTTPAPIEX_OK, \
    HTTPAPIEX_ERROR, \
    HTTPAPIEX_INVALID_ARG, \
    HTTPAPIEX_RECOVERYFAILED
/*to be continued*/
 
DEFINE_ENUM(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);
 
extern HTTPAPIEX_HANDLE HTTPAPIEX_Create(const char* hostName);
 
extern HTTPAPIEX_RESULT HTTPAPIEX_ExecuteRequest(HTTPAPIEX_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE requestHttpHeadersHandle, BUFFER_HANDLE requestContent, unsigned int* statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent);
 
extern void HTTPAPIEX_Destroy(HTTPAPIEX_HANDLE handle);
extern HTTPAPIEX_RESULT HTTPAPIEX_SetOption(HTTPAPIEX_HANDLE handle, const char* optionName, const void* value);
```

###HTTPAPIEX_Create
```c
HTTPAPIEX_HANDLE HTTPAPIEX_Create(const char* hostName)
```

HTTPAPIEX_Create shall create a HTTPAPIEX_HANDLE that can be used in further calls. 
**SRS_HTTPAPIEX_02_001: [**If parameter hostName is NULL then HTTPAPIEX_Create shall return NULL.**]**
**SRS_HTTPAPIEX_02_002: [**Parameter hostName shall be saved.**]**
**SRS_HTTPAPIEX_02_003: [**If saving the parameter hostName fails for any reason, then HTTPAPIEX_Create shall return NULL.**]**
**SRS_HTTPAPIEX_02_004: [**Otherwise, HTTPAPIEX_Create shall return a HTTAPIEX_HANDLE suitable for further calls to the module.**]**
**SRS_HTTPAPIEX_02_005: [**If creating the handle fails for any reason, then HTTAPIEX_Create shall return NULL.**]**
 
###HTTPAPIEX_ExecuteRequest
```c
HTTPAPIEX_RESULT HTTPAPIEX_ExecuteRequest(HTTPAPIEX_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE requestHttpHeadersHandle, BUFFER_HANDLE requestContent, unsigned int* statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE  responseContent);
```

HTTPAPIEX_ExecuteRequest tries to execute an HTTP request of type requestType, on the server's relativePath relativePath, pushing the request HTTP headers requestHttpHeadersHandle, having the content of the request as pointed to by requestcontent.
If successful, HTTAPIEX writes in the out parameter statusCode the HTTP status, populates the responseHeadersHandle with the response headers and copies the response body to responseContent.

**SRS_HTTPAPIEX_02_006: [**If parameter handle is NULL then HTTPAPIEX_ExecuteRequest shall fail and return HTTPAPIEX_INVALID_ARG.**]** 
**SRS_HTTPAPIEX_02_007: [**If parameter requestType does not indicate a valid request, HTTPAPIEX_ExecuteRequest shall fail and return HTTPAPIEX_INVALID_ARG.**]** 
**SRS_HTTPAPIEX_02_008: [**If parameter relativePath is NULL then HTTPAPIEX_INVALID_ARG shall not assume a relative path - that is, it will assume an empty path ("").**]** 
**SRS_HTTPAPIEX_02_009: [**If parameter requestHttpHeadersHandle is NULL then HTTPAPIEX_ExecuteRequest shall allocate a temporary internal instance of HTTPHEADERS, shall add to that instance the following headers
	Host:{hostname} - as it was indicated by the call to HTTPAPIEX_Create API call
	Content-Length:the size of the requestContent parameter, and use this instance to all the subsequent calls to HTTPAPI_ExecuteRequest as parameter httpHeadersHandle.**]** 
**SRS_HTTPAPIEX_02_010: [**If any of the operations in SRS_HTTAPIEX_02_009 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.**]** 
**SRS_HTTPAPIEX_02_011: [**If parameter requestHttpHeadersHandle is not NULL then HTTPAPIEX_ExecuteRequest shall create or update the following headers of the request:
	Host:{hostname}
	Content-Length:the size of the requestContent parameter, and shall use the so constructed HTTPHEADERS object to all calls to HTTPAPI_ExecuteRequest as parameter httpHeadersHandle.**]** 
**SRS_HTTPAPIEX_02_012: [**If any of the operations required for SRS_HTTAPIEX_02_011 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.**]** 
**SRS_HTTPAPIEX_02_013: [**If requestContent is NULL then HTTPAPIEX_ExecuteRequest shall behave as if a buffer of zero size would have been used, that is, it shall call HTTPAPI_ExecuteRequest with parameter content = NULL and contentLength = 0.**]** 
**SRS_HTTPAPIEX_02_014: [**If requestContent is not NULL then its content and its size shall be used for parameters content and contentLength of HTTPAPI_ExecuteRequest.**]** 
**SRS_HTTPAPIEX_02_015: [**If statusCode is NULL then HTTPAPIEX_ExecuteRequest shall not write in statusCode the HTTP status code, and it will use a temporary internal int for parameter statusCode to the calls of HTTPAPI_ExecuteRequest.**]** 
**SRS_HTTPAPIEX_02_016: [**If statusCode is not NULL then If statusCode is NULL then HTTPAPIEX_ExecuteRequest shall use it for parameter statusCode to the calls of HTTPAPI_ExecuteRequest.**]** 
**SRS_HTTPAPIEX_02_017: [**If responseHeaders handle is NULL then HTTPAPIEX_ExecuteRequest shall create a temporary internal instance of HTTPHEADERS object and use that for responseHeaders parameter of HTTPAPI_ExecuteRequest call.**]** 
**SRS_HTTPAPIEX_02_018: [**If creating the temporary http headers in SRS_HTTPAPIEX_02_017 fails then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.**]** 
**SRS_HTTPAPIEX_02_019: [**If responseHeaders is not NULL, then then HTTPAPIEX_ExecuteRequest shall use that object as parameter responseHeaders of HTTPAPI_ExecuteRequest call.**]** 
**SRS_HTTPAPIEX_02_020: [**If responseContent is NULL then HTTPAPIEX_ExecuteRequest shall create a temporary internal BUFFER object and use that as parameter responseContent of HTTPAPI_ExecuteRequest call.**]** 
**SRS_HTTPAPIEX_02_021: [**If creating the BUFFER_HANDLE in SRS_HTTPAPIEX_02_020 fails, then HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_ERROR.**]** 
**SRS_HTTPAPIEX_02_022: [**If responseContent is not NULL then HTTPAPIEX_ExecuteRequest use that as parameter responseContent of HTTPAPI_ExecuteRequest call.**]** 
**SRS_HTTPAPIEX_02_023: [**HTTPAPIEX_ExecuteRequest shall try to execute the HTTP call by ensuring the following API call sequence is respected:**]** 
1.	HTTPAPI_Init
2.	HTTPAPI_CreateConnection
3.	HTTPAPI_ExecuteRequest

**SRS_HTTPAPIEX_02_035: [**HTTPAPIEX_ExecuteRequest shall pass all the saved options (see HTTPAPIEX_SetOption) to the newly create HTTPAPI_HANDLE in step 2 by calling HTTPAPI_SetOption.**]**
**SRS_HTTPAPIEX_02_036: [**If setting the option fails, then the failure shall be ignored.**]** 
**SRS_HTTPAPIEX_02_024: [**If any point in the sequence fails, HTTPAPIEX_ExecuteRequest shall attempt to recover by going back to the previous step and retrying that step.**]**
**SRS_HTTPAPIEX_02_025: [**If the first step fails, then the sequence fails.**]**
**SRS_HTTPAPIEX_02_026: [**A step shall be retried at most once.**]**
**SRS_HTTPAPIEX_02_027: [**If a step has been retried then all subsequent steps shall be retried too.**]** 
**SRS_HTTPAPIEX_02_028: [**HTTPAPIEX_ExecuteRequest shall return HTTPAPIEX_OK when a call to HTTPAPI_ExecuteRequest has been completed successfully.**]**
**SRS_HTTPAPIEX_02_029: [**Otherwise, HTTAPIEX_ExecuteRequest shall return HTTPAPIEX_RECOVERYFAILED.**]** 

###HTTPAPIEX_Destroy
```c
void HTTPAPIEX_Destroy(HTTPAPIEX_HANDLE handle);
```

**SRS_HTTPAPIEX_02_043: [**If parameter handle is NULL then HTTPAPIEX_Destroy shall take no action.**]** 
**SRS_HTTPAPIEX_02_042: [**HTTPAPIEX_Destroy shall free all the resources used by HTTAPIEX_HANDLE.**]** 

###HTTPAPIEX_SetOption
```c
extern HTTPAPIEX_RESULT HTTPAPIEX_SetOption(HTTPAPIEX_HANDLE handle, const char* optionName, const void* value);
```

HTTPAPIEX_SetOption set the option "optionName" to the value pointed to by value. Since underlying layer's handle is not necessarily created yet at the time of HTTPAPIEX_SetOption, the options and values are to be saved until the HTTPAPI_HANDLE is created and passed to HTTPAPI_HANDLE at that time. If HTTPAPI_HANDLE exists at the time of calling HTTPAPIEX_SetOption, the option and the value are still saved and passed immediately to HTTPAPI_SetOption.
**SRS_HTTPAPIEX_02_032: [**If parameter handle is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.**]** 
**SRS_HTTPAPIEX_02_033: [**If parameter optionName is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.**]** 
**SRS_HTTPAPIEX_02_034: [**If parameter value is NULL then HTTPAPIEX_SetOption shall return HTTPAPIEX_INVALID_ARG.**]** 
**SRS_HTTPAPIEX_02_030: [**If parameter optionName is one of the options handled by HTTPAPIEX then it shall be set to value *value.**]** 
**SRS_HTTPAPIEX_02_037: [**HTTPAPIEX_SetOption shall attempt to save the value of the option by calling HTTPAPI_CloneOption passing optionName and value, irrespective of the existence of a HTTPAPI_HANDLE**]**
**SRS_HTTPAPIEX_02_038: [**If HTTPAPI_CloneOption returns HTTPAPI_INVALID_ARG then HTTPAPIEX shall return HTTPAPIEX_INVALID_ARG.**]**
**SRS_HTTPAPIEX_02_039: [**If HTTPAPI_CloneOption returns HTTPAPI_OK then HTTPAPIEX_SetOption shall create or update the pair optionName/value.**]**
**SRS_HTTPAPIEX_02_041: [**If creating or updating the pair optionName/value fails then shall return HTTPAPIEX_ERROR.**]**
**SRS_HTTPAPIEX_02_040: [**For all other return values of HTTPAPI_SetOption, HTTPIAPIEX_SetOption shall return HTTPAPIEX_ERROR.**]** 
**SRS_HTTPAPIEX_02_031: [**If HTTPAPI_HANDLE exists then HTTPAPIEX_SetOption shall call HTTPAPI_SetOption passing the same optionName and value and shall return a value conforming to the below table:**]**
 
|HTTPAPI return code            |HTTPAPIEX return code|
|-------------------------------|---------------------|
|HTTPAPI_OK                     |HTTPAPIEX_OK         |
|HTTPAPI_INVALID_ARG            |HTTPAPIEX_INVALID_ARG|
|Any other HTTPAPI return code  |HTTPAPIEX_ERROR      |

Options currently handled in HTTAPIEX:
-none

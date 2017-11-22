HTTPAPIEX SAS Requirements
================
 
##Overview

This module is used to manage the SAS Tokens within HTTP headers that are passing through to the HTTPAPIEX interface.
A guiding principle of this module to that if SAS Tokens are being used within the http headers then at all costs SAS token expiration should be avoided.
Retransmission is very expensive. Additionally this module does not try to enforce any policy on the headers.
For instance, if there is no "Authorization" header, this module will make no attempt to error out.  It will still try to invoke the HTTPAPIEX_ExecuteRequest.

##References
[SAS Token Create requirements]

[HTTPAPIEX requirements]

[HTTP headers requirements]

##Exposed API
```c
typedef void* HTTPAPIEX_SAS_HANDLE;

extern HTTPAPIEX_SAS_HANDLE HTTPAPIEX_SAS_Create(STRING_HANDLE key, STRING_HANDLE uriResource, STRING_HANDLE keyName);

extern void HTTPAPIEX_SAS_Destroy(HTTPAPIEX_SAS_HANDLE handle);

extern HTTPAPIEX_RESULT HTTPAPIEX_SAS_ExecuteRequest(HTTPAPIEX_SAS_HANDLE sasHandle, HTTPAPIEX_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE requestHttpHeadersHandle, BUFFER_HANDLE requestContent, unsigned int* statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent);
```
 
###HTTPAPIEX_SAS_Create
```c
extern HTTPAPIEX_SAS_HANDLE HTTPAPIEX_SAS_Create(STRING_HANDLE key, STRING_HANDLE uriResource, STRING_HANDLE keyName);
```  

**SRS_HTTPAPIEXSAS_06_001: [**If the parameter key is NULL then HTTPAPIEX_SAS_Create shall return NULL.**]**
**SRS_HTTPAPIEXSAS_06_002: [**If the parameter uriResource is NULL then HTTPAPIEX_SAS_Create shall return NULL.**]**
**SRS_HTTPAPIEXSAS_06_003: [**If the parameter keyName is NULL then HTTPAPIEX_SAS_Create shall return NULL.**]**
**SRS_HTTPAPIEXSAS_06_004: [**If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.**]**
 
###HTTPAPIEX_SAS_Destroy
```c
extern void HTTPAPIEX_SAS_Destroy(HTTPAPIEX_SAS_HANDLE handle);
```
  
**SRS_HTTPAPIEXSAS_06_005: [**If the parameter handle is NULL then HTTAPIEX_SAS_Destroy shall do nothing and return.**]**
Otherwise, **SRS_HTTPAPIEXSAS_06_006: [**HTTAPIEX_SAS_Destroy shall deallocate any structures denoted by the parameter handle.**]**
 
###HTTPAPIEX_SAS_ExecuteRequest
```c
extern HTTPAPIEX_RESULT HTTPAPIEX_SAS_ExecuteRequest(HTTPAPIEX_SAS_HANDLE sasHandle, HTTPAPIEX_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE requestHttpHeadersHandle, BUFFER_HANDLE requestContent, unsigned int* statusCode, HTTP_HEADERS_HANDLE responseHeadersHandle, BUFFER_HANDLE responseContent);
```

**SRS_HTTPAPIEXSAS_06_007: [**If the parameter sasHandle is NULL then HTTPAPIEX_SAS_ExecuteRequest shall simply invoke HTTPAPIEX_ExecuteRequest with the remaining parameters (following sasHandle) as its arguments and shall return immediately with the result of that call as the result of HTTPAPIEX_SAS_ExecuteRequest.**]** 
This invocation of HTTPAPIEX_ExecuteRequest and returning its result as the result of HTTPAPIEX_SAS_ExecuteRequest shall be henceforth known as fallthrough.
Otherwise,**SRS_HTTPAPIEXSAS_06_008: [**if the parameter requestHttpHeadersHandle is NULL then fallthrough.**]**
**SRS_HTTPAPIEXSAS_06_009: [**HTTPHeaders_FindHeaderValue shall be invoked with the requestHttpHeadersHandle as its first argument and the string "Authorization" as its second argument.**]**
**SRS_HTTPAPIEXSAS_06_010: [**If the return result of the invocation of HTTPHeaders_FindHeaderValue is NULL then fallthrough.**]**   
The caller of HTTPAPIEX_SAS_ExecuteRequest may know that there is no "Authorization" header.  HTTPAPIEX_SAS_ExecuteRequest does not try to enforce policy.
**SRS_HTTPAPIEXSAS_06_018: [**A value of type time_t that shall be known as currentTime is obtained from calling get_time.**]**
**SRS_HTTPAPIEXSAS_06_019: [**If the value of currentTime is (time_t)-1 is then fallthrough.**]** The size_t value ((size_t) (difftime(currentTime,0) + 3600)) is obtained an shall be known as expiry. **SRS_HTTPAPIEXSAS_06_011: [**SASToken_Create shall be invoked.**]**   **SRS_HTTPAPIEXSAS_06_012: [**If the return result of SASToken_Create is NULL then fallthrough.**]** 
The call to HTTPAPIEX_ExecuteRequest is attempted because there certainly could still be a valid SAS Token as the value the Authorization header.  Note also that an error will be logged that the token could not be created.
The result of the SASToken_Create shall be known as newSASToken.
**SRS_HTTPAPIEXSAS_06_013: [**HTTPHeaders_ReplaceHeaderNameValuePair shall be invoked with "Authorization" as its second argument and STRING_c_str (newSASToken) as its third argument.**]**
**SRS_HTTPAPIEXSAS_06_015: [**STRING_delete(newSASToken) will be invoked.**]**
**SRS_HTTPAPIEXSAS_06_014: [**If the result of the invocation of HTTPHeaders_ReplaceHeaderNameValuePair is NOT HTTP_HEADERS_OK then fallthrough.**]**   
Note that an error will be logged that the "Authorization" header could not be replaced.  
Finally, **SRS_HTTPAPIEXSAS_06_016: [**HTTPAPIEX_ExecuteRequest with the remaining parameters (following sasHandle) as its arguments will be invoked and the result of that call is the result of HTTPAPIEX_SAS_ExecuteRequest.**]** 


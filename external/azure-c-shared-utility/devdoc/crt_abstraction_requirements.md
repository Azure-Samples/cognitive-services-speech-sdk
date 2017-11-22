CRTAbstractions Requirements
================
 
##Overview

Many old CRT functions have newer, more secure versions via Microsoft's implementation of the run-time library. These have the _s ("secure") suffix.
The CRTAbstractions module provides the same secure functions for when not using a Microsoft platforms.

##Generic Requirements

**SRS_CRT_ABSTRACTIONS_99_001: [**The module shall not redefine the secure functions implemented by Microsoft CRT.**]**
**SRS_CRT_ABSTRACTIONS_99_040: [**The module shall still compile when building on a Microsoft platform.**]**

##Exposed API
**SRS_CRT_ABSTRACTIONS_99_002: [**
CRTAbstractions module shall expose the following API:
```c
int strcat_s(char* dst, size_t dstSizeInBytes, const char* src);
int strcpy_s(char* dst, size_t dstSizeInBytes, const char* src);
int strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t maxCount);
int sprintf_s(char* dst, size_t dstSizeInBytes, const char* format, ... );
int mallocAndStrcpy_s(char** destination, const char* source);
int unsignedIntToString(char* destination, size_t destinationSize, unsigned int value);
int size_tToString(char* destination, size_t destinationSize, size_t value);
```
**]**

###strcat_s
```c
int strcat_s(char* dst, size_t dstSizeInBytes, const char* src);
```

**SRS_CRT_ABSTRACTIONS_99_008: [**strcat_s shall append the src to dst and terminates the resulting string with a null character.**]**
**SRS_CRT_ABSTRACTIONS_99_009: [**The initial character of src shall overwrite the terminating null character of dst.**]**
**SRS_CRT_ABSTRACTIONS_99_003: [**strcat_s shall return Zero upon success.**]**

strcat_s shall return an error code upon failure as follows:

**SRS_CRT_ABSTRACTIONS_99_004: [**If dst is NULL or unterminated, the error code returned shall be EINVAL & dst shall not be modified.**]**
**SRS_CRT_ABSTRACTIONS_99_005: [**If src is NULL, the error code returned shall be EINVAL and dst[0**]** shall be set to 0.]
**SRS_CRT_ABSTRACTIONS_99_006: [**If the dstSizeInBytes is 0 or smaller than the required size for dst & src, the error code returned shall be ERANGE & dst[0**]** set to 0.]

###strcpy_s
```c
int strcpy_s(char* dst, size_t dstSizeInBytes, const char* src);
```
**SRS_CRT_ABSTRACTIONS_99_016: [**strcpy_s shall copy the contents in the address of src, including the terminating null character, to the location that's specified by dst.**]**
**SRS_CRT_ABSTRACTIONS_99_011: [**strcpy_s shall return Zero upon success**]**

strcpy shall return an error code upon failure as follows:

**SRS_CRT_ABSTRACTIONS_99_012: [**If dst is NULL, the error code returned shall be EINVAL & dst shall not be modified.**]**
**SRS_CRT_ABSTRACTIONS_99_013: [**If src is NULL, the error code returned shall be EINVAL and dst[0**]** shall be set to 0.]
**SRS_CRT_ABSTRACTIONS_99_014: [**If the dstSizeInBytes is 0 or smaller than the required size for the src string, the error code returned shall be ERANGE & dst[0**]** set to 0.]

###strncpy_s
```c
int strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t maxCount);
```
**SRS_CRT_ABSTRACTIONS_99_025: [**strncpy_s shall copy the first N characters of src to dst, where N is the lesser of MaxCount and the length of src.**]**
**SRS_CRT_ABSTRACTIONS_99_041: [**If those N characters will fit within dst (whose size is given as dstSizeInBytes) and still leave room for a null terminator, then those characters shall be copied and a terminating null is appended; otherwise, strDest[0**]** is set to the null character and ERANGE error code returned.]
**SRS_CRT_ABSTRACTIONS_99_026: [**If MaxCount is _TRUNCATE (defined as -1), then as much of src as will fit into dst shall be copied while still leaving room for the terminating null to be appended.**]**
**SRS_CRT_ABSTRACTIONS_99_018: [**strncpy_s shall return Zero upon success**]**

strncpy_s shall return an error code upon failure as follows:

**SRS_CRT_ABSTRACTIONS_99_019: [**If truncation occurred as a result of the copy, the error code returned shall be STRUNCATE.**]**
**SRS_CRT_ABSTRACTIONS_99_020: [**If dst is NULL, the error code returned shall be EINVAL and dst shall not be modified.**]**
**SRS_CRT_ABSTRACTIONS_99_021: [**If src is NULL, the error code returned shall be EINVAL and dst[0**]** shall be set to 0.]
**SRS_CRT_ABSTRACTIONS_99_022: [**If the dstSizeInBytes is 0, the error code returned shall be EINVAL and dst shall not be modified.**]**
**SRS_CRT_ABSTRACTIONS_99_023: [**If dst is not NULL & dstSizeInBytes is smaller than the required size for the src string, the error code returned shall be ERANGE and dst[0**]** shall be set to 0.]

###sprintf_s
```c
int sprintf_s(char* dst, size_t dstSizeInBytes, const char* format, ... );
```

**SRS_CRT_ABSTRACTIONS_99_029: [**The sprintf_s function shall format and store series of characters and values in dst.  Each argument (if any) is converted and output according to the corresponding Format Specification in the format variable.**]**
**SRS_CRT_ABSTRACTIONS_99_031: [**A null character is appended after the last character written.**]**
**SRS_CRT_ABSTRACTIONS_99_027: [**sprintf_s shall return the number of characters stored in dst upon success.  This number shall not include the terminating null character.**]**
**SRS_CRT_ABSTRACTIONS_99_028: [**If dst or format is a null pointer, sprintf_s shall return -1 and set errno to EINVAL**]**
**SRS_CRT_ABSTRACTIONS_99_034: [**If the dst buffer is too small for the text being printed, then dst is set to an empty string and the function shall return -1.**]**

###mallocAndStrcpy_s
```c
int mallocAndStrcpy_s(char** destination, const char* source);
```
**SRS_CRT_ABSTRACTIONS_99_038: [**mallocAndstrcpy_s shall allocate memory for destination buffer to fit the string in the source parameter.**]**
**SRS_CRT_ABSTRACTIONS_99_039: [**mallocAndstrcpy_s shall copy the contents in the address source, including the terminating null character into location specified by the destination pointer after the memory allocation.**]**
**SRS_CRT_ABSTRACTIONS_99_035: [**mallocAndstrcpy_s shall return Zero upon success**]**

mallocAndstrcpy_s shall return an error code upon failure as follows:

**SRS_CRT_ABSTRACTIONS_99_036: [**destination parameter or source parameter is NULL, the error code returned shall be EINVAL and destination shall not be modified.**]**
**SRS_CRT_ABSTRACTIONS_99_037: [**Upon failure to allocate memory for the destination, the function will return ENOMEM.**]**

###unsignedIntToString
```c
int unsignedIntToString(char* destination, size_t destinationSize, unsigned int value)
```

**SRS_CRT_ABSTRACTIONS_02_001: [**unsignedIntToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.**]** 
**SRS_CRT_ABSTRACTIONS_02_002: [**If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and unsignedIntToString shall fail.**]** 
**SRS_CRT_ABSTRACTIONS_02_003: [**If destination is NULL then unsignedIntToString shall fail.**]** 
**SRS_CRT_ABSTRACTIONS_02_004: [**If the conversion has been successfull then unsignedIntToString shall return 0.**]**

###size_tToString
```c
int size_tToString(char* destination, size_t destinationSize, size_t value)
```

**SRS_CRT_ABSTRACTIONS_02_001: [**size_tToString shall convert the parameter value to its decimal representation as a string in the buffer indicated by parameter destination having the size indicated by parameter destinationSize.**]** 
**SRS_CRT_ABSTRACTIONS_02_002: [**If the conversion fails for any reason (for example, insufficient buffer space), a non-zero return value shall be supplied and size_tToString shall fail.**]** 
**SRS_CRT_ABSTRACTIONS_02_003: [**If destination is NULL then size_tToString shall fail.**]** 
**SRS_CRT_ABSTRACTIONS_02_004: [**If the conversion has been successfull then size_tToString shall return 0.**]**

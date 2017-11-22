Base64 Requirements
================
 
##Overview

This module is used to encode a BUFFER using the standard base64 encoding stream.

##References
[IETF RFC 4648](https://tools.ietf.org/html/rfc4648)

##Exposed API

```c
extern STRING_HANDLE Base64_Encode(BUFFER_HANDLE input);
extern STRING_HANDLE Base64_Encode_Bytes(const unsigned char* source, size_t size);
extern BUFFER_HANDLE Base64_Decoder(const char* source);
```

###Base64_Encode
```c
extern STRING_HANDLE Base64_Encode(BUFFER_HANDLE input);
```

Base64_Encode takes as a parameter a pointer to BUFFER, input.
**SRS_BASE64_06_001: [**If input is NULL then Base64_Encode shall return NULL.**]** The size of the BUFFER pointed to by input MAY be zero.
**SRS_BASE64_06_006: [**If when allocating memory to produce the encoding a failure occurs then Base64_Encode shall return NULL.**]**
**SRS_BASE64_06_007: [**Otherwise Base64_Encode shall return a pointer to STRING, that string contains the base 64 encoding of input.**]**
This encoding of input shall NOT contain embedded line feeds.

###Base64_Encode_Bytes
```c
extern STRING_HANDLE Base64_Encode_Bytes(const unsigned char* source, size_t size);
```

Base64_Encode_Bytes shall produce a STRING_HANDLE containing the base64 encoding of the buffer pointed to by source, having the size as given by parameter size.
**SRS_BASE64_02_001: [**If source is NULL then Base64_Encode_Bytes shall return NULL.**]** 
**SRS_BASE64_02_002: [**If source is not NULL and size is zero, then Base64_Encode_Bytes shall produce an empty STRING_HANDLE.**]** 
**SRS_BASE64_02_003: [**Otherwise, Base64_Encode_Bytes shall produce a STRING_HANDLE containing the Base64 representation of the buffer.**]** 
**SRS_BASE64_02_004: [**In case of any errors, Base64_Encode_Bytes shall return NULL.**]** 

###Base64_Decoder
```c
extern BUFFER_HANDLE Base64_Decoder(const char* source);
```

**SRS_BASE64_06_008: [**If source is NULL then Base64_Decoder shall return NULL.**]**
**SRS_BASE64_06_009: [**If the string pointed to by source is zero length then the handle returned shall refer to a zero length buffer.**]**
**SRS_BASE64_06_010: [**If there is any memory allocation failure during the decode then Base64_Decoder shall return NULL.**]**
**SRS_BASE64_06_011: [**If the source string has an invalid length for a base 64 encoded string then Base64_Decoder shall return NULL.**]** 

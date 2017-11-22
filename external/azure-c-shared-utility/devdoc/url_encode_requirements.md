url_encode requirements
================

##Overview

This document will specify the requirements for URL_Encode.

##Exposed API

```c
extern STRING* URL_Encode(STRING* input);
```

###URL_Encode

URL_Encode will take as a parameter a pointer to a STRING, input.  URL_Encode will return a pointer to STRING.
**SRS_URL_ENCODE_06_001: [**If input is NULL then URL_Encode will return NULL.**]**
**SRS_URL_ENCODE_06_002: [**If an error occurs during the encoding of input then URL_Encode will return NULL.**]**
**SRS_URL_ENCODE_06_003: [**If input is a zero length string then URL_Encode will return a zero length string.**]**
URL_Encode will encode input in a manner that respects the encoding used in the .net HttpUtility.UrlEncode.

sastoken requirements
================
 
##Overview

This function is used to create a SAS token.

##References
[Keyed-Hashing for Message Authentication, RFC2104](https://www.ietf.org/rfc/rfc2104.txt)

##Exposed API
```c
extern STRING_HANDLE SASToken_Create(STRING_HANDLE key, STRING_HANDLE scope, STRING_HANDLE keyName, size_t expiry);
```
 
###SASToken_Create 
```c
extern STRING_HANDLE SASToken_Create(STRING_HANDLE key, STRING_HANDLE scope, STRING_HANDLE keyName, size_t expiry);
```

**SRS_SASTOKEN_06_001: [**If key is NULL then SASToken_Create shall return NULL.**]** 
**SRS_SASTOKEN_06_003: [**If scope is NULL then SASToken_Create shall return NULL.**]**
**SRS_SASTOKEN_06_007: [**If keyName is NULL then SASToken_Create shall return NULL.**]**  
**SRS_SASTOKEN_06_029: [**The key parameter is decoded from base64.**]**
**SRS_SASTOKEN_06_030: [**If there is an error in the decoding then SASToken_Create shall return NULL.**]** The decoded value shall henceforth be known as decodedKey.
The expiry argument shall be converted to a char* by invoking size_tToString.
**SRS_SASTOKEN_06_026: [**If the conversion to string form fails for any reason then SASToken_Create shall return NULL.**]** The string shall be henceforth referred to as tokenExpirationTime.
**SRS_SASTOKEN_06_009: [**The scope is the basis for creating a STRING_HANDLE.**]**
**SRS_SASTOKEN_06_010: [**A "\n" is appended to that string.**]**
**SRS_SASTOKEN_06_011: [**tokenExpirationTime is appended to that string.**]** This is henceforth referred to as toBeHashed. 
**SRS_SASTOKEN_06_012: [**An HMAC256 hash is calculated using the decodedKey, over toBeHashed.**]** 
**SRS_SASTOKEN_06_013: [**If an error is returned from the HMAC256 function then NULL is returned from SASToken_Create.**]** 
**SRS_SASTOKEN_06_014: [**If there are any errors from the following operations then NULL shall be returned.**]** 
**SRS_SASTOKEN_06_015: [**The hash is base 64 encoded.**]** That (STRING_HANDLE) shall be called base64Signature. 
**SRS_SASTOKEN_06_028: [**base64Signature shall be url encoded.**]** This (STRING_HANDLE) shall be called urlEncodedSignature. 
**SRS_SASTOKEN_06_016: [**The string "SharedAccessSignature sr=" is the first part of the result of SASToken_Create.**]**
**SRS_SASTOKEN_06_017: [**The scope parameter is appended to result.**]**
**SRS_SASTOKEN_06_018: [**The string "&sig=" is appended to result.**]**
**SRS_SASTOKEN_06_019: [**The string urlEncodedSignature shall be appended to result.**]**
**SRS_SASTOKEN_06_020: [**The string "&se=" shall be appended to result.**]**
**SRS_SASTOKEN_06_021: [**tokenExpirationTime is appended to result.**]**
**SRS_SASTOKEN_06_022: [**The string "&skn=" is appended to result.**]**
**SRS_SASTOKEN_06_023: [**The argument keyName is appended to result.**]** 
result is returned.


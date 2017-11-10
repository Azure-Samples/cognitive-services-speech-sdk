// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include <time.h>
#include <stddef.h>

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/hmacsha256.h"
#include "azure_c_shared_utility/base64.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/xlogging.h"

STRING_HANDLE SASToken_Create(STRING_HANDLE key, STRING_HANDLE scope, STRING_HANDLE keyName, size_t expiry)
{
    STRING_HANDLE result = NULL;
    char tokenExpirationTime[32] = { 0 };

    /*Codes_SRS_SASTOKEN_06_001: [If key is NULL then SASToken_Create shall return NULL.]*/
    /*Codes_SRS_SASTOKEN_06_003: [If scope is NULL then SASToken_Create shall return NULL.]*/
    /*Codes_SRS_SASTOKEN_06_007: [If keyName is NULL then SASToken_Create shall return NULL.]*/
    if ((key == NULL) ||
        (scope == NULL) ||
        (keyName == NULL))
    {
        LogError("Invalid Parameter to SASToken_Create. handle key: %p, handle scope: %p, handle keyName: %p", key, scope, keyName);
    }
    else
    {
        BUFFER_HANDLE decodedKey;
        /*Codes_SRS_SASTOKEN_06_029: [The key parameter is decoded from base64.]*/
        if ((decodedKey = Base64_Decoder(STRING_c_str(key))) == NULL)
        {
            /*Codes_SRS_SASTOKEN_06_030: [If there is an error in the decoding then SASToken_Create shall return NULL.]*/
            LogError("Unable to decode the key for generating the SAS.");
        }
        else
        {
            /*Codes_SRS_SASTOKEN_06_026: [If the conversion to string form fails for any reason then SASToken_Create shall return NULL.]*/
            if (size_tToString(tokenExpirationTime, sizeof(tokenExpirationTime), expiry) != 0)
            {
                LogError("For some reason converting seconds to a string failed.  No SAS can be generated.");
            }
            else
            {
                STRING_HANDLE toBeHashed = NULL;
                BUFFER_HANDLE hash = NULL;
                if (((hash = BUFFER_new()) == NULL) ||
                    ((toBeHashed = STRING_new()) == NULL) ||
                    ((result = STRING_new()) == NULL))
                {
                    LogError("Unable to allocate memory to prepare SAS token.");
                }
                else
                {
                    /*Codes_SRS_SASTOKEN_06_009: [The scope is the basis for creating a STRING_HANDLE.]*/
                    /*Codes_SRS_SASTOKEN_06_010: [A "\n" is appended to that string.]*/
                    /*Codes_SRS_SASTOKEN_06_011: [tokenExpirationTime is appended to that string.]*/
                    if ((STRING_concat_with_STRING(toBeHashed, scope) != 0) ||
                        (STRING_concat(toBeHashed, "\n") != 0) ||
                        (STRING_concat(toBeHashed, tokenExpirationTime) != 0))
                    {
                        LogError("Unable to build the input to the HMAC to prepare SAS token.");
                        STRING_delete(result);
                        result = NULL;
                    }
                    else
                    {
                        STRING_HANDLE base64Signature = NULL;
                        STRING_HANDLE urlEncodedSignature = NULL;
                        /*Codes_SRS_SASTOKEN_06_013: [If an error is returned from the HMAC256 function then NULL is returned from SASToken_Create.]*/
                        /*Codes_SRS_SASTOKEN_06_012: [An HMAC256 hash is calculated using the decodedKey, over toBeHashed.]*/
                        /*Codes_SRS_SASTOKEN_06_014: [If there are any errors from the following operations then NULL shall be returned.]*/
                        /*Codes_SRS_SASTOKEN_06_015: [The hash is base 64 encoded.]*/
                        /*Codes_SRS_SASTOKEN_06_028: [base64Signature shall be url encoded.]*/
                        /*Codes_SRS_SASTOKEN_06_016: [The string "SharedAccessSignature sr=" is the first part of the result of SASToken_Create.]*/
                        /*Codes_SRS_SASTOKEN_06_017: [The scope parameter is appended to result.]*/
                        /*Codes_SRS_SASTOKEN_06_018: [The string "&sig=" is appended to result.]*/
                        /*Codes_SRS_SASTOKEN_06_019: [The string urlEncodedSignature shall be appended to result.]*/
                        /*Codes_SRS_SASTOKEN_06_020: [The string "&se=" shall be appended to result.]*/
                        /*Codes_SRS_SASTOKEN_06_021: [tokenExpirationTime is appended to result.]*/
                        /*Codes_SRS_SASTOKEN_06_022: [The string "&skn=" is appended to result.]*/
                        /*Codes_SRS_SASTOKEN_06_023: [The argument keyName is appended to result.]*/
                        if ((HMACSHA256_ComputeHash(BUFFER_u_char(decodedKey), BUFFER_length(decodedKey), (const unsigned char*)STRING_c_str(toBeHashed), STRING_length(toBeHashed), hash) != HMACSHA256_OK) ||
                            ((base64Signature = Base64_Encode(hash)) == NULL) ||
                            ((urlEncodedSignature = URL_Encode(base64Signature)) == NULL) ||
                            (STRING_copy(result, "SharedAccessSignature sr=") != 0) ||
                            (STRING_concat_with_STRING(result, scope) != 0) ||
                            (STRING_concat(result, "&sig=") != 0) ||
                            (STRING_concat_with_STRING(result, urlEncodedSignature) != 0) ||
                            (STRING_concat(result, "&se=") != 0) ||
                            (STRING_concat(result, tokenExpirationTime) != 0) ||
                            (STRING_concat(result, "&skn=") != 0) ||
                            (STRING_concat_with_STRING(result, keyName) != 0))
                        {
                            LogError("Unable to build the SAS token.");
                            STRING_delete(result);
                            result = NULL;
                        }
                        else
                        {
                            /* everything OK */
                        }
                        STRING_delete(base64Signature);
                        STRING_delete(urlEncodedSignature);
                    }
                }
                STRING_delete(toBeHashed);
                BUFFER_delete(hash);
            }
            BUFFER_delete(decodedKey);
        }
    }

    return result;
}


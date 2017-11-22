// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// PUT NO INCLUDES BEFORE HERE !!!!
//
#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include <stddef.h>
#include <string.h>
//
// PUT NO CLIENT LIBRARY INCLUDES BEFORE HERE !!!!
//
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/strings.h"

static const struct {
    size_t numberOfChars;
    const char* encoding;
} urlEncoding[] = {
    { 1, "\0"  },
    { 3, "%01" },
    { 3, "%02" },
    { 3, "%03" },
    { 3, "%04" },
    { 3, "%05" },
    { 3, "%06" },
    { 3, "%07" },
    { 3, "%08" },
    { 3, "%09" },
    { 3, "%0a" },
    { 3, "%0b" },
    { 3, "%0c" },
    { 3, "%0d" },
    { 3, "%0e" },
    { 3, "%0f" },
    { 3, "%10" },
    { 3, "%11" },
    { 3, "%12" },
    { 3, "%13" },
    { 3, "%14" },
    { 3, "%15" },
    { 3, "%16" },
    { 3, "%17" },
    { 3, "%18" },
    { 3, "%19" },
    { 3, "%1a" },
    { 3, "%1b" },
    { 3, "%1c" },
    { 3, "%1d" },
    { 3, "%1e" },
    { 3, "%1f" },
    { 3, "%20" },
    { 1, "!" },
    { 3, "%22" },
    { 3, "%23" },
    { 3, "%24" },
    { 3, "%25" },
    { 3, "%26" },
    { 3, "%27" },
    { 1, "(" },
    { 1, ")" },
    { 1, "*" },
    { 3, "%2b" },
    { 3, "%2c" },
    { 1, "-" },
    { 1, "." },
    { 3, "%2f" },
    { 1, "0" },
    { 1, "1" },
    { 1, "2" },
    { 1, "3" },
    { 1, "4" },
    { 1, "5" },
    { 1, "6" },
    { 1, "7" },
    { 1, "8" },
    { 1, "9" },
    { 3, "%3a" },
    { 3, "%3b" },
    { 3, "%3c" },
    { 3, "%3d" },
    { 3, "%3e" },
    { 3, "%3f" },
    { 3, "%40" },
    { 1, "A" },
    { 1, "B" },
    { 1, "C" },
    { 1, "D" },
    { 1, "E" },
    { 1, "F" },
    { 1, "G" },
    { 1, "H" },
    { 1, "I" },
    { 1, "J" },
    { 1, "K" },
    { 1, "L" },
    { 1, "M" },
    { 1, "N" },
    { 1, "O" },
    { 1, "P" },
    { 1, "Q" },
    { 1, "R" },
    { 1, "S" },
    { 1, "T" },
    { 1, "U" },
    { 1, "V" },
    { 1, "W" },
    { 1, "X" },
    { 1, "Y" },
    { 1, "Z" },
    { 3, "%5b" },
    { 3, "%5c" },
    { 3, "%5d" },
    { 3, "%5e" },
    { 1, "_" },
    { 3, "%60" },
    { 1, "a" },
    { 1, "b" },
    { 1, "c" },
    { 1, "d" },
    { 1, "e" },
    { 1, "f" },
    { 1, "g" },
    { 1, "h" },
    { 1, "i" },
    { 1, "j" },
    { 1, "k" },
    { 1, "l" },
    { 1, "m" },
    { 1, "n" },
    { 1, "o" },
    { 1, "p" },
    { 1, "q" },
    { 1, "r" },
    { 1, "s" },
    { 1, "t" },
    { 1, "u" },
    { 1, "v" },
    { 1, "w" },
    { 1, "x" },
    { 1, "y" },
    { 1, "z" },
    { 3, "%7b" },
    { 3, "%7c" },
    { 3, "%7d" },
    { 3, "%7e" },
    { 3, "%7f" },
    { 3, "%80" },
    { 3, "%81" },
    { 3, "%82" },
    { 3, "%83" },
    { 3, "%84" },
    { 3, "%85" },
    { 3, "%86" },
    { 3, "%87" },
    { 3, "%88" },
    { 3, "%89" },
    { 3, "%8a" },
    { 3, "%8b" },
    { 3, "%8c" },
    { 3, "%8d" },
    { 3, "%8e" },
    { 3, "%8f" },
    { 3, "%90" },
    { 3, "%91" },
    { 3, "%92" },
    { 3, "%93" },
    { 3, "%94" },
    { 3, "%95" },
    { 3, "%96" },
    { 3, "%97" },
    { 3, "%98" },
    { 3, "%99" },
    { 3, "%9a" },
    { 3, "%9b" },
    { 3, "%9c" },
    { 3, "%9d" },
    { 3, "%9e" },
    { 3, "%9f" },
    { 3, "%a0" },
    { 3, "%a1" },
    { 3, "%a2" },
    { 3, "%a3" },
    { 3, "%a4" },
    { 3, "%a5" },
    { 3, "%a6" },
    { 3, "%a7" },
    { 3, "%a8" },
    { 3, "%a9" },
    { 3, "%aa" },
    { 3, "%ab" },
    { 3, "%ac" },
    { 3, "%ad" },
    { 3, "%ae" },
    { 3, "%af" },
    { 3, "%b0" },
    { 3, "%b1" },
    { 3, "%b2" },
    { 3, "%b3" },
    { 3, "%b4" },
    { 3, "%b5" },
    { 3, "%b6" },
    { 3, "%b7" },
    { 3, "%b8" },
    { 3, "%b9" },
    { 3, "%ba" },
    { 3, "%bb" },
    { 3, "%bc" },
    { 3, "%bd" },
    { 3, "%be" },
    { 3, "%bf" },
    { 3, "%c0" },
    { 3, "%c1" },
    { 3, "%c2" },
    { 3, "%c3" },
    { 3, "%c4" },
    { 3, "%c5" },
    { 3, "%c6" },
    { 3, "%c7" },
    { 3, "%c8" },
    { 3, "%c9" },
    { 3, "%ca" },
    { 3, "%cb" },
    { 3, "%cc" },
    { 3, "%cd" },
    { 3, "%ce" },
    { 3, "%cf" },
    { 3, "%d0" },
    { 3, "%d1" },
    { 3, "%d2" },
    { 3, "%d3" },
    { 3, "%d4" },
    { 3, "%d5" },
    { 3, "%d6" },
    { 3, "%d7" },
    { 3, "%d8" },
    { 3, "%d9" },
    { 3, "%da" },
    { 3, "%db" },
    { 3, "%dc" },
    { 3, "%dd" },
    { 3, "%de" },
    { 3, "%df" },
    { 3, "%e0" },
    { 3, "%e1" },
    { 3, "%e2" },
    { 3, "%e3" },
    { 3, "%e4" },
    { 3, "%e5" },
    { 3, "%e6" },
    { 3, "%e7" },
    { 3, "%e8" },
    { 3, "%e9" },
    { 3, "%ea" },
    { 3, "%eb" },
    { 3, "%ec" },
    { 3, "%ed" },
    { 3, "%ee" },
    { 3, "%ef" },
    { 3, "%f0" },
    { 3, "%f1" },
    { 3, "%f2" },
    { 3, "%f3" },
    { 3, "%f4" },
    { 3, "%f5" },
    { 3, "%f6" },
    { 3, "%f7" },
    { 3, "%f8" },
    { 3, "%f9" },
    { 3, "%fa" },
    { 3, "%fb" },
    { 3, "%fc" },
    { 3, "%fd" },
    { 3, "%fe" },
    { 3, "%ff" }
};

STRING_HANDLE URL_EncodeString(const char* textEncode)
{
    STRING_HANDLE result;
    if (textEncode == NULL)
    {
        result = NULL;
    }
    else
    {
        STRING_HANDLE tempString = STRING_construct(textEncode);
        if (tempString == NULL)
        {
            result = NULL;
        }
        else
        {
            result = URL_Encode(tempString);
            STRING_delete(tempString);
        }
    }
    return result;
}

STRING_HANDLE URL_Encode(STRING_HANDLE input)
{
    STRING_HANDLE result;
    if (input == NULL)
    {
        /*Codes_SRS_URL_ENCODE_06_001: [If input is NULL then URL_Encode will return NULL.]*/
        result = NULL;
        LogError("URL_Encode:: NULL input");
    }
    else
    {
        size_t lengthOfResult = 0;
        char* encodedURL;
        const char* currentInput;
        unsigned char currentUnsignedChar;
        currentInput = STRING_c_str(input);
        /*Codes_SRS_URL_ENCODE_06_003: [If input is a zero length string then URL_Encode will return a zero length string.]*/
        do
        {
            currentUnsignedChar = (unsigned char)(*currentInput++);
            lengthOfResult += urlEncoding[currentUnsignedChar].numberOfChars;
        } while (currentUnsignedChar != 0);
        if ((encodedURL = malloc(lengthOfResult)) == NULL)
        {
            /*Codes_SRS_URL_ENCODE_06_002: [If an error occurs during the encoding of input then URL_Encode will return NULL.]*/
            result = NULL;
            LogError("URL_Encode:: MALLOC failure on encode.");
        }
        else
        {
            size_t currentEncodePosition = 0;
            currentInput = STRING_c_str(input);
            do
            {
                currentUnsignedChar = (unsigned char)(*currentInput++);
                if (urlEncoding[currentUnsignedChar].numberOfChars == 1)
                {
                    encodedURL[currentEncodePosition++] = *(urlEncoding[currentUnsignedChar].encoding);
                }
                else
                {
                    memcpy(encodedURL + currentEncodePosition, urlEncoding[currentUnsignedChar].encoding, urlEncoding[currentUnsignedChar].numberOfChars);
                    currentEncodePosition += urlEncoding[currentUnsignedChar].numberOfChars;
                }
            } while (currentUnsignedChar != 0);
            result = STRING_new_with_memory(encodedURL);
        }
    }
    return result;
}

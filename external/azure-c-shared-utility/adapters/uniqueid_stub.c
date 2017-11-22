// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include "azure_c_shared_utility/uniqueid.h"
#include "azure_c_shared_utility/xlogging.h"
#include <time.h>

DEFINE_ENUM_STRINGS(UNIQUEID_RESULT, UNIQUEID_RESULT_VALUES);

static const char tochar[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
static void generate128BitUUID(unsigned char* arrayOfByte)
{
    size_t arrayIndex;

    for (arrayIndex = 0; arrayIndex < 16; arrayIndex++)
    {
        arrayOfByte[arrayIndex] = (unsigned char)rand();
    }

    //
    // Stick in the version field for random uuid.
    //
    arrayOfByte[7] &= 0x0f; //clear the bit field
    arrayOfByte[7] |= 0x40; //set the ones we care about

    //
    // Stick in the variant field for the random uuid.
    //
    arrayOfByte[8] &= 0xf3; // Clear
    arrayOfByte[8] |= 0x08; // Set

}

// TODO: The User will need to call srand before calling this function
UNIQUEID_RESULT UniqueId_Generate(char* uid, size_t len)
{
    UNIQUEID_RESULT result;
    unsigned char arrayOfChar[16];

    /* Codes_SRS_UNIQUEID_07_002: [If uid is NULL then UniqueId_Generate shall return UNIQUEID_INVALID_ARG] */
    /* Codes_SRS_UNIQUEID_07_003: [If len is less then 37 then UniqueId_Generate shall return UNIQUEID_INVALID_ARG] */
    if (uid == NULL || len < 37)
    {
        result = UNIQUEID_INVALID_ARG;
        LogError("Buffer Size is Null or length is less then 37 bytes");
    }
    else 
    {
        size_t arrayIndex;
        size_t shiftCount;
        size_t characterPosition = 0;

        /* Codes_SRS_UNIQUEID_07_001: [UniqueId_Generate shall create a unique Id 36 character long string.] */
        generate128BitUUID(arrayOfChar);
        for (arrayIndex = 0; arrayIndex < 16; arrayIndex++)
        {
            for (shiftCount = 0; shiftCount <= 1; shiftCount++)
            {
                char hexChar = tochar[arrayOfChar[arrayIndex] & 0xf];
                if ((characterPosition == 8) || (characterPosition == 13) || (characterPosition == 18) || (characterPosition == 23))
                {
                    uid[characterPosition] = '-';
                    characterPosition++;
                }
                uid[characterPosition] = hexChar;
                characterPosition++;
                arrayOfChar[arrayIndex] = arrayOfChar[arrayIndex] >> 4;
            }
        }
        uid[characterPosition] = 0;
        result = UNIQUEID_OK;
    }
    return result;
}

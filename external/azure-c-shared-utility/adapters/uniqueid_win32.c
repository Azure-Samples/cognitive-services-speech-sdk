// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include "azure_c_shared_utility/uniqueid.h"
#include "azure_c_shared_utility/xlogging.h"
#include <rpc.h>

DEFINE_ENUM_STRINGS(UNIQUEID_RESULT, UNIQUEID_RESULT_VALUES);

UNIQUEID_RESULT UniqueId_Generate(char* uid, size_t len)
{
    UNIQUEID_RESULT result;

    /* Codes_SRS_UNIQUEID_07_002: [If uid is NULL then UniqueId_Generate shall return UNIQUEID_INVALID_ARG] */
    /* Codes_SRS_UNIQUEID_07_003: [If len is less then 37 then UniqueId_Generate shall return UNIQUEID_INVALID_ARG] */
    if (uid == NULL || len < 37)
    {
        result = UNIQUEID_INVALID_ARG;
        LogError("Buffer Size is Null or length is less then 37 bytes");
    }
    else 
    {
        UUID uuidVal;
        RPC_STATUS status = UuidCreate(&uuidVal);
        if (status != RPC_S_OK && status != RPC_S_UUID_LOCAL_ONLY)
        {
            LogError("Unable to aquire unique Id");
            result = UNIQUEID_ERROR;
        }
        else
        {
            unsigned char* randomResult;
            status = UuidToStringA(&uuidVal, &randomResult);
            if (status != RPC_S_OK)
            {
                LogError("Unable to convert Id to string");
                result = UNIQUEID_ERROR;
            }
            else
            {
                /* Codes_SRS_UNIQUEID_07_001: [UniqueId_Generate shall create a unique Id 36 character long string.] */
                memset(uid, 0, len);
                size_t cpyLen = strlen((char*)randomResult);
                if (cpyLen > len - 1)
                {
                    cpyLen = len - 1;
                }
                memcpy(uid, randomResult, cpyLen);
                RpcStringFreeA(&randomResult);
                result = UNIQUEID_OK;
            }
        }
    }
    return result;
}

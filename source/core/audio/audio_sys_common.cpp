//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_sys_common.cpp: Platform-independent functions to handle audio-related data.
//
#include "azure_c_shared_utility_strings_wrapper.h"

#include "audio_sys.h"

// create a new AUDIO_SETTINGS instance
AUDIO_SETTINGS_HANDLE audio_format_create()
{
    AUDIO_SETTINGS* result = nullptr;
    result = new AUDIO_SETTINGS();
    memset(result, 0, sizeof(AUDIO_SETTINGS));

    result->hDeviceName = STRING_new();

    return result;
}

// destroy a AUDIO_SETTINGS instance
void audio_format_destroy(AUDIO_SETTINGS_HANDLE handle)
{
    if (handle != NULL)
    {
        if (handle->hDeviceName)
        {
            STRING_delete(handle->hDeviceName);
        }

        delete handle;
    }
}

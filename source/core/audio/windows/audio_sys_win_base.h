//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//
// common file for winRT and win32 audio sys.

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "audio_sys.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/buffer_.h"
#include <windows.h>
#include <mmsystem.h>

#include <initguid.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <mmstream.h>
#include <endpointvolume.h>
#include <spxdebug.h>

#define MAGIC_TAG_RIFF      0x46464952
#define MAGIC_TAG_WAVE      0x45564157
#define MAGIC_TAG_data      0x61746164
#define MAGIC_TAG_fmt       0x20746d66

#define AUDIO_FORMAT_PCM        1
#define AUDIO_CHANNELS_MONO     1
#define AUDIO_SAMPLE_RATE       16000
#define AUDIO_BITS              16
#define AUDIO_BLOCK_ALIGN       (AUDIO_CHANNELS_MONO * (AUDIO_BITS >> 3))
#define AUDIO_BYTE_RATE         (AUDIO_SAMPLE_RATE * AUDIO_BLOCK_ALIGN)

#ifndef AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
#define AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM 0x80000000
#endif

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  (10000000 *25)
#define REFTIMES_PER_MILLISEC  10000


#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { LogError("The last API call returned error 0x%x", hres); goto Exit; }

#define EXIT_ON_ERROR_IF(hres, cond)                              \
    do {                                                        \
        int fCond = (cond);                                     \
        if (fCond) {                                            \
            hr = hres;                                       \
            if (FAILED(hr)) {                                \
                LogError("%s %d: The last API call returned error 0x%x", __FUNCTION__, __LINE__, hr); goto Exit; }} \
        } while (0)

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

#define SAFE_CLOSE_HANDLE(punk)  \
              if ((punk) != NULL)  \
                { CloseHandle(punk); (punk) = NULL; }

struct AudioDataBuffer
{
    BYTE *pAudioData;
    uint32_t totalSize;
    uint32_t currentSize;
};

HRESULT GetBufferAndCallBackClient(
    IAudioCaptureClient* pCaptureClient,
    AudioDataBuffer& audioBuff,
    AUDIOINPUT_WRITE audio_write_cb,
    void * user_write_ctx,
    int & audio_result);

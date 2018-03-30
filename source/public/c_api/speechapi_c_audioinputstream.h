//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_session.h: Public API declarations for AudioInputStream related C methods
//

#pragma once
#include <speechapi_c_common.h>

typedef struct tAudioInputStreamFormatC
{
    unsigned short wFormatTag;         /* format type */
    unsigned short nChannels;          /* number of channels (i.e. mono, stereo...) */
    unsigned int   nSamplesPerSec;     /* sample rate */
    unsigned int   nAvgBytesPerSec;    /* for buffer estimation */
    unsigned short nBlockAlign;        /* block size of data */
    unsigned short wBitsPerSample;     /* number of bits per sample of mono data */
    unsigned short cbSize;             /* the count in bytes of the size of */
                                       /* extra information (after cbSize) */
} AudioInputStreamFormat, *PAudioInputStreamFormat;

struct _AudioInputStream;

typedef unsigned short (*PAUDIOINPUTSTREAM_GETFORMAT_CALLBACK_FUNC)(struct _AudioInputStream *context, struct tAudioInputStreamFormatC* pformat, unsigned short cbFormat);
typedef unsigned int (*PAUDIOINPUTSTREAM_READ_CALLBACK_FUNC)(struct _AudioInputStream *context, unsigned char* pbuffer, unsigned int cbBuffer);
typedef void(*PAUDIOINPUTSTREAM_CLOSE_CALLBACK_FUNC)(struct _AudioInputStream *context);

typedef struct _AudioInputStream
{
    PAUDIOINPUTSTREAM_GETFORMAT_CALLBACK_FUNC GetFormat;
    PAUDIOINPUTSTREAM_READ_CALLBACK_FUNC Read;
    PAUDIOINPUTSTREAM_CLOSE_CALLBACK_FUNC Close;
} SpeechApi_AudioInputStream;

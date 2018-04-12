//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_AudioInputStream.h: Public API declarations for AudioInputStream C++ base class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {

typedef struct tAudioInputStreamFormat
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

class AudioInputStream
{
public:

    virtual ~AudioInputStream() {}

    virtual unsigned short GetFormat(AudioInputStreamFormat* pformat, unsigned short cbFormat) = 0;
    virtual unsigned int Read(char* dataBuffer, unsigned int size) = 0;
    virtual void Close() = 0;

protected:

    AudioInputStream() {};


private:

    AudioInputStream(const AudioInputStream&) = delete;
    AudioInputStream(const AudioInputStream&&) = delete;

    AudioInputStream& operator=(const AudioInputStream&) = delete;
};


} // CARBON_NAMESPACE_ROOT

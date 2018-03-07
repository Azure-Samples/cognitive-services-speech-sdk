//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_audio_reader_impl.cpp: Implementation definitions for ISpxMockWavFileReaderImpl C++ class
//

#include "stdafx.h"
#include "mock_audio_reader_impl.h"


namespace CARBON_IMPL_NAMESPACE() {


uint16_t ISpxMockAudioReaderImpl::GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat)
{
    uint16_t cbFormatRequired = sizeof(WAVEFORMATEX);

    if (pformat != nullptr) // Calling with GetFormat(nullptr, ???) is valid; we don't copy bits, only return sizeof block required
    {
        WAVEFORMATEX format;
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = 1;
        format.nSamplesPerSec = 16000;
        format.nAvgBytesPerSec = 16000 * 2;
        format.nBlockAlign = 2;
        format.wBitsPerSample = 16;
        format.cbSize = 0;

        size_t cb = std::min(cbFormat, cbFormatRequired);
        std::memcpy(pformat, &format, cb);
    }

    return cbFormatRequired;
}

uint32_t ISpxMockAudioReaderImpl::Read(uint8_t* pbuffer, uint32_t cbBuffer)
{
    memset(pbuffer, 0, cbBuffer);
    return cbBuffer;
}


} // CARBON_IMPL_NAMESPACE

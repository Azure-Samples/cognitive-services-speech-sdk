//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_audio_reader_impl.h: Implementation definitions for ISpxMockWavFileReaderImpl C++ class
//

#pragma once
#include "stdafx.h"


namespace CARBON_IMPL_NAMESPACE() {


class ISpxMockAudioReaderImpl : public ISpxAudioReader
{
public:

    ISpxMockAudioReaderImpl() = default;


    // --- ISpxAudioReader ---

    uint16_t GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat);
    uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer);
    void Close() { }


private:

    ISpxMockAudioReaderImpl(ISpxMockAudioReaderImpl&&) = delete;
    ISpxMockAudioReaderImpl(const ISpxMockAudioReaderImpl&) = delete;
    ISpxMockAudioReaderImpl& operator=(ISpxMockAudioReaderImpl&&) = delete;
    ISpxMockAudioReaderImpl& operator=(const ISpxMockAudioReaderImpl&) = delete;
};


} // CARBON_IMPL_NAMESPACE

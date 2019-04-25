//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_audio_reader_impl.h: Implementation definitions for ISpxMockWavFileReaderImpl C++ class
//

#pragma once
#include "stdafx.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class ISpxMockAudioReaderImpl : public ISpxAudioStream, public ISpxAudioStreamReader
{
public:

    ISpxMockAudioReaderImpl() = default;

    // --- ISpxAudioStreamCanBeThrottled ---
    void SetRealTimeThrottlePercentage(uint8_t percentage);
    uint8_t GetRealTimeThrottlePercentage();

    // --- ISpxAudioStreamReader ---
    uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat);
    uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer);
    void Close() { }


private:

    ISpxMockAudioReaderImpl(ISpxMockAudioReaderImpl&&) = delete;
    ISpxMockAudioReaderImpl(const ISpxMockAudioReaderImpl&) = delete;
    ISpxMockAudioReaderImpl& operator=(ISpxMockAudioReaderImpl&&) = delete;
    ISpxMockAudioReaderImpl& operator=(const ISpxMockAudioReaderImpl&) = delete;

    uint8_t m_simulateRealtimePercentage = 0;     // 0 == as fast as possible; 100 == real time; 200 == 2x slower than real time
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

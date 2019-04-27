//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// push_audio_input_stream.h: Implementation definitions for CSpxPushAudioInputStream C++ class
//

#pragma once
#include "stdafx.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include <queue>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

#define DATA_INFO_TIME_STAMP_KEY  "timestamp"
#define DATA_INFO_SPEAKER_ID_KEY "speakerid"

class CSpxPushAudioInputStream :
    public ISpxAudioStreamInitFormat,
    public ISpxAudioStream,
    public ISpxAudioStreamWriter,
    public ISpxAudioStreamReader
{
public:
    using DataInfo = std::map<std::string, std::string>;

    CSpxPushAudioInputStream();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamInitFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStream)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamWriter)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamReader)
    SPX_INTERFACE_MAP_END()

    // --- ISpxAudioStreamInitFormat ---
    void SetFormat(SPXWAVEFORMATEX* format) override;

    // --- ISpxAudioStreamWriter ---
    void Write(uint8_t* buffer, uint32_t size) override;
    void SetProperty(PropertyId propertyId, const SPXSTRING& value) override;
    void SetProperty(const SPXSTRING& name, const SPXSTRING& value) override;

    // --- ISpxAudioStreamReader ---
    uint16_t GetFormat(SPXWAVEFORMATEX* format, uint16_t formatSize) override;
    uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer) override;
    SPXSTRING GetProperty(PropertyId propertyId) override;
    void Close() override { }

private:

    DISABLE_COPY_AND_MOVE(CSpxPushAudioInputStream);

    void WriteBuffer(uint8_t* buffer, uint32_t size);
    bool WaitForMoreData();

    void SignalEndOfStream();

    std::shared_ptr<SPXWAVEFORMATEX> m_format;

    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::queue<std::tuple<SpxSharedAudioBuffer_Type, uint32_t, DataInfo>> m_audioQueue;
    std::shared_ptr<uint8_t> m_buffer;
    uint32_t m_bytesInBuffer;
    uint8_t* m_ptrIntoBuffer;
    uint32_t m_bytesLeftInBuffer;

    DataInfo m_dataInfo;
    DataInfo m_dataInfoInRead;

    bool m_endOfStream;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

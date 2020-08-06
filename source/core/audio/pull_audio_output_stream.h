//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// pull_audio_output_stream.h: Implementation definitions for CSpxPullAudioOutputStream C++ class
//

#pragma once
#include "stdafx.h"
#include "null_audio_output.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include <queue>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxPullAudioOutputStream :
    public ISpxAudioOutputReader,
    public ISpxServiceProvider,
    public CSpxNullAudioOutput
{
public:

    CSpxPullAudioOutputStream();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamInitFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStream)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutputFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutputInitFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutput)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutputReader)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
    SPX_INTERFACE_MAP_END()

    // --- ISpxAudioOutput ---
    virtual uint32_t Write(uint8_t* buffer, uint32_t size) override;
    virtual void Close() override;
    void ClearUnread() override;

    // --- ISpxAudioOutputReader ---
    virtual uint32_t Read(uint8_t* buffer, uint32_t bufferSize) override;
    virtual uint32_t AvailableSize() override;

    // --- ISpxServiceProvider ---
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxAudioStream)
    SPX_SERVICE_MAP_END()


protected:

    bool WaitForMoreData(size_t minSize);

    void SignalEndOfWriting();

    uint32_t m_inventorySize = 0;

    std::atomic<bool> m_writingEnded { false };

    std::mutex m_mutex;
    std::condition_variable m_cv;

private:

    DISABLE_COPY_AND_MOVE(CSpxPullAudioOutputStream);

    uint32_t FillBuffer(uint8_t* buffer, uint32_t bufferSize);

    std::queue<std::pair<SpxSharedAudioBuffer_Type, uint32_t>> m_audioQueue;
    bool m_frontItemPartiallyRead = false;
    uint32_t m_frontItemRemainingSize = 0;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

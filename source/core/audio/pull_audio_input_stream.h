//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// pull_audio_input_stream.h: Implementation definitions for CSpxPullAudioInputStream C++ class
//

#pragma once
#include "stdafx.h"
#include "service_helpers.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxPullAudioInputStream : 
    public ISpxAudioStreamReaderInitCallbacks,
    public ISpxAudioStreamInitFormat,
    public ISpxAudioStream,
    public ISpxAudioStreamReader
{
public:

    CSpxPullAudioInputStream();
    ~CSpxPullAudioInputStream();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamInitFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamReaderInitCallbacks)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStream)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamReader)
    SPX_INTERFACE_MAP_END()

    // --- ISpxAudioStreamInitFormat ---
    void SetFormat(SPXWAVEFORMATEX* format) override;

    // --- ISpxAudioStreamReaderInitCallbacks ---
    void SetCallbacks(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback, ISpxAudioStreamReaderInitCallbacks::CloseCallbackFunction_Type closeCallback) override;
    void SetPropertyCallback(GetPropertyCallbackFunction_Type getPropertyCallBack) override;

    // --- ISpxAudioStreamReader ---
    uint16_t GetFormat(SPXWAVEFORMATEX* format, uint16_t formatSize) override;
    uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer) override;
    SPXSTRING GetProperty(PropertyId propertyId) override;

    void Close() override;

private:

    DISABLE_COPY_AND_MOVE(CSpxPullAudioInputStream);

    std::shared_ptr<SPXWAVEFORMATEX> m_format;

    ReadCallbackFunction_Type m_readCallback;
    CloseCallbackFunction_Type m_closeCallback;
    GetPropertyCallbackFunction_Type m_getPropertyCallback;
    static constexpr size_t m_maxPropertyLenInBytes = 1024;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

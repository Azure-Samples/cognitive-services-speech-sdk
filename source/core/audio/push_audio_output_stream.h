//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// push_audio_output_stream.h: Implementation definitions for CSpxPushAudioOutputStream C++ class
//

#pragma once
#include "stdafx.h"
#include "null_audio_output.h"
#include "interface_helpers.h"
#include "service_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxPushAudioOutputStream :
    public ISpxAudioStreamWriterInitCallbacks,
    public ISpxServiceProvider,
    public CSpxNullAudioOutput
{
public:

    CSpxPushAudioOutputStream();
    ~CSpxPushAudioOutputStream();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStream)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamInitFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutputFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutputInitFormat)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutput)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamWriterInitCallbacks)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
    SPX_INTERFACE_MAP_END()

    // --- ISpxAudioOutput ---
    uint32_t Write(uint8_t* buffer, uint32_t size) override;
    void Close() override;
    void ClearUnread() override {}

    // --- ISpxAudioStreamReaderInitCallbacks ---
    void SetCallbacks(ISpxAudioStreamWriterInitCallbacks::WriteCallbackFunction_Type writeCallback, ISpxAudioStreamWriterInitCallbacks::CloseCallbackFunction_Type closeCallback) override;

    // --- ISpxServiceProvider ---
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxAudioStream)
    SPX_SERVICE_MAP_END()

private:

    DISABLE_COPY_AND_MOVE(CSpxPushAudioOutputStream);

    WriteCallbackFunction_Type m_writeCallback;
    CloseCallbackFunction_Type m_closeCallback;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

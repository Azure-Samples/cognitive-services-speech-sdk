//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// opus_decoder.h: Implementation declarations for OpusDecoder C++ class
//

#pragma once
#include "base_gstreamer.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class OpusDecoder : public BaseGstreamer
{
public:
    OpusDecoder(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback, BaseGstreamer::BufferType buffer, uint16_t bitsPerSample, uint16_t numChannels, uint32_t sampleRate);
    virtual ~OpusDecoder();

private:
    static void PadAddedHandler(GstElement *src, GstPad *new_pad, OpusDecoder *data);
    void ThrowAfterCleanLocal(bool cond, uint32_t errCode, const char* pszFormat);
    GstElement *m_oggDemux = nullptr;
    GstElement *m_opusParser = nullptr;
    GstElement *m_opusDecoder = nullptr;
    GstElement *m_audioConvert = nullptr;
    GstElement *m_audioResampler = nullptr;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl

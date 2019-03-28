//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mpeg_decoder.h: Implementation declarations for MpegDecoder C++ class
//

#pragma once
#include "base_gstreamer.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class MpegDecoder : public BaseGstreamer
{
public:
    MpegDecoder(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback);
    virtual ~MpegDecoder();

private:
    GstElement *m_mpegAudioParse = nullptr;
    GstElement *m_mpeg123AudioDecode = nullptr;
    GstElement *m_audioConvert = nullptr;
    GstElement *m_audioResample = nullptr;
    void ThrowAfterCleanLocal(bool cond, uint32_t errCode, const char* pszFormat);
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

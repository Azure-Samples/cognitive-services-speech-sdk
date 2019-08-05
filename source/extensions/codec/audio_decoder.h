//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_decoder.h: Implementation declarations for AudioDecoder C++ class
//

#pragma once
#include "base_gstreamer.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class AudioDecoder : public BaseGstreamer
{
public:
    AudioDecoder(ISpxAudioStreamReaderInitCallbacks::ReadCallbackFunction_Type readCallback, CodecsTypeInternal codecType);
    virtual ~AudioDecoder();

private:
    std::vector<GstElement*> m_gstElementQueue;
    bool m_isSinkInQueue = false;
    void ThrowAfterCleanLocal(bool cond, uint32_t errCode, const char* pszFormat);
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

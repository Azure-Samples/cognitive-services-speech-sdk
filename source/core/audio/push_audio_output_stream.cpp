//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// push_audio_output_stream.cpp: Implementation definitions for CSpxPushAudioOutputStream C++ class
//

#include "stdafx.h"
#include "push_audio_output_stream.h"
#include <chrono>
#include <cstring>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxPushAudioOutputStream::CSpxPushAudioOutputStream()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxPushAudioOutputStream::~CSpxPushAudioOutputStream()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

void CSpxPushAudioOutputStream::SetCallbacks(WriteCallbackFunction_Type writeCallback, CloseCallbackFunction_Type closeCallback)
{
    m_writeCallback = writeCallback;
    m_closeCallback = closeCallback;
}

uint32_t CSpxPushAudioOutputStream::Write(uint8_t* buffer, uint32_t size)
{
    return m_writeCallback(buffer, size);
}

void CSpxPushAudioOutputStream::Close()
{
    m_closeCallback();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

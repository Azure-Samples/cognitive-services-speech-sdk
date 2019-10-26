//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_processor_write_to_audio_source_buffer.cpp: Implementation definitions for CSpxAudioProcessorWriteToAudioSourceBuffer
//

// ROBCH: Introduced in AUDIO.V3

#include "stdafx.h"
#include "audio_processor_write_to_audio_source_buffer.h"
#include "service_helpers.h"
#include <property_id_2_name_map.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxAudioProcessorWriteToAudioSourceBuffer::~CSpxAudioProcessorWriteToAudioSourceBuffer()
{
    SPX_DBG_ASSERT(m_format == nullptr);
    SPX_DBG_ASSERT(m_bufferData == nullptr);
    SPX_DBG_ASSERT(m_bufferProperties == nullptr);
    SPX_DBG_ASSERT(m_notifySource == nullptr);
    SPX_DBG_ASSERT(m_notifyTarget == nullptr);
}

void CSpxAudioProcessorWriteToAudioSourceBuffer::Term()
{
    SPX_DBG_ASSERT(m_format == nullptr);
    SPX_DBG_ASSERT(m_bufferData == nullptr);
    SPX_DBG_ASSERT(m_bufferProperties == nullptr);
    SPX_DBG_ASSERT(m_notifySource == nullptr);
    SPX_DBG_ASSERT(m_notifyTarget == nullptr);
}

void CSpxAudioProcessorWriteToAudioSourceBuffer::SetFormat(const SPXWAVEFORMATEX* format)
{
    AudioProcessor_Base::SetFormat(format);
    InitFromSite(format != nullptr);
    NotifyTarget();
}

void CSpxAudioProcessorWriteToAudioSourceBuffer::ProcessAudio(const DataChunkPtr& audioChunk)
{
    AudioProcessor_Base::ProcessAudio(audioChunk);

    SetProperty(PropertyId::DataBuffer_TimeStamp, audioChunk->capturedTime);
    SetProperty(PropertyId::DataBuffer_UserId, audioChunk->userId);

    m_bufferData->Write(audioChunk->data.get(), audioChunk->size);
    NotifyTarget();
}

void CSpxAudioProcessorWriteToAudioSourceBuffer::InitFromSite(bool init)
{
    if (!init && m_bufferData != nullptr)
    {
        m_bufferData->Write(nullptr, 0);
    }

    InitNotifyTargetFromSite(init);
    InitBufferDataAndPropertiesFromSite(init);
}

void CSpxAudioProcessorWriteToAudioSourceBuffer::InitNotifyTargetFromSite(bool init)
{
    m_notifyTarget = init ? SpxQueryService<ISpxAudioProcessorNotifyMe>(GetSite()) : nullptr;
    m_notifySource = init ? SpxSharedPtrFromThis<ISpxAudioProcessor>(this) : nullptr;

    SPX_IFFALSE(init, SPX_DBG_ASSERT(m_notifyTarget == nullptr));
    SPX_IFFALSE(init, SPX_DBG_ASSERT(m_notifySource == nullptr));
}

void CSpxAudioProcessorWriteToAudioSourceBuffer::InitBufferDataAndPropertiesFromSite(bool init)
{
    m_bufferData = init ? SpxQueryService<ISpxAudioSourceBufferDataWriter>(GetSite(), "AudioSourceBufferData") : nullptr;
    m_bufferProperties = init ? SpxQueryService<ISpxAudioSourceBufferProperties>(GetSite(), "AudioSourceBufferProperties") : nullptr;

    SPX_IFFALSE(init, SPX_DBG_ASSERT(m_bufferData == nullptr));
    SPX_IFFALSE(init, SPX_DBG_ASSERT(m_bufferProperties == nullptr));
}

void CSpxAudioProcessorWriteToAudioSourceBuffer::SetProperty(PropertyId id, const std::string& value)
{
    SPX_IFTRUE_RETURN(value.empty());
    auto name = GetPropertyName(id);
    m_bufferProperties->SetBufferProperty(name, value.c_str());
}

void CSpxAudioProcessorWriteToAudioSourceBuffer::NotifyTarget()
{
    // TODO: Who needs to catch this info?
    if (m_notifyTarget != nullptr)
    {
        m_notifyTarget->NotifyMe(m_notifySource);
    }
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

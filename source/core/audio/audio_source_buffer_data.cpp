//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_buffer_data.cpp: Implementation definitions for CSpxAudioSourceBufferData
//

// ROBCH: Introduced in AUDIO.V3

#include "stdafx.h"
#include "audio_source_buffer_data.h"
#include "create_object_helpers.h"
#include "read_write_buffer_delegate_helper.h"
#include <string>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

#define DEFAULT_AUDIO_SOURCE_BUFFER_DATA_SIZE_SECONDS   ((uint64_t)60 * 5)

CSpxAudioSourceBufferData::CSpxAudioSourceBufferData()
{
    m_bytesDead = m_bytesRead = 0;
    SPX_DBG_ASSERT(m_ringBuffer.IsClear());
}

CSpxAudioSourceBufferData::~CSpxAudioSourceBufferData()
{
    // The buffer is shared between writers and readers, trying to call Term from either side breaks the
    // ref counting pattern especially since readers may linger owning the buffer longer.
    TermRingBuffer();
    SPX_DBG_ASSERT(m_ringBuffer.IsClear());
}

void CSpxAudioSourceBufferData::Init()
{
    EnsureInitRingBuffer();
}

void CSpxAudioSourceBufferData::Term()
{
    TermRingBuffer();
}

uint64_t CSpxAudioSourceBufferData::GetOffset()
{
    return m_bytesDead + m_bytesRead;
}

uint64_t CSpxAudioSourceBufferData::GetNewMultiReaderOffset()
{
    EnsureInitRingBuffer();
    auto writePos = m_ringBuffer.DelegateGetWritePos();
    return writePos;
}

uint32_t CSpxAudioSourceBufferData::Read(uint8_t* buffer, uint32_t size)
{
    size_t bytesRead = 0;

    EnsureInitRingBuffer();
    m_ringBuffer.DelegateRead(buffer, size, &bytesRead);

    m_bytesRead += bytesRead;

    return (uint32_t)bytesRead;
}

uint32_t CSpxAudioSourceBufferData::ReadAt(uint64_t offset, uint8_t* buffer, uint32_t size)
{
    size_t bytesRead;

    EnsureInitRingBuffer();
    m_ringBuffer.DelegateReadAtBytePos(offset, buffer, size, &bytesRead);

    return (uint32_t)bytesRead;
}

uint64_t CSpxAudioSourceBufferData::GetBytesDead()
{
    return m_bytesDead;
}

uint64_t CSpxAudioSourceBufferData::GetBytesRead()
{
    return m_bytesRead;
}

uint64_t CSpxAudioSourceBufferData::GetBytesReady()
{
    EnsureInitRingBuffer();
    auto writePos = m_ringBuffer.DelegateGetWritePos();
    auto readPos = m_ringBuffer.DelegateGetReadPos();

    return writePos - readPos;
}

uint64_t CSpxAudioSourceBufferData::GetBytesReadyMax()
{
    return UINT64_MAX;
}

void CSpxAudioSourceBufferData::Write(uint8_t* buffer, uint32_t size)
{
    EnsureInitRingBuffer();
    m_ringBuffer.DelegateWrite(buffer, size);
}

void CSpxAudioSourceBufferData::EnsureInitRingBuffer()
{
    SPX_IFTRUE_RETURN(!m_ringBuffer.IsClear());
    SPX_TRACE_VERBOSE("[%p]CSpxAudioSourceBufferData::EnsureInitRingBuffer - Init", (void*) this);
    // CSpxBlockingReadWriteRingBuffer does not have a site actually

    auto init = SpxCreateObjectWithSite<ISpxReadWriteBufferInit>("CSpxBlockingReadWriteRingBuffer", this);
    init->SetName("AudioSourceBufferData");
    init->AllowOverflow(GetAudioSourceBufferAllowOverflow());
    init->SetSize(GetAudioSourceBufferDataSize());
    init->SetInitPos(GetAudioSourceBufferDataInitPos());

    auto rwb = SpxQueryInterface<ISpxReadWriteBuffer>(init);
    m_ringBuffer.SetReadWriteBufferDelegate(rwb);
}

void CSpxAudioSourceBufferData::TermRingBuffer()
{
    SPX_IFTRUE_RETURN(m_ringBuffer.IsClear());

    auto ptr = m_ringBuffer.GetReadWriteBufferDelegate();
    m_ringBuffer.Zombie(true);
    m_ringBuffer.Clear();
    SpxTermAndClear(ptr);

    SPX_DBG_ASSERT(m_ringBuffer.IsClear());
}

size_t CSpxAudioSourceBufferData::GetAudioSourceBufferDataSize()
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto size = properties->GetStringValue("AudioSourceBufferDataSizeInBytes", std::to_string(GetDefaultAudioSourceBufferDataSize()).c_str());
    return (size_t)std::stoul(size.c_str());
}

bool CSpxAudioSourceBufferData::GetAudioSourceBufferAllowOverflow()
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto bufferAllowOverflow = properties->GetStringValue("AudioSourceBufferAllowOverflow", "false");
    return (PAL::stricmp(bufferAllowOverflow.c_str(), "true") == 0 || PAL::stricmp(bufferAllowOverflow.c_str(), "1") == 0);
}

uint64_t CSpxAudioSourceBufferData::GetDefaultAudioSourceBufferDataSize()
{
    // Assume 16Khz 16bit mono for now...
    return DEFAULT_AUDIO_SOURCE_BUFFER_DATA_SIZE_SECONDS * 16000 * 2;
}

uint64_t CSpxAudioSourceBufferData::GetAudioSourceBufferDataInitPos()
{
    return GetOffset();
}

void CSpxAudioSourceBufferData::InitDelegatePtr(std::shared_ptr<ISpxAudioSourceBufferProperties>& ptr)
{
    auto site = ISpxInterfaceBase::QueryInterface<ISpxGenericSite>();
    ptr = SpxCreateObjectWithSite<ISpxAudioSourceBufferProperties>("CSpxAudioSourceBufferProperties", site);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

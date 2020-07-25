//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "stdafx.h"

#include "buffer_data.h"

#include "create_object_helpers.h"
#include "read_write_buffer_delegate_helper.h"
#include <string>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

constexpr SizeType DEFAULT_AUDIO_SOURCE_BUFFER_DATA_SIZE_SECONDS = 60 * 5;

CSpxBufferData::CSpxBufferData()
{
    m_bytesDead = m_bytesRead = 0;
    SPX_DBG_ASSERT(m_ringBuffer.IsClear());
}

CSpxBufferData::~CSpxBufferData()
{
    // The buffer is shared between writers and readers, trying to call Term from either side breaks the
    // ref counting pattern especially since readers may linger owning the buffer longer.
    TermRingBuffer();
    SPX_DBG_ASSERT(m_ringBuffer.IsClear());
}

void CSpxBufferData::Init()
{
    EnsureInitRingBuffer();
}

void CSpxBufferData::Term()
{
    TermRingBuffer();
}

uint64_t CSpxBufferData::GetOffset()
{
    return m_bytesDead + m_bytesRead;
}

uint64_t CSpxBufferData::GetNewMultiReaderOffset()
{
    EnsureInitRingBuffer();
    auto writePos = m_ringBuffer.DelegateGetWritePos();
    return writePos;
}

uint32_t CSpxBufferData::Read(uint8_t* buffer, uint32_t size)
{
    size_t bytesRead = 0;

    EnsureInitRingBuffer();
    m_ringBuffer.DelegateRead(buffer, size, &bytesRead);

    m_bytesRead += bytesRead;

    return (uint32_t)bytesRead;
}

uint32_t CSpxBufferData::ReadAt(uint64_t offset, uint8_t* buffer, uint32_t size)
{
    size_t bytesRead;

    EnsureInitRingBuffer();
    m_ringBuffer.DelegateReadAtBytePos(offset, buffer, size, &bytesRead);

    return (uint32_t)bytesRead;
}

uint64_t CSpxBufferData::GetBytesDead()
{
    return m_bytesDead;
}

uint64_t CSpxBufferData::GetBytesRead()
{
    return m_bytesRead;
}

uint64_t CSpxBufferData::GetBytesReady()
{
    EnsureInitRingBuffer();
    auto writePos = m_ringBuffer.DelegateGetWritePos();
    auto readPos = m_ringBuffer.DelegateGetReadPos();

    return writePos - readPos;
}

uint64_t CSpxBufferData::GetBytesReadyMax()
{
    return std::numeric_limits<uint32_t>::max();
}

void CSpxBufferData::Write(uint8_t* buffer, uint32_t size)
{
    EnsureInitRingBuffer();
    m_ringBuffer.DelegateWrite(buffer, size);
}

void CSpxBufferData::EnsureInitRingBuffer()
{
    SPX_IFTRUE_RETURN(!m_ringBuffer.IsClear());
    SPX_TRACE_VERBOSE("[%p]CSpxBufferData::EnsureInitRingBuffer - Init", (void*) this);
    // CSpxBlockingReadWriteRingBuffer does not have a site actually

    auto init = SpxCreateObjectWithSite<ISpxReadWriteBufferInit>("CSpxBlockingReadWriteRingBuffer", this);
    init->SetName("BufferData");
    init->AllowOverflow(GetBufferAllowOverflow());
    init->SetSize(GetBufferDataSize());
    init->SetInitPos(GetBufferDataInitPos());

    auto rwb = SpxQueryInterface<ISpxReadWriteBuffer>(init);
    m_ringBuffer.SetDelegate(rwb);
}

void CSpxBufferData::TermRingBuffer()
{
    SPX_IFTRUE_RETURN(m_ringBuffer.IsClear());
    SpxTermAndClearDelegate(m_ringBuffer);
    SPX_DBG_ASSERT(m_ringBuffer.IsClear());
}

size_t CSpxBufferData::GetBufferDataSize()
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto size = properties->GetStringValue("BufferDataSizeInBytes", std::to_string(GetDefaultBufferDataSize()).c_str());
    return (size_t)std::stoul(size.c_str());
}

bool CSpxBufferData::GetBufferAllowOverflow()
{
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto bufferAllowOverflow = properties->GetStringValue("BufferAllowOverflow", "false");
    return (PAL::stricmp(bufferAllowOverflow.c_str(), "true") == 0 || PAL::stricmp(bufferAllowOverflow.c_str(), "1") == 0);
}

uint64_t CSpxBufferData::GetDefaultBufferDataSize()
{
    // Assume 16Khz 16bit mono for now...
    return DEFAULT_AUDIO_SOURCE_BUFFER_DATA_SIZE_SECONDS * 16000 * 2;
}

uint64_t CSpxBufferData::GetBufferDataInitPos()
{
    return GetOffset();
}

void CSpxBufferData::InitDelegatePtr(std::shared_ptr<ISpxBufferProperties>& ptr)
{
    auto site = ISpxInterfaceBase::QueryInterface<ISpxGenericSite>();
    ptr = SpxCreateObjectWithSite<ISpxBufferProperties>("CSpxBufferProperties", site);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

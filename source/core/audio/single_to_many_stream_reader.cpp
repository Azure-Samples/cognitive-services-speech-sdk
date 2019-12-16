//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// single_to_many_stream_reader.cpp: Implementation definitions for CSpxSingleToManyStreamReader C++ class
//

#include "stdafx.h"
#include "single_to_many_stream_reader.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxSingleToManyStreamReader::CSpxSingleToManyStreamReader(long id, SpxWAVEFORMATEX_Type sourceFormat)
    : m_id(id), m_sourceFormat{ sourceFormat }, m_streamOpened{ false }
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxSingleToManyStreamReader::~CSpxSingleToManyStreamReader()
{
    SPX_DBG_TRACE_INFO("CSpxSingleToManyStreamReader(%ld)::dtor", m_id);
}

void CSpxSingleToManyStreamReader::SetError(const std::string& error)
{
    m_lastError = error;
}

uint16_t CSpxSingleToManyStreamReader::GetFormat(SPXWAVEFORMATEX* pFormat, uint16_t cbFormat)
{
    uint16_t formatSizeRequired = sizeof(SPXWAVEFORMATEX) + m_sourceFormat->cbSize;

    if (pFormat != nullptr)
    {
        size_t size = std::min(cbFormat, formatSizeRequired);
        std::memcpy(pFormat, m_sourceFormat.get(), size);
    }

    return formatSizeRequired;
}

uint32_t CSpxSingleToManyStreamReader::Read(uint8_t* pBuffer, uint32_t cbBuffer)
{
    SPX_IFFALSE_THROW_HR(m_streamOpened, SPXERR_UNINITIALIZED);
    // SPX_DBG_TRACE_VERBOSE("CSpxSingleToManyStreamReader(%ld)::Read ... %u bytes", m_id, cbBuffer);

    auto bytesRead = m_bufferData->ReadAt(m_bufferOffset, pBuffer, cbBuffer);
    m_bufferOffset += bytesRead;
    if (IsDownstreamError())
    {
        ThrowRuntimeError(m_lastError);
    }

    return bytesRead;
}

void CSpxSingleToManyStreamReader::Close()
{
    m_lastError.clear();
    if (!m_streamOpened)
    {
        SPX_TRACE_ERROR("CSpxSingleToManyStreamReader(%ld)::Close - already closed", m_id);
        return;
    }

    SPX_DBG_TRACE_INFO("CSpxSingleToManyStreamReader(%ld)::Close ", m_id);

    ResetBufferData();

    auto site = GetSite();
    auto siteReaderAdapter = SpxQueryInterface<ISpxSingleToManyStreamReaderAdapterSite>(site);
    siteReaderAdapter->DisconnectClient(m_id);
    m_streamOpened = false;
}

void CSpxSingleToManyStreamReader::Init()
{
    if (m_streamOpened)
    {
        SPX_TRACE_ERROR("CSpxSingleToManyStreamReader(%ld)::Init already initialized", m_id);
        return;
    }

    auto site = GetSite();
    auto siteReader = SpxQueryInterface<ISpxSingleToManyStreamReaderAdapterSite>(site);

    // Make sure the adapter knows we are consumer again. 
    siteReader->ReconnectClient(m_id, SpxSharedPtrFromThis<ISpxAudioStreamReader>(this));
    InitBufferDataFromSite();

    m_streamOpened = true;
    SPX_DBG_TRACE_INFO("CSpxSingleToManyStreamReader(%ld)::Init", m_id);
}

void CSpxSingleToManyStreamReader::Term()
{
    SPX_DBG_TRACE_INFO("CSpxSingleToManyStreamReader(%ld)::Term", m_id);
}

void CSpxSingleToManyStreamReader::InitBufferDataFromSite()
{
    m_bufferData = SpxQueryService<ISpxAudioSourceBufferData>(GetSite(), "AudioSourceBufferData");
    SPX_DBG_ASSERT(m_bufferData != nullptr);

    m_bufferOffset = m_bufferData->GetNewMultiReaderOffset();
    SPX_DBG_TRACE_VERBOSE("CSpxSingleToManyStreamReader(%ld)::InitBufferDataFromSite - offset %" PRIu64 "", m_id, m_bufferOffset);
}

void CSpxSingleToManyStreamReader::ResetBufferData()
{
    m_bufferData.reset();
    m_bufferOffset = 0;
}

bool CSpxSingleToManyStreamReader::IsDownstreamError()
{
    return m_lastError.length() > 0;
}


}}}}

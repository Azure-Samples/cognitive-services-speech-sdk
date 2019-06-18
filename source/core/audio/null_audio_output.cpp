//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// null_audio_output.cpp: Implementation definitions for CSpxNullAudioOutput C++ class
//

#include "stdafx.h"
#include "null_audio_output.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxNullAudioOutput::CSpxNullAudioOutput()
{
}

CSpxNullAudioOutput::~CSpxNullAudioOutput()
{
}

uint32_t CSpxNullAudioOutput::Write(uint8_t* buffer, uint32_t size)
{
    UNUSED(buffer);
    return size;
}

void CSpxNullAudioOutput::WaitUntilDone()
{
}

void CSpxNullAudioOutput::Close()
{
}

uint16_t CSpxNullAudioOutput::GetFormat(SPXWAVEFORMATEX* formatBuffer, uint16_t formatSize)
{
    uint16_t formatSizeRequired = sizeof(SPXWAVEFORMATEX) + m_format->cbSize;
    SPX_DBG_TRACE_VERBOSE("CSpxNullAudioOuput::GetFormat is called formatBuffer is %s formatSize=%d", formatBuffer ? "not null" : "null", formatSize);

    if (formatBuffer != nullptr)
    {
        size_t size = std::min(formatSize, formatSizeRequired);
        memcpy(formatBuffer, m_format.get(), size);
    }

    return formatSizeRequired;
}

bool CSpxNullAudioOutput::HasHeader()
{
    return m_hasHeader;
}

std::string CSpxNullAudioOutput::GetFormatString()
{
    return m_formatString;
}

std::string CSpxNullAudioOutput::GetRawFormatString()
{
    return m_rawFormatString;
}

void CSpxNullAudioOutput::SetFormat(SPXWAVEFORMATEX* pformat)
{
    SPX_IFTRUE_THROW_HR(pformat == nullptr, SPXERR_INVALID_ARG);

    // Allocate the buffer for the format
    auto formatSize = sizeof(SPXWAVEFORMATEX) + pformat->cbSize;
    m_format = SpxAllocWAVEFORMATEX(formatSize);
    SPX_DBG_TRACE_VERBOSE("CSpxNullAudioOuput::SetFormat is called with format 0x%p", (void*)pformat);

    // Copy the format
    memcpy(m_format.get(), pformat, formatSize);
}

void CSpxNullAudioOutput::SetHeader(bool hasHeader)
{
    m_hasHeader = hasHeader;
}

void CSpxNullAudioOutput::SetFormatString(const std::string& formatString)
{
    m_formatString = formatString;
}

void CSpxNullAudioOutput::SetRawFormatString(const std::string& rawFormatString)
{
    m_rawFormatString = rawFormatString;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

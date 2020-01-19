//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// connection_message.cpp: Implementation definitions for CSpxConnectionMessage C++ class
//

#include "stdafx.h"
#include "connection_message.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

CSpxConnectionMessage::CSpxConnectionMessage()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxConnectionMessage::~CSpxConnectionMessage()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

const std::string& CSpxConnectionMessage::GetHeaders() const
{
    return m_headers;
}

const std::string& CSpxConnectionMessage::GetPath() const
{
    return m_path;
}

const uint8_t* CSpxConnectionMessage::GetBuffer() const
{
    return m_buffer.get();
}

uint32_t CSpxConnectionMessage::GetBufferSize() const
{
    return m_bufferSize;
}

bool CSpxConnectionMessage::IsBufferBinary() const
{
    return m_bufferIsBinary;
}

void CSpxConnectionMessage::Init(const std::string& headers, const std::string& path, const uint8_t* buffer, uint32_t bufferSize, bool bufferIsBinary)
{
    m_headers = headers;
    m_path = path;

    m_buffer = SpxAllocSharedUint8Buffer(bufferSize);
    memcpy(m_buffer.get(), buffer, bufferSize);

    m_bufferSize = bufferSize;
    m_bufferIsBinary = bufferIsBinary;

    SetStringValue("connection.message.type", m_bufferIsBinary ? "binary" : "text");
    SetStringValue("connection.message.path", m_path.c_str());
    if (!bufferIsBinary)
    {
        auto text = std::string((const char*)buffer, bufferSize); // ensure zero-terminated at bufferSize length
        SetStringValue("connection.message.text.message", text.c_str());
    }

    InitHeaderMap();
}

void CSpxConnectionMessage::InitHeaderMap()
{
    SPX_IFTRUE_RETURN(*m_headers.c_str() == '\0');

    auto pch = m_headers.c_str();

    while (*pch != '\0')
    {
        // skip line sentinels
        while (*pch == '\r' || *pch == '\n') pch++;
        if (*pch == '\0') break;

        // accumulate the name
        auto name = pch;
        while (*pch != ':' && *pch != '\r' && *pch != '\n' && *pch != '\0') pch++;
        auto nameSize = pch - name;

        // skip over the name/value separators
        if (*pch == ':') pch++;
        if (*pch == ' ') pch++;

        // accumulate the value
        auto value = pch;
        while (*pch != '\0' && *pch != '\r' && *pch != '\n') pch++;
        auto valueSize = pch - value;

        // store the name/value pair in the named properties collection
        if (nameSize > 0)
        {
            SetStringValue(std::string(name, nameSize).c_str(), std::string(value, valueSize).c_str());
        }
    }

}

} } } } // Microsoft::CognitiveServices::Speech::Impl

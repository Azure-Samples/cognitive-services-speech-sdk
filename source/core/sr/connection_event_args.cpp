//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// connection_event_args.h: Implementation declarations for CSpxConnectionEventArgs C++ class
//

#include "stdafx.h"
#include "connection_event_args.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxConnectionEventArgs::CSpxConnectionEventArgs()
{
}

const std::wstring& CSpxConnectionEventArgs::GetSessionId()
{
    SPX_IFTRUE_THROW_HR(m_sessionId.length() == 0, SPXERR_UNINITIALIZED);
    return m_sessionId;
}

void CSpxConnectionEventArgs::Init(const std::wstring& sessionId)
{
    SPX_IFTRUE_THROW_HR(m_sessionId.length() != 0, SPXERR_ALREADY_INITIALIZED);
    m_sessionId = sessionId;
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

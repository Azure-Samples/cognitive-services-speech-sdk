//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "stdafx.h"

#include "session2.h"
#include "guid_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxSession2::CSpxSession2() :
    m_sessionId(PAL::CreateGuidWithoutDashesUTF8())
{
}

CSpxSession2::~CSpxSession2()
{
}


void CSpxSession2::Init()
{
}

void CSpxSession2::Term()
{
}

const char* CSpxSession2::GetSessionId() const
{
    return m_sessionId.c_str();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

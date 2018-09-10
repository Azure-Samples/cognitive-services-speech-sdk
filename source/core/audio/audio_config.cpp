//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_config.cpp: Implementation definitions for CSpxAudioConfig C++ class
//

#include "stdafx.h"
#include "audio_config.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxAudioConfig::CSpxAudioConfig()
{
}

void CSpxAudioConfig::InitFromDefaultDevice()
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;
}

void CSpxAudioConfig::InitFromFile(const wchar_t* pszFileName)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;
    m_fileName = pszFileName;
}

void CSpxAudioConfig::InitFromStream(std::shared_ptr<ISpxAudioStream> stream)
{
    SPX_IFTRUE_THROW_HR(m_init, SPXERR_ALREADY_INITIALIZED);
    m_init = true;
    m_stream = stream;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// wav_file_pump.cpp: Implementation declarations for CSpxWavFilePump C++ class
//

#include "stdafx.h"
#include "ispxinterfaces.h"
#include "create_object_helpers.h"
#include "service_helpers.h"
#include "wav_file_pump.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxWavFilePump::CSpxWavFilePump()
{
}

void CSpxWavFilePump::Open(const wchar_t* pszFileName)
{
    EnsureFile(pszFileName);
    EnsurePump();
}

void CSpxWavFilePump::EnsureFile(const wchar_t* fileName)
{
    SPX_IFTRUE_THROW_HR(m_delegateToAudioFile != nullptr, SPXERR_ALREADY_INITIALIZED);

    // Create the reader...
    auto audioFile = SpxCreateObjectWithSite<ISpxAudioFile>("CSpxWavFileReader", GetSite());

    // Open the file... 
    audioFile->Open(fileName);

    // And ... We're finished
    m_delegateToAudioFile = audioFile;
}

void CSpxWavFilePump::EnsurePump()
{
    SPX_IFTRUE_THROW_HR(m_delegateToAudioFile == nullptr, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(m_delegateToAudioPump != nullptr, SPXERR_ALREADY_INITIALIZED);

    // Create the pump ... 
    auto pumpInit = SpxCreateObjectWithSite<ISpxAudioPumpReaderInit>("CSpxAudioPump", GetSite());

    // Set the reader...
    auto fileAsReader = SpxQueryInterface<ISpxAudioReader>(m_delegateToAudioFile);
    pumpInit->SetAudioReader(fileAsReader);

    // And ... We're finished
    m_delegateToAudioPump = SpxQueryInterface<ISpxAudioPump>(pumpInit);
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

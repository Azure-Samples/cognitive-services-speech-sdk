//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_stream_session.cpp: Implementation definitions for CSpxAudioStreamSession C++ class
//

#include "stdafx.h"
#include <iostream>
#include <istream>
#include <fstream>
#include <thread>
#include "audio_stream_session.h"
#include "audio_pump.h"
#include "wav_file_reader.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxAudioStreamSession::CSpxAudioStreamSession()
{
}

void CSpxAudioStreamSession::InitFromFile(const wchar_t* pszFileName)
{
    SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_fileName.length() > 0);
    m_fileName = pszFileName;

    // Open the file
    auto wavfile = std::make_shared<CSpxWavFileReader>();
    wavfile->Open(pszFileName);

    // Keep hold of the file
    auto pISpxAudioFile = std::dynamic_pointer_cast<ISpxAudioFile>(wavfile);
    m_audioFile = pISpxAudioFile;

    // ... and the reader
    auto pISpxAudioReader = std::dynamic_pointer_cast<ISpxAudioReader>(pISpxAudioFile);
    m_audioReader = pISpxAudioReader;

    // Create the pump
    auto pump = std::make_shared<CSpxAudioPump>();
    pump->SetAudioReader(pISpxAudioReader);

    // Keep hold of the pump
    auto pISpxAudioPump = std::dynamic_pointer_cast<ISpxAudioPump>(pump);
    m_audioPump = pISpxAudioPump;
}

void CSpxAudioStreamSession::SetFormat(WAVEFORMATEX* pformat)
{
    SPX_DBG_TRACE_VERBOSE_IF(pformat == nullptr, "%s - pformat == nullptr", __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE_IF(pformat != nullptr, "%s\n  wFormatTag:      %s\n  nChannels:       %d\n  nSamplesPerSec:  %d\n  nAvgBytesPerSec: %d\n  nBlockAlign:     %d\n  wBitsPerSample:  %d\n  cbSize:          %d",
        __FUNCTION__,
        pformat->wFormatTag == WAVE_FORMAT_PCM ? "PCM" : "non-PCM",
        pformat->nChannels,
        pformat->nSamplesPerSec,
        pformat->nAvgBytesPerSec,
        pformat->nBlockAlign,
        pformat->wBitsPerSample,
        pformat->cbSize);
}

void CSpxAudioStreamSession::ProcessAudio(AudioData_Type data, uint32_t size)
{
    SPX_DBG_TRACE_VERBOSE("%s - size=%d", __FUNCTION__, size);
}

void CSpxAudioStreamSession::StartRecognizing()
{
    Base_Type::StartRecognizing();

    auto ptr = (ISpxAudioProcessor*)this;
    auto pISpxAudioProcessor = std::dynamic_pointer_cast<ISpxAudioProcessor>(ptr->shared_from_this());
    m_audioPump->StartPump(pISpxAudioProcessor);
}

void CSpxAudioStreamSession::StopRecognizing()
{
    m_audioPump->StopPump();

    Base_Type::StopRecognizing();
}


}; // CARBON_IMPL_NAMESPACE()

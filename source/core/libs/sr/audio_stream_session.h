//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_stream_session.h: Implementation declarations for CSpxAudioStreamSession C++ class
//

#pragma once
#include <spxcore_common.h>
#include "session.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxAudioStreamSession : public CSpxSession, public ISpxAudioStreamSessionInit, public ISpxAudioProcessor
{
public:

    CSpxAudioStreamSession();

    // --- ISpxAudioStreamSessionInit
    
    void InitFromFile(const wchar_t* pszFileName) override;

    // --- ISpxAudioProcessor

    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;


protected:

    void StartRecognizing() override;
    void StopRecognizing() override;
    
    
private:

    using Base_Type = CSpxSession;

    CSpxAudioStreamSession(const CSpxAudioStreamSession&) = delete;
    CSpxAudioStreamSession(const CSpxAudioStreamSession&&) = delete;

    CSpxAudioStreamSession& operator=(const CSpxAudioStreamSession&) = delete;

    std::wstring m_fileName;
    std::shared_ptr<ISpxAudioFile> m_audioFile;

    std::shared_ptr<ISpxAudioPump> m_audioPump;
    std::shared_ptr<ISpxAudioReader> m_audioReader;
};


}; // CARBON_IMPL_NAMESPACE()

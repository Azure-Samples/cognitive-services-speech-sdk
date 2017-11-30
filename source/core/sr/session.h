//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// session.h: Implementation declarations for CSpxSession C++ base class
//

#pragma once
#include <atomic>
#include <future>
#include <memory>
#include <list>
#include <spxcore_common.h>
#include "asyncop.h"
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxSession : public ISpxSession
{
public:

    CSpxSession();
    virtual ~CSpxSession();

    std::wstring GetSessionId() const;

    void AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer);
    void RemoveRecognizer(ISpxRecognizer* precognzier);

    CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync();
    CSpxAsyncOp<void> StartContinuousRecognitionAsync();
    CSpxAsyncOp<void> StopContinuousRecognitionAsync();


public:

    virtual void StartRecognizing();
    virtual void StopRecognizing();


private:

    CSpxSession(const CSpxSession&) = delete;
    CSpxSession(const CSpxSession&&) = delete;

    CSpxSession& operator=(const CSpxSession&) = delete;

    std::wstring m_sessionId;
    std::list<std::weak_ptr<ISpxRecognizer>> m_recognizers;
};


class CSpxAudioSession : public CSpxSession, public ISpxAudioProcessor
{
public:

    CSpxAudioSession() = default;

    // --- ISpxAudioProcessor

    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;


protected:

    void StartRecognizing() override;
    void StopRecognizing() override;

    std::shared_ptr<ISpxAudioPump> m_audioPump;

private:

    using Base_Type = CSpxSession;

    CSpxAudioSession(const CSpxAudioSession&) = delete;
    CSpxAudioSession(const CSpxAudioSession&&) = delete;

    CSpxAudioSession& operator=(const CSpxAudioSession&) = delete;
};


}; // CARBON_IMPL_NAMESPACE()

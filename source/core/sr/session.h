//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// session.h: Implementation declarations for CSpxSession C++ base class
//

#pragma once
#include <future>
#include <memory>
#include <list>
#include "spxcore_common.h"
#include "asyncop.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxSession : public ISpxSession
{
public:

    CSpxSession();
    virtual ~CSpxSession();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxSession)
    SPX_INTERFACE_MAP_END()

    // --- ISpxSession

    const std::wstring& GetSessionId() const;

    void AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer);
    void RemoveRecognizer(ISpxRecognizer* recognizer);

    CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync() override;
    CSpxAsyncOp<void> StartContinuousRecognitionAsync() override;
    CSpxAsyncOp<void> StopContinuousRecognitionAsync() override;

    CSpxAsyncOp<void> StartKeywordRecognitionAsync(const std::wstring& keyword) override;
    CSpxAsyncOp<void> StopKeywordRecognitionAsync() override;


protected:

    enum RecognitionKind { Idle = 0, Keyword = 1, KwsSingleShot = 2, SingleShot = 3, Continuous = 4 };
    virtual CSpxAsyncOp<void> StartRecognitionAsync(RecognitionKind startKind, std::wstring keyword = L"");
    virtual CSpxAsyncOp<void> StopRecognitionAsync(RecognitionKind stopKind);

    virtual void StartRecognizing(RecognitionKind startKind, std::wstring keyword = L"");
    virtual void StopRecognizing(RecognitionKind stopKind);

    virtual std::shared_ptr<ISpxRecognitionResult> WaitForRecognition();
    virtual void WaitForRecognition_Complete(std::shared_ptr<ISpxRecognitionResult> result);

    virtual void FireSessionStartedEvent();
    virtual void FireSessionStoppedEvent();

    virtual void FireSpeechStartDetectedEvent();
    virtual void FireSpeechEndDetectedEvent();

    virtual void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result);
    virtual void EnsureFireResultEvent();

    std::mutex m_mutex;
    std::condition_variable m_cv;

    const int m_recoAsyncTimeout = 5;
    const int m_waitForDoneTimeout = 20;

    bool m_recoAsyncWaiting;
    std::shared_ptr<ISpxRecognitionResult> m_recoAsyncResult;


private:

    CSpxSession(const CSpxSession&) = delete;
    CSpxSession(const CSpxSession&&) = delete;

    CSpxSession& operator=(const CSpxSession&) = delete;

    const std::wstring m_sessionId;
    std::list<std::weak_ptr<ISpxRecognizer>> m_recognizers;

    enum SesssionEventType {SessionStart, SessionStop, SpeechStart, SpeechEnd};
    void FireSessionEvent(SesssionEventType sessionType);
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

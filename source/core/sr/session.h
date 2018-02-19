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
#include "spxcore_common.h"
#include "asyncop.h"
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {
    

class CSpxSession : public ISpxSession
{
public:

    CSpxSession();
    virtual ~CSpxSession();

    // --- ISpxSession

    const std::wstring& GetSessionId() const;

    void AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer);
    void RemoveRecognizer(ISpxRecognizer* recognizer);

    CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync();
    CSpxAsyncOp<void> StartContinuousRecognitionAsync();
    CSpxAsyncOp<void> StopContinuousRecognitionAsync();


protected:

    virtual void StartRecognizing();
    virtual void StopRecognizing();

    virtual std::shared_ptr<ISpxRecognitionResult> WaitForRecognition();
    virtual void WaitForRecognition_Complete(std::shared_ptr<ISpxRecognitionResult> result);

    virtual void FireSessionStartedEvent();
    virtual void FireSessionStoppedEvent();

    virtual void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result);
    virtual void EnsureFireResultEvent();

    std::mutex m_mutex;
    std::condition_variable m_cv;

    const int m_recoAsyncTimeout = 5;
    const int m_waitForDoneTimeout = 20;

    bool m_fRecoAsyncWaiting;
    std::shared_ptr<ISpxRecognitionResult> m_recoAsyncResult;


private:

    CSpxSession(const CSpxSession&) = delete;
    CSpxSession(const CSpxSession&&) = delete;

    CSpxSession& operator=(const CSpxSession&) = delete;

    const std::wstring m_sessionId;
    std::list<std::weak_ptr<ISpxRecognizer>> m_recognizers;
};


} // CARBON_IMPL_NAMESPACE

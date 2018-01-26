#pragma once
#include <atomic>
#include <future>
#include <memory>
#include "asyncop.h"
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxRecognizer : public ISpxRecognizer, public ISpxRecognizerEvents
{
public:

    CSpxRecognizer(std::shared_ptr<ISpxSession>& session);
    virtual ~CSpxRecognizer();

    // --- ISpxRecognizer

    bool IsEnabled() override;
    void Enable() override;
    void Disable() override;

    CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync() override;
    CSpxAsyncOp<void> StartContinuousRecognitionAsync() override;
    CSpxAsyncOp<void> StopContinuousRecognitionAsync() override;

    // --- ISpxRecognizerEvents

    void FireSessionStarted(const std::wstring& sessionId) override;
    void FireSessionStopped(const std::wstring& sessionId) override;

    void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) override;


protected:

    void OnIsEnabledChanged();


private:

    CSpxRecognizer(const CSpxRecognizer&) = delete;
    CSpxRecognizer(const CSpxRecognizer&&) = delete;

    CSpxRecognizer& operator=(const CSpxRecognizer&) = delete;

    std::shared_ptr<ISpxSession> m_defaultSession;
    std::atomic_bool m_fEnabled;
};


} // CARBON_IMPL_NAMESPACE()

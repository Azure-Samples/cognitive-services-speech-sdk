#pragma once
#include <atomic>
#include <future>
#include <memory>
#include "asyncop.h"
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxRecognizer : public ISpxRecognizer
{
public:

    CSpxRecognizer(std::shared_ptr<ISpxSession>& session);
    virtual ~CSpxRecognizer();

    bool IsEnabled();
    void Enable();
    void Disable();

    CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync();
    CSpxAsyncOp<void> StartContinuousRecognitionAsync();
    CSpxAsyncOp<void> StopContinuousRecognitionAsync();

protected:

    void OnIsEnabledChanged();

        
private:

    CSpxRecognizer(const CSpxRecognizer&) = delete;
    CSpxRecognizer(const CSpxRecognizer&&) = delete;

    CSpxRecognizer& operator=(const CSpxRecognizer&) = delete;

    std::shared_ptr<ISpxSession> m_defaultSession;
    std::atomic_bool m_fEnabled;
};


}; // CARBON_IMPL_NAMESPACE()

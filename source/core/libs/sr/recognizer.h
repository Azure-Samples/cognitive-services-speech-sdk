#pragma once
#include <atomic>
#include <future>
#include <memory>
#include "asyncop.h"
#include "recognition_result.h"

namespace CARBON_IMPL_NAMESPACE() {


class CSpxRecognizer
{
public:

    CSpxRecognizer();
    CSpxRecognizer(const std::wstring& language);
    virtual ~CSpxRecognizer();

    bool IsEnabled();
    void Enable();
    void Disable();

    CSpxAsyncOp<std::shared_ptr<CSpxRecognitionResult>> RecognizeAsync();
    CSpxAsyncOp<void> StartContinuousRecognitionAsync();
    CSpxAsyncOp<void> StopContinuousRecognitionAsync();

protected:

    void OnIsEnabledChanged();

        
private:

    CSpxRecognizer(const CSpxRecognizer&) = delete;
    CSpxRecognizer(const CSpxRecognizer&&) = delete;

    CSpxRecognizer& operator=(const CSpxRecognizer&) = delete;

    std::atomic_bool m_fEnabled;
};


}; // CARBON_IMPL_NAMESPACE()

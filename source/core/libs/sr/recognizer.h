#pragma once
#include <future>
#include <memory>
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
   
    std::future<std::shared_ptr<CSpxRecognitionResult>> RecognizeAsync();
    std::future<void> StartContinuousRecognitionAsync();
    std::future<void> StopContinuousRecognitionAsync();


private:

    CSpxRecognizer(const CSpxRecognizer&) = delete;
    CSpxRecognizer(const CSpxRecognizer&&) = delete;

    CSpxRecognizer& operator=(const CSpxRecognizer&) = delete;
};


}; // CARBON_IMPL_NAMESPACE()

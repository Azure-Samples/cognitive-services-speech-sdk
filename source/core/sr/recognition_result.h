#pragma once
#include <string>
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {

    
class CSpxRecognitionResult : public ISpxRecognitionResult
{
public:

    CSpxRecognitionResult();
    virtual ~CSpxRecognitionResult();

    std::wstring GetResultId();
    std::wstring GetText();

    enum class Reason GetReason();

    // TODO: RobCh: Payload


private:

    CSpxRecognitionResult(const CSpxRecognitionResult&) = delete;
    CSpxRecognitionResult(const CSpxRecognitionResult&&) = delete;

    CSpxRecognitionResult& operator=(const CSpxRecognitionResult&) = delete;
};


}; // CARBON_IMPL_NAMESPACE()

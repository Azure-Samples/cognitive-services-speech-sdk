#pragma once
#include <string>


namespace CARBON_IMPL_NAMESPACE() {

    
enum class Reason { Recognized, NoMatch, Canceled, OtherRecognizer };
   

class CSpxRecognitionResult
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

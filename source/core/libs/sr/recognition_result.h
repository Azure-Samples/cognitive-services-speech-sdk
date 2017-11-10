#pragma once
#include <string>


namespace CARBON_IMPL_NAMESPACE() {


class CSpxRecognitionResult
{
public:

    CSpxRecognitionResult();
    virtual ~CSpxRecognitionResult();

    std::wstring GetResultId();
    std::wstring GetText();

    // TODO: RobCh: Reason
    // TODO: RobCh: Payload


private:

    CSpxRecognitionResult(const CSpxRecognitionResult&) = delete;
    CSpxRecognitionResult(const CSpxRecognitionResult&&) = delete;

    CSpxRecognitionResult& operator=(const CSpxRecognitionResult&) = delete;
};


}; // CARBON_IMPL_NAMESPACE()

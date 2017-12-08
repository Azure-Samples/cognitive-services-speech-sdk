#pragma once
#include <string>
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {

    
class CSpxRecognitionResult : public ISpxRecognitionResult
{
public:

    struct NoMatch_Type {};
    constexpr static NoMatch_Type NoMatch {};

    struct IntermediateResult_Type {};
    constexpr static IntermediateResult_Type IntermediateResult {};

    CSpxRecognitionResult(const wchar_t* resultId, const wchar_t* text);
    CSpxRecognitionResult(const wchar_t* resultId, const wchar_t* text, IntermediateResult_Type);
    CSpxRecognitionResult(NoMatch_Type);
    virtual ~CSpxRecognitionResult();

    // --- ISpxRecognitionResult

    std::wstring GetResultId();
    std::wstring GetText();
    enum class Reason GetReason();

    // TODO: RobCh: Payload


private:

    CSpxRecognitionResult(const CSpxRecognitionResult&) = delete;
    CSpxRecognitionResult(const CSpxRecognitionResult&&) = delete;

    CSpxRecognitionResult& operator=(const CSpxRecognitionResult&) = delete;

    std::wstring m_resultId;
    std::wstring m_text;
    enum class Reason m_reason;
};


}; // CARBON_IMPL_NAMESPACE()

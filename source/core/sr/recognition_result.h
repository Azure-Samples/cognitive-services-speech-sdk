#pragma once
#include <string>
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {

    
class CSpxRecognitionResult :
    public ISpxRecognitionResult,
    public ISpxRecognitionResultInit
{
public:

    CSpxRecognitionResult();
    virtual ~CSpxRecognitionResult();

    // --- ISpxRecognitionResult

    std::wstring GetResultId();
    std::wstring GetText();
    enum Reason GetReason();

    // --- ISpxRecognitionResultInit

    void InitIntermediateResult(const wchar_t* resultId, const wchar_t* text) override;
    void InitFinalResult(const wchar_t* resultId, const wchar_t* text) override;
    void InitNoMatch() override;


private:

    CSpxRecognitionResult(const CSpxRecognitionResult&) = delete;
    CSpxRecognitionResult(const CSpxRecognitionResult&&) = delete;

    CSpxRecognitionResult& operator=(const CSpxRecognitionResult&) = delete;

    std::wstring m_resultId;
    std::wstring m_text;
    enum Reason m_reason;
};


} // CARBON_IMPL_NAMESPACE

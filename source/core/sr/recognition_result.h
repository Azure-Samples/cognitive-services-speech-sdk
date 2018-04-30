#pragma once
#include <string>
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "named_properties_impl.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxRecognitionResult :
    public ISpxRecognitionResult,
    public ISpxRecognitionResultInit,
    public ISpxIntentRecognitionResult,
    public ISpxIntentRecognitionResultInit,
    public ISpxTranslationTextResult,
    public ISpxTranslationTextResultInit,
    public ISpxTranslationSynthesisResult,
    public ISpxTranslationSynthesisResultInit,
    public ISpxNamedPropertiesImpl
{
public:

    CSpxRecognitionResult();
    virtual ~CSpxRecognitionResult();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognitionResult)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognitionResultInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxIntentRecognitionResult)
        SPX_INTERFACE_MAP_ENTRY(ISpxIntentRecognitionResultInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxTranslationTextResult)
        SPX_INTERFACE_MAP_ENTRY(ISpxTranslationTextResultInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxTranslationSynthesisResult)
        SPX_INTERFACE_MAP_ENTRY(ISpxTranslationSynthesisResultInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
    SPX_INTERFACE_MAP_END()

    // --- ISpxRecognitionResult ---
    std::wstring GetResultId() override;
    std::wstring GetText() override;
    Reason GetReason() override;
    ResultType GetType() override;

    // --- ISpxRecognitionResultInit ---
    void InitIntermediateResult(const wchar_t* resultId, const wchar_t* text, ResultType type) override;
    void InitFinalResult(const wchar_t* resultId, const wchar_t* text, ResultType type) override;
    void InitNoMatch(ResultType type) override;
    void InitError(const wchar_t* text, ResultType type) override;

    // --- ISpxIntentRecognitionResult ---
    std::wstring GetIntentId() override;

    // --- ISpxIntentRecognitionResultInit ---
    void InitIntentResult(const wchar_t* intentId, const wchar_t* jsonPayload) override;

    // -- ISpxTranslationTextResult ---
    TranslationTextStatus GetTextStatus() const override;
    const std::wstring& GetTextFailureReason() const override;
    const std::map<std::wstring, std::wstring>& GetTranslationText() override;

    // -- ISpxTranslationTextResulInit --
    void InitTranslationTextResult(TranslationTextStatus status, const std::map<std::wstring, std::wstring>& translations, const std::wstring& failureReason) override;

    // -- ISpxTranslationSynthesisResult ---
    const uint8_t* GetAudio() const override;
    TranslationSynthesisStatus GetSynthesisStatus() override;
    const std::wstring& GetSynthesisFailureReason() override;
    size_t GetLength() const override;

    // ISpxTranslationSynthesisResultInit
    void InitTranslationSynthesisResult(TranslationSynthesisStatus status, const uint8_t* audioData, size_t audioLength, const std::wstring& failureReason) override;

private:

    CSpxRecognitionResult(const CSpxRecognitionResult&) = delete;
    CSpxRecognitionResult(const CSpxRecognitionResult&&) = delete;

    CSpxRecognitionResult& operator=(const CSpxRecognitionResult&) = delete;

    std::wstring m_resultId;
    std::wstring m_text;
    Reason m_reason;
    ResultType m_type;

    std::wstring m_intentId;

    std::map<std::wstring, std::wstring> m_translations;
    TranslationTextStatus m_translationTextStatus;
    std::wstring m_translationTextFailureReason;

    TranslationSynthesisStatus m_translationSynthesisStatus;
    std::wstring m_translationSynthesisFailureReason;
    const uint8_t* m_audioBuffer;
    size_t m_audioLength;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

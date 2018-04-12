#pragma once
#include <string>
#include "ispxinterfaces.h"
#include "named_properties_impl.h"


namespace CARBON_IMPL_NAMESPACE() {

    
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

    // --- ISpxRecognitionResult ---
    std::wstring GetResultId() override;
    std::wstring GetText() override;
    enum Reason GetReason() override;
    enum ResultType GetType() override;

    // --- ISpxRecognitionResultInit ---
    void InitIntermediateResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type) override;
    void InitFinalResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type) override;
    void InitNoMatch(enum ResultType type) override;
    void InitError(const wchar_t* text) override;

    // --- ISpxIntentRecognitionResult ---
    std::wstring GetIntentId() override;

    // --- ISpxIntentRecognitionResultInit ---
    void InitIntentResult(const wchar_t* intentId, const wchar_t* jsonPayload) override;

    // -- ISpxTranslationTextResult ---
    std::wstring GetTranslationText() override;

    // Todo: check whether we need return a vector of wstring for multiple languages.
    std::wstring GetSourceLanguage() override;
    std::wstring GetTargetLanguage() override;

    // -- ISpxTranslationTextResulInit --
    void InitTranslationTextResult(const std::wstring& sourceLangauge, const std::wstring& targetLanguage, const std::wstring& translatedText) override;

    // -- ISpxTranslationSynthesisResult ---
    // Todo: check we need to include text that represents the audio data.
    const std::shared_ptr<const uint8_t[]> GetAudioBuffer() const override;
    size_t GetAudioLength() const override;
    const std::wstring GetAudioText() const override;

    // ISpxTranslationSynthesisResultInit
    void InitTranslationSynthesisResult(const std::shared_ptr<const uint8_t[]> audioData, size_t audioLength, const std::wstring& text) override;

private:

    CSpxRecognitionResult(const CSpxRecognitionResult&) = delete;
    CSpxRecognitionResult(const CSpxRecognitionResult&&) = delete;

    CSpxRecognitionResult& operator=(const CSpxRecognitionResult&) = delete;

    std::wstring m_resultId;
    std::wstring m_text;
    enum Reason m_reason;
    enum ResultType m_type;

    std::wstring m_intentId;

    std::wstring m_sourceLanguage;
    std::wstring m_targetLanguage;
    std::wstring m_translationText;

    std::shared_ptr<const uint8_t[]> m_audioBuffer;
    size_t m_audioLength;
    std::wstring m_audioText;
};


} // CARBON_IMPL_NAMESPACE

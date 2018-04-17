#pragma once
#include <string>
#include "ispxinterfaces.h"
#include "interface_helpers.h"
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
    const std::unordered_map<std::wstring, std::wstring>& GetTranslationText() override;

    // -- ISpxTranslationTextResulInit --
    void InitTranslationTextResult(ISpxTranslationStatus status, const std::unordered_map<std::wstring, std::wstring>& translations) override;

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

    std::unordered_map<std::wstring, std::wstring> m_translations;
    ISpxTranslationStatus m_translationStatus;

    std::shared_ptr<const uint8_t[]> m_audioBuffer;
    size_t m_audioLength;
    std::wstring m_audioText;
};


} // CARBON_IMPL_NAMESPACE

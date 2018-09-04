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
    uint64_t GetOffset() const override { return m_offset; }
    uint64_t GetDuration() const override { return m_duration; }
    void SetOffset(uint64_t offset) override { m_offset = offset; }

    // --- ISpxRecognitionResultInit ---
    void InitIntermediateResult(const wchar_t* resultId, const wchar_t* text, ResultType type, uint64_t offset, uint64_t duration) override;
    void InitFinalResult(const wchar_t* resultId, Reason reason, const wchar_t* text, ResultType type, uint64_t offset, uint64_t duration) override;
    void InitError(const wchar_t* text, ResultType type) override;

    // --- ISpxIntentRecognitionResult ---
    std::wstring GetIntentId() override;

    // --- ISpxIntentRecognitionResultInit ---
    void InitIntentResult(const wchar_t* intentId, const wchar_t* jsonPayload) override;

    // -- ISpxTranslationTextResult ---
    TranslationStatusCode GetTranslationStatus() const override;
    const std::wstring& GetTranslationFailureReason() const override;
    const std::map<std::wstring, std::wstring>& GetTranslationText() override;

    // -- ISpxTranslationTextResulInit --
    void InitTranslationTextResult(TranslationStatusCode status, const std::map<std::wstring, std::wstring>& translations, const std::wstring& failureReason) override;

    // -- ISpxTranslationSynthesisResult ---
    const uint8_t* GetAudio() const override;
    SynthesisStatusCode GetSynthesisStatus() override;
    const std::wstring& GetSynthesisFailureReason() override;
    size_t GetLength() const override;

    // ISpxTranslationSynthesisResultInit
    void InitTranslationSynthesisResult(SynthesisStatusCode status, const uint8_t* audioData, size_t audioLength, const std::wstring& failureReason) override;

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
    TranslationStatusCode m_translationStatus;
    std::wstring m_translationFailureReason;

    SynthesisStatusCode m_synthesisStatus;
    std::wstring m_synthesisFailureReason;
    const uint8_t* m_audioBuffer;
    size_t m_audioLength{0};
    uint64_t m_offset{0};
    uint64_t m_duration{0};
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

#pragma once
#include <string>
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxRecognitionResult :
    public ISpxRecognitionResult,
    public ISpxRecognitionResultInit,
    public ISpxIntentRecognitionResult,
    public ISpxIntentRecognitionResultInit,
    public ISpxTranslationRecognitionResult,
    public ISpxTranslationRecognitionResultInit,
    public ISpxTranslationSynthesisResult,
    public ISpxTranslationSynthesisResultInit,
    public ISpxPropertyBagImpl
{
public:

    CSpxRecognitionResult();
    virtual ~CSpxRecognitionResult();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognitionResult)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognitionResultInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxIntentRecognitionResult)
        SPX_INTERFACE_MAP_ENTRY(ISpxIntentRecognitionResultInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxTranslationRecognitionResult)
        SPX_INTERFACE_MAP_ENTRY(ISpxTranslationRecognitionResultInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxTranslationSynthesisResult)
        SPX_INTERFACE_MAP_ENTRY(ISpxTranslationSynthesisResultInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
    SPX_INTERFACE_MAP_END()

    // --- ISpxRecognitionResult ---
    std::wstring GetResultId() override;
    std::wstring GetText() override;

    ResultReason GetReason() override;
    NoMatchReason GetNoMatchReason() override;
    CancellationReason GetCancellationReason() override;
    CancellationErrorCode GetCancellationErrorCode() override;

    uint64_t GetOffset() const override { return m_offset; }
    uint64_t GetDuration() const override { return m_duration; }
    void SetOffset(uint64_t offset) override { m_offset = offset; }

    // --- ISpxRecognitionResultInit ---
    void InitIntermediateResult(const wchar_t* resultId, const wchar_t* text, uint64_t offset, uint64_t duration) override;
    void InitFinalResult(const wchar_t* resultId, ResultReason reason, NoMatchReason noMatchReason, CancellationReason cancellation, CancellationErrorCode errorCode, const wchar_t* text, uint64_t offset, uint64_t duration) override;

    // --- ISpxIntentRecognitionResult ---
    std::wstring GetIntentId() override;

    // --- ISpxIntentRecognitionResultInit ---
    void InitIntentResult(const wchar_t* intentId, const wchar_t* jsonPayload) override;

    // -- ISpxTranslationRecognitionResult ---
    const std::map<std::wstring, std::wstring>& GetTranslationText() override;

    // -- ISpxTranslationRecognitionResultInit --
    void InitTranslationRecognitionResult(TranslationStatusCode status, const std::map<std::wstring, std::wstring>& translations, const std::wstring& failureReason) override;

    // -- ISpxTranslationSynthesisResult ---
    const uint8_t* GetAudio() const override;
    size_t GetLength() const override;

    // ISpxTranslationSynthesisResultInit
    void InitTranslationSynthesisResult(SynthesisStatusCode status, const uint8_t* audioData, size_t audioLength, const std::wstring& failureReason) override;

    // --- ISpxNamedProperties (overrides)
    void SetStringValue(const char* name, const char* value) override;

private:

    CSpxRecognitionResult(const CSpxRecognitionResult&) = delete;
    CSpxRecognitionResult(const CSpxRecognitionResult&&) = delete;

    CSpxRecognitionResult& operator=(const CSpxRecognitionResult&) = delete;

    void InitPropertiesFromJsonResult(const char* value);

    std::wstring m_resultId;
    std::wstring m_text;
    ResultReason m_reason;
    CancellationReason m_cancellationReason;
    CancellationErrorCode m_cancellationErrorCode;
    NoMatchReason m_noMatchReason;

    std::wstring m_intentId;

    std::map<std::wstring, std::wstring> m_translations;

    const uint8_t* m_audioBuffer;
    size_t m_audioLength{0};
    uint64_t m_offset{0};
    uint64_t m_duration{0};
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

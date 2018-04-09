#include "stdafx.h"
#include "recognition_result.h"
#include "guid_utils.h"
#include "named_properties_constants.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxRecognitionResult::CSpxRecognitionResult()
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxRecognitionResult::~CSpxRecognitionResult()
{
    SPX_DBG_TRACE_FUNCTION();
}

std::wstring CSpxRecognitionResult::GetResultId()
{
    return m_resultId;
}

std::wstring CSpxRecognitionResult::GetText()
{
    return m_text;
}

Reason CSpxRecognitionResult::GetReason()
{
    return m_reason;
}

ResultType CSpxRecognitionResult::GetType()
{
    return m_type;
}

void CSpxRecognitionResult::InitIntermediateResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::IntermediateResult;
    m_type = type;

    m_resultId = resultId == nullptr
        ? PAL::CreateGuid()
        : resultId;

    m_text = text;
}

void CSpxRecognitionResult::InitFinalResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::Recognized;
    m_type = type;

    m_resultId = resultId == nullptr
        ? PAL::CreateGuid()
        : resultId;

    m_text = text == nullptr
        ? L""
        : text;
}

void CSpxRecognitionResult::InitNoMatch(enum ResultType type)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::NoMatch;
    m_type = type;
}

void CSpxRecognitionResult::InitError(const wchar_t* text)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::Canceled;
    m_text = (text == nullptr) ? L"" : text;
}

std::wstring CSpxRecognitionResult::GetIntentId()
{
    return m_intentId;
}

void CSpxRecognitionResult::InitIntentResult(const wchar_t* intentId, const wchar_t* jsonPayload)
{
    m_intentId = intentId != nullptr
        ? intentId
        : L"";

    SetStringValue(g_RESULT_LuisJson, jsonPayload);
}

std::wstring CSpxRecognitionResult::GetTranslatedText()
{
    return m_translationText;
}

std::wstring CSpxRecognitionResult::GetSourceLanguage()
{
    return m_sourceLanguage;
}

std::wstring CSpxRecognitionResult::GetTargetLanguage()
{
    return m_targetLanguage;
}

void CSpxRecognitionResult::InitTranslationTextResult(const std::wstring& sourceLangauge, const std::wstring& targetLanguage, const std::wstring& translatedText)
{
    m_sourceLanguage = sourceLangauge;
    m_targetLanguage = targetLanguage;
    m_translationText = translatedText;
}

const std::shared_ptr<const uint8_t[]> CSpxRecognitionResult::GetAudioBuffer() const
{
    return m_audioBuffer;
}

size_t CSpxRecognitionResult::GetAudioLength() const
{
    return m_audioLength;
}

const std::wstring CSpxRecognitionResult::GetAudioText() const
{
    return m_audioText;
}


// ISpxTranslationSynthesisResultInit
void CSpxRecognitionResult::InitTranslationSynthesisResult(const std::shared_ptr<const uint8_t[]> audioData, size_t audioLength, const std::wstring& text)
{
    m_audioBuffer = audioData;
    m_audioLength = audioLength;
    m_audioText = text;
}


} // CARBON_IMPL_NAMESPACE

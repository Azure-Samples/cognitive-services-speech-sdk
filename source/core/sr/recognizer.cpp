#include "stdafx.h"
#include "recognizer.h"
#include <future>
#include "handle_table.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxRecognizer::CSpxRecognizer() :
    ISpxRecognizerEvents(nullptr, nullptr),
    m_fEnabled(true)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxRecognizer::~CSpxRecognizer()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    TermDefaultSession();
}

void CSpxRecognizer::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    EnsureDefaultSession();
}

void CSpxRecognizer::Term()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxRecognizer::SetStringValue(const char* name, const char* value)
{
    SetStringValueInProperties(name, value);
}

bool CSpxRecognizer::IsEnabled()
{
    return m_fEnabled;
}

void CSpxRecognizer::Enable()
{
    if (m_fEnabled.exchange(true) != true)
    {
        OnIsEnabledChanged();
    }
}

void CSpxRecognizer::Disable()
{
    if (m_fEnabled.exchange(false) != false)
    {
        OnIsEnabledChanged();
    }
}

CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> CSpxRecognizer::RecognizeAsync()
{
    const char* reco_mode = GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoMode);
    auto currentRecoMode = GetStringValueFromProperties(reco_mode, "");
    auto recoModeToSet = dynamic_cast<ISpxTranslationRecognizer *>(this) != nullptr
        ? GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoMode_Conversation)
        : GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoMode_Interactive);

    std::string recoMode = recoModeToSet ? recoModeToSet : "";
    if (currentRecoMode.empty())
    {
        SetStringValueInProperties(reco_mode, recoModeToSet);
    }
    else
    {
        // Since the mode is set during connection setup, no mode switch is allowed.
        SPX_IFTRUE_THROW_HR((currentRecoMode.compare(recoMode) != 0), SPXERR_SWITCH_MODE_NOT_ALLOWED);
    }

    return m_defaultSession->RecognizeAsync();
}

CSpxAsyncOp<void> CSpxRecognizer::StartContinuousRecognitionAsync()
{
    const char* reco_mode = GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoMode);
    auto currentRecoMode = GetStringValueFromProperties(reco_mode, "");
    if (currentRecoMode.empty())
    {
        SetStringValueInProperties(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoMode), GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoMode_Conversation));
    }
    else
    {
        // Since the mode is set during connection setup, no mode switch is allowed.
        SPX_IFTRUE_THROW_HR((currentRecoMode.compare(GetPropertyName(SpeechPropertyId::SpeechServiceConnection_RecoMode_Conversation)) != 0), SPXERR_SWITCH_MODE_NOT_ALLOWED);
    }
    return m_defaultSession->StartContinuousRecognitionAsync();
}

CSpxAsyncOp<void> CSpxRecognizer::StopContinuousRecognitionAsync()
{
    return m_defaultSession->StopContinuousRecognitionAsync();
}

CSpxAsyncOp<void> CSpxRecognizer::StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model)
{
    return m_defaultSession->StartKeywordRecognitionAsync(model);
}

CSpxAsyncOp<void> CSpxRecognizer::StopKeywordRecognitionAsync()
{
    return m_defaultSession->StopKeywordRecognitionAsync();
}

std::shared_ptr<ISpxSession> CSpxRecognizer::GetDefaultSession()
{
    EnsureDefaultSession();
    return m_defaultSession;
}

void CSpxRecognizer::FireSessionStarted(const std::wstring& sessionId)
{
    SPX_DBG_ASSERT(GetSite());
    auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
    auto sessionEvent = factory->CreateSessionEventArgs(sessionId);
    SessionStarted.Signal(sessionEvent);
}

void CSpxRecognizer::FireSessionStopped(const std::wstring& sessionId)
{
    SPX_DBG_ASSERT(GetSite());
    auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
    auto sessionEvent = factory->CreateSessionEventArgs(sessionId);
    SessionStopped.Signal(sessionEvent);
}

void CSpxRecognizer::FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset)
{   
    FireRecoEvent(&SpeechStartDetected, sessionId, nullptr, offset);
}

void CSpxRecognizer::FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset)
{
    FireRecoEvent(&SpeechEndDetected, sessionId, nullptr, offset);
}

void CSpxRecognizer::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    ISpxRecognizerEvents::RecoEvent_Type* pevent = nullptr;
    auto reason = result->GetReason();
    switch (reason)
    {
    case ResultReason::Canceled:
        pevent = &Canceled;
        break;

    case ResultReason::NoMatch:
    case ResultReason::RecognizedSpeech:
    case ResultReason::RecognizedIntent:
    case ResultReason::TranslatedSpeech:
        pevent = &FinalResult;
        SPX_DBG_TRACE_VERBOSE_IF(!pevent->IsConnected(), "%s: No FinalResult event signal connected!! nobody listening...", __FUNCTION__);
        break;

    case ResultReason::RecognizingSpeech:
    case ResultReason::RecognizingIntent:
    case ResultReason::TranslatingSpeech:
        pevent = &IntermediateResult;
        break;

    case ResultReason::SynthesizingAudio:
    case ResultReason::SynthesizingAudioComplete:
        pevent = &TranslationSynthesisResult;
        break;

    default:
        // This should be changed to throw exception. But currently it causes problem in lock.
        // Bug: https://msasg.visualstudio.com/Skyman/_workitems/edit/1314877
        SPX_DBG_ASSERT_WITH_MESSAGE(false, "The reason found in the result was unexpected.");
        break;
    }

    FireRecoEvent(pevent, sessionId, result);
}

void CSpxRecognizer::FireRecoEvent(ISpxRecognizerEvents::RecoEvent_Type* pevent, const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result, uint64_t offset)
{
    if (pevent != nullptr && pevent->IsConnected())
    {
        SPX_DBG_ASSERT(GetSite());
        auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
        auto recoEvent = (result != nullptr) 
            ? factory->CreateRecognitionEventArgs(sessionId, result) 
            : factory->CreateRecognitionEventArgs(sessionId, offset);
        pevent->Signal(recoEvent);
    }
}

void CSpxRecognizer::SetStringValueInProperties(const char* name, const char* value)
{
    // For now, we can only have one Recognizer per Session, so, we'll just pass this over to the default session.
    EnsureDefaultSession();
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
    SPX_IFTRUE_THROW_HR(namedProperties->HasStringValue(name), SPXERR_ALREADY_INITIALIZED); // throw if it's already been set
    namedProperties->SetStringValue(name, value);
}

std::string CSpxRecognizer::GetStringValueFromProperties(const char* name, const char* defaultValue)
{
    // For now, we can only have one Recognizer per Session, so, we'll just pass this over to the default session.
    EnsureDefaultSession();
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
    return namedProperties->GetStringValue(name, defaultValue);
}

void CSpxRecognizer::EnsureDefaultSession()
{
    if (m_defaultSession == nullptr)
    {
        SPX_DBG_ASSERT(GetSite());
        m_defaultSession = GetSite()->GetDefaultSession();
    }
}

void CSpxRecognizer::TermDefaultSession()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    if (m_defaultSession != nullptr)
    {
        m_defaultSession->RemoveRecognizer(this);
    }
    SpxTermAndClear(m_defaultSession);
}

void CSpxRecognizer::OnIsEnabledChanged()
{
    // no op currently
}

std::shared_ptr<ISpxNamedProperties> CSpxRecognizer::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

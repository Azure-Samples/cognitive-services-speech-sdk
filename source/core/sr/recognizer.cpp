#include "stdafx.h"
#include "recognizer.h"
#include <future>
#include "handle_table.h"
#include "service_helpers.h"
#include "create_object_helpers.h"


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

void CSpxRecognizer::SetStringValue(const wchar_t* name, const wchar_t* value)
{
    // Check to see if the caller is trying to set the CUSTOM SPEECH Model ID...
    if (wcscmp(name, g_SPEECH_ModelId) == 0)
    {
        SetStringValueInProperties(name, value);
    }
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
    auto currentRecoMode = GetStringValueFromProperties(g_SPEECH_RecoMode, L"");
    auto recoModeToSet = dynamic_cast<ISpxTranslationRecognizer *>(this) != nullptr ? g_SPEECH_RecoMode_Conversation : g_SPEECH_RecoMode_Interactive;

    if (currentRecoMode.empty())
    {
        SetStringValueInProperties(g_SPEECH_RecoMode, recoModeToSet);
    }
    else
    {
        // Since the mode is set during connection setup, no mode switch is allowed.
        SPX_IFTRUE_THROW_HR((currentRecoMode.compare(recoModeToSet) != 0), SPXERR_SWITCH_MODE_NOT_ALLOWED);
    }

    return m_defaultSession->RecognizeAsync();
}

CSpxAsyncOp<void> CSpxRecognizer::StartContinuousRecognitionAsync()
{
    auto currentRecoMode = GetStringValueFromProperties(g_SPEECH_RecoMode, L"");
    if (currentRecoMode.empty())
    {
        SetStringValueInProperties(g_SPEECH_RecoMode, g_SPEECH_RecoMode_Conversation);
    }
    else
    {
        // Since the mode is set during connection setup, no mode switch is allowed.
        SPX_IFTRUE_THROW_HR((currentRecoMode.compare(g_SPEECH_RecoMode_Conversation) != 0), SPXERR_SWITCH_MODE_NOT_ALLOWED);
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

    if (result->GetType() == ResultType::TranslationSynthesis)
    {
        pevent = &TranslationSynthesisResult;
    }
    else
    {
        // Speech recognition result, intent recognition result, or translation text result.
        auto reason = result->GetReason();
        switch (reason)
        {
        case Reason::Recognized:
        case Reason::InitialBabbleTimeout:
        case Reason::InitialSilenceTimeout:
        case Reason::NoMatch:
            pevent = &FinalResult;
            SPX_DBG_TRACE_VERBOSE_IF(!pevent->IsConnected(), "%s: No FinalResult event signal connected!! nobody listening...", __FUNCTION__);
            break;

        case Reason::IntermediateResult:
            pevent = &IntermediateResult;
            break;

        case Reason::Canceled:
            pevent = &Canceled;
            break;

        default:
            // This should be changed to throw exception. But currently it causes problem in lock.
            // Bug: https://msasg.visualstudio.com/Skyman/_workitems/edit/1314877
            SPX_DBG_ASSERT_WITH_MESSAGE(false, "The reason found in the result was unexpected.");
            break;
        }
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

void CSpxRecognizer::SetStringValueInProperties(const wchar_t* name, const wchar_t* value)
{
    // For now, we can only have one Recognizer per Session, so, we'll just pass this over to the default session.
    EnsureDefaultSession();
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
    SPX_IFTRUE_THROW_HR(namedProperties->HasStringValue(name), SPXERR_ALREADY_INITIALIZED); // throw if it's already been set
    namedProperties->SetStringValue(name, value);
}

std::wstring CSpxRecognizer::GetStringValueFromProperties(const wchar_t* name, const wchar_t* defaultValue) {
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

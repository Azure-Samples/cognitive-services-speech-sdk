#include "stdafx.h"
#include "recognizer.h"
#include <future>
#include "handle_table.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "named_properties_constants.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxRecognizer::CSpxRecognizer() :
    ISpxRecognizerEvents(nullptr, nullptr),
    m_fEnabled(true)
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxRecognizer::~CSpxRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
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
        // For now, we can only have one Recognizer per Session, so, we'll 
        // just pass this over to the default session...

        EnsureDefaultSession();

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
        SPX_IFTRUE_THROW_HR(namedProperties->HasStringValue(name), SPXERR_ALREADY_INITIALIZED); // throw if it's already been set

        namedProperties->SetStringValue(name, value);
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
    return m_defaultSession->RecognizeAsync();
}

CSpxAsyncOp<void> CSpxRecognizer::StartContinuousRecognitionAsync()
{
    return m_defaultSession->StartContinuousRecognitionAsync();
}

CSpxAsyncOp<void> CSpxRecognizer::StopContinuousRecognitionAsync()
{
    return m_defaultSession->StopContinuousRecognitionAsync();
}

CSpxAsyncOp<void> CSpxRecognizer::StartKeywordRecognitionAsync(const std::wstring& keyword)
{
    return m_defaultSession->StartKeywordRecognitionAsync(keyword);
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
            pevent = &FinalResult;
            SPX_DBG_TRACE_VERBOSE_IF(!pevent->IsConnected(), "%s: No FinalResult event signal connected!! nobody listening...", __FUNCTION__);
            break;

        case Reason::IntermediateResult:
            pevent = &IntermediateResult;
            break;

        case Reason::NoMatch:
            pevent = &NoMatch;
            break;

        case Reason::Canceled:
            pevent = &Canceled;
            break;

        case Reason::OtherRecognizer:
            SPX_THROW_HR(SPXERR_INVALID_ARG);
            break;

        default:
            SPX_DBG_ASSERT_WITH_MESSAGE(
                reason != Reason::Recognized &&
                reason != Reason::IntermediateResult &&
                reason != Reason::NoMatch &&
                reason != Reason::Canceled &&
                reason != Reason::OtherRecognizer,
                "The reason found in the result was unexpected.");
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
        auto recoEvent = (result != nullptr) ? factory->CreateRecognitionEventArgs(sessionId, result) : factory->CreateRecognitionEventArgsWithOffset(sessionId, offset);
        pevent->Signal(recoEvent);
    }
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

std::shared_ptr<ISpxNamedProperties> CSpxRecognizer::GetParentProperties()
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

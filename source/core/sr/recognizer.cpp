#include "stdafx.h"
#include "recognizer.h"
#include <future>
#include "handle_table.h"
#include "service_helpers.h"
#include "create_object_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


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
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    EnsureDefaultSession();
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

    auto handletable = CSpxSharedPtrHandleTableManager::Get<ISpxSessionEventArgs, SPXEVENTHANDLE>();
    auto hevent = handletable->TrackHandle(sessionEvent);

    SessionStarted.Signal(sessionEvent);
    handletable->StopTracking(hevent);
}

void CSpxRecognizer::FireSessionStopped(const std::wstring& sessionId)
{
    SPX_DBG_ASSERT(GetSite());
    auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
    auto sessionEvent = factory->CreateSessionEventArgs(sessionId);

    auto handletable = CSpxSharedPtrHandleTableManager::Get<ISpxSessionEventArgs, SPXEVENTHANDLE>();
    auto hevent = handletable->TrackHandle(sessionEvent);

    SessionStopped.Signal(sessionEvent);
    handletable->StopTracking(hevent);
}

void CSpxRecognizer::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    ISpxRecognizerEvents::RecoEvent_Type* pevent = nullptr;

    auto reason = result->GetReason();
    switch (reason)
    {
        case Reason::Recognized:
            pevent = &FinalResult;
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

    if (pevent != nullptr && pevent->IsConnected())
    {
        SPX_DBG_ASSERT(GetSite());
        auto factory = SpxQueryService<ISpxEventArgsFactory>(GetSite());
        auto recoEvent = factory->CreateRecognitionEventArgs(sessionId, result);

        auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
        auto hevent = eventhandles->TrackHandle(recoEvent);

        pevent->Signal(recoEvent);
        eventhandles->StopTracking(hevent);
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


} // CARBON_IMPL_NAMESPACE

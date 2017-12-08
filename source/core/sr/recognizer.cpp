#include "stdafx.h"
#include "recognizer.h"
#include <future>
#include "handle_table.h"
#include "recognition_event_args.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxRecognizer::CSpxRecognizer(std::shared_ptr<ISpxSession>& session) :
    ISpxRecognizerEvents(nullptr, nullptr),
    m_defaultSession(session),
    m_fEnabled(true)
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxRecognizer::~CSpxRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
    m_defaultSession->RemoveRecognizer(this);
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

void CSpxRecognizer::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    if (result->GetReason() == Reason::Recognized && FinalResult.IsConnected())
    {
        auto recoEvent = SpxMakeShared<CSpxRecognitionEventArgs, ISpxRecognitionEventArgs>(sessionId, result);

        auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
        auto hevent = eventhandles->TrackHandle(recoEvent);

        FinalResult.Signal(recoEvent);
    }
    else if (result->GetReason() == Reason::IntermediateResult && IntermediateResult.IsConnected())
    {
        auto recoEvent = SpxMakeShared<CSpxRecognitionEventArgs, ISpxRecognitionEventArgs>(sessionId, result);

        auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
        auto hevent = eventhandles->TrackHandle(recoEvent);

        IntermediateResult.Signal(recoEvent);
    }
    else if (result->GetReason() == Reason::NoMatch && NoMatch.IsConnected())
    {
        auto recoEvent = SpxMakeShared<CSpxRecognitionEventArgs, ISpxRecognitionEventArgs>(sessionId, result);

        auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
        auto hevent = eventhandles->TrackHandle(recoEvent);

        NoMatch.Signal(recoEvent);
    }
    else if (result->GetReason() == Reason::Canceled && Canceled.IsConnected())
    {
        auto recoEvent = SpxMakeShared<CSpxRecognitionEventArgs, ISpxRecognitionEventArgs>(sessionId, result);

        auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
        auto hevent = eventhandles->TrackHandle(recoEvent);

        Canceled.Signal(recoEvent);
    }
    else if (result->GetReason() == Reason::OtherRecognizer)
    {
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }
    else
    {
        auto reason = result->GetReason();
        SPX_DBG_ASSERT_WITH_MESSAGE(
            reason != Reason::Recognized && 
            reason != Reason::IntermediateResult &&
            reason != Reason::NoMatch &&
            reason != Reason::Canceled && 
            reason != Reason::OtherRecognizer,
            "The reason found in the result was unexpected.");
    }
}

void CSpxRecognizer::OnIsEnabledChanged()
{
    // no op currently
}


}; // CARBON_IMPL_NAMESPACE()

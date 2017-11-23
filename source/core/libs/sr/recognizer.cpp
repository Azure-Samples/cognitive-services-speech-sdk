#include "stdafx.h"
#include <future>
#include "recognizer.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxRecognizer::CSpxRecognizer(std::shared_ptr<ISpxSession>& session) :
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

void CSpxRecognizer::OnIsEnabledChanged()
{
    // no op currently
}


}; // CARBON_IMPL_NAMESPACE()

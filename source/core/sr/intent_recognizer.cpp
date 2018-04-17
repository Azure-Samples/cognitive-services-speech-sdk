#include "stdafx.h"
#include "intent_recognizer.h"
#include "service_helpers.h"
#include "site_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxIntentRecognizer::CSpxIntentRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxIntentRecognizer::~CSpxIntentRecognizer()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxIntentRecognizer::Init()
{
    CSpxRecognizer::Init();
    InitTriggerService();
}

void CSpxIntentRecognizer::AddIntentTrigger(const wchar_t* id, std::shared_ptr<ISpxTrigger> trigger)
{
    auto triggerService = m_triggerService.lock();
    triggerService->AddIntentTrigger(id, trigger);
}

void CSpxIntentRecognizer::InitTriggerService()
{
    // Ensure that the Trigger Service exists and is ready to do it's thing... 
    m_triggerService = SpxQueryService<ISpxIntentTriggerService>(GetSite());
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

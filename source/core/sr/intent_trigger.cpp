#include "stdafx.h"
#include "intent_trigger.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


void CSpxIntentTrigger::InitPhraseTrigger(const wchar_t* phrase)
{
    SPX_IFTRUE_THROW_HR(!m_intentName.empty() || m_model != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!m_phrase.empty(), SPXERR_ALREADY_INITIALIZED);
    m_phrase = phrase;
}

void CSpxIntentTrigger::InitLanguageUnderstandingModelTrigger(std::shared_ptr<ISpxLanguageUnderstandingModel> model, const wchar_t* intentName)
{
    SPX_IFTRUE_THROW_HR(!m_intentName.empty() || m_model != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!m_phrase.empty(), SPXERR_ALREADY_INITIALIZED);
    m_model = model;
    m_intentName = intentName; 
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

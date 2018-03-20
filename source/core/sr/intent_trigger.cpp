#include "stdafx.h"
#include "intent_trigger.h"


namespace CARBON_IMPL_NAMESPACE() {


void CSpxIntentTrigger::InitPhraseTrigger(const wchar_t* phrase)
{
    SPX_IFTRUE_THROW_HR(!m_intentName.empty() || m_model != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!m_phrase.empty(), SPXERR_ALREADY_INITIALIZED);
    m_phrase = phrase;
}

void CSpxIntentTrigger::InitLuisModelTrigger(std::shared_ptr<ISpxLuisModel> model, const wchar_t* intentName)
{
    SPX_IFTRUE_THROW_HR(!m_intentName.empty() || m_model != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!m_phrase.empty(), SPXERR_ALREADY_INITIALIZED);
    m_model = model;
    m_intentName = intentName; 
}


} // CARBON_IMPL_NAMESPACE

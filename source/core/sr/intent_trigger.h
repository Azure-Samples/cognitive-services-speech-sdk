#pragma once
#include <string>
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxIntentTrigger : public ISpxTrigger
{
public:

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxTrigger)
    SPX_INTERFACE_MAP_END()

    // --- ISpxTrigger ---
    void InitPhraseTrigger(const wchar_t* phrase) override;
    void InitLanguageUnderstandingModelTrigger(std::shared_ptr<ISpxLanguageUnderstandingModel> model, const wchar_t* intentName) override;

    std::wstring GetPhrase() const override { return m_phrase; }
    
    std::shared_ptr<ISpxLanguageUnderstandingModel> GetModel() const override { return m_model; }
    std::wstring GetModelIntentName() const override { return m_intentName; }


private:

    std::wstring m_phrase;

    std::shared_ptr<ISpxLanguageUnderstandingModel> m_model;
    std::wstring m_intentName;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

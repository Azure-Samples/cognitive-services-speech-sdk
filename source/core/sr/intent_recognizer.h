#pragma once
#include <list>
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "recognizer.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxIntentRecognizer : public CSpxRecognizer, public ISpxIntentRecognizer
{
public:

    CSpxIntentRecognizer();
    ~CSpxIntentRecognizer();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxSessionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizerEvents)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxIntentRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxMessageParamFromUser)
        SPX_INTERFACE_MAP_ENTRY(ISpxGetUspMessageParamsFromUser)
        SPX_INTERFACE_MAP_ENTRY(ISpxGrammarList)
        SPX_INTERFACE_MAP_ENTRY(ISpxGrammar)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit (overrides) ---
    void Init() override;

    // --- ISpxIntentRecognizer ---
    void AddIntentTrigger(const wchar_t* intentId, std::shared_ptr<ISpxTrigger> trigger) override;


    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

private:

    CSpxIntentRecognizer(CSpxIntentRecognizer&&) = delete;
    CSpxIntentRecognizer(const CSpxIntentRecognizer&) = delete;
    CSpxIntentRecognizer& operator=(CSpxIntentRecognizer&&) = delete;
    CSpxIntentRecognizer& operator=(const CSpxIntentRecognizer&) = delete;

    void InitTriggerService();

    std::list<std::shared_ptr<ISpxTrigger>> m_triggers;
    std::weak_ptr<ISpxIntentTriggerService> m_triggerService;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

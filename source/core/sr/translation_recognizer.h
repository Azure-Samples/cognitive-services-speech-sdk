#pragma once
#include <list>
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "recognizer.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxTranslationRecognizer : public CSpxRecognizer, public ISpxTranslationRecognizer
{
public:

    CSpxTranslationRecognizer();
    ~CSpxTranslationRecognizer();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxSessionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizerEvents)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxTranslationRecognizer)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit (overrides) ---
    void Init() override;

private:

    CSpxTranslationRecognizer(CSpxTranslationRecognizer&&) = delete;
    CSpxTranslationRecognizer(const CSpxTranslationRecognizer&) = delete;
    CSpxTranslationRecognizer& operator=(CSpxTranslationRecognizer&&) = delete;
    CSpxTranslationRecognizer& operator=(const CSpxTranslationRecognizer&) = delete;
};


} // CARBON_IMPL_NAMESPACE

#pragma once
#include <list>
#include "ispxinterfaces.h"
#include "recognizer.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxTranslationRecognizer : public CSpxRecognizer, public ISpxTranslationRecognizer
{
public:

    CSpxTranslationRecognizer();
    ~CSpxTranslationRecognizer();

    // --- ISpxObjectInit (overrides) ---
    void Init() override;

private:

    CSpxTranslationRecognizer(CSpxTranslationRecognizer&&) = delete;
    CSpxTranslationRecognizer(const CSpxTranslationRecognizer&) = delete;
    CSpxTranslationRecognizer& operator=(CSpxTranslationRecognizer&&) = delete;
    CSpxTranslationRecognizer& operator=(const CSpxTranslationRecognizer&) = delete;
};


} // CARBON_IMPL_NAMESPACE

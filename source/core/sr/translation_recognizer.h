#pragma once
#include <list>
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "recognizer.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


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
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxGrammarList)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit (overrides) ---
    void Init() override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
        SPX_SERVICE_MAP_END()

    // --- ISpxTranslationRecognizer ---
    void AddTargetLanguage(const std::string& lang) override;
    void RemoveTargetLanguage(const std::string& lang) override;

private:

    void UpdateTargetLanguages(const std::string& targetLanguages);

    CSpxTranslationRecognizer(CSpxTranslationRecognizer&&) = delete;
    CSpxTranslationRecognizer(const CSpxTranslationRecognizer&) = delete;
    CSpxTranslationRecognizer& operator=(CSpxTranslationRecognizer&&) = delete;
    CSpxTranslationRecognizer& operator=(const CSpxTranslationRecognizer&) = delete;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

#include "stdafx.h"
#include "class_language_model.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

void CSpxClassLanguageModel::InitClassLanguageModel(const wchar_t* id)
{
    SPX_IFTRUE_THROW_HR(!m_id.empty(), SPXERR_ALREADY_INITIALIZED);
    m_id = id;
}

void CSpxClassLanguageModel::AssignClass(const wchar_t *className, std::shared_ptr<ISpxGrammar> grammar)
{
    SPX_IFTRUE_THROW_HR(!className, SPXERR_INVALID_ARG);
    SPX_IFTRUE_THROW_HR(!grammar, SPXERR_INVALID_ARG);

    m_referencedGrammars.push_back(std::pair<std::wstring, std::shared_ptr<ISpxGrammar>>(className, grammar));
}

std::list<std::string> CSpxClassLanguageModel::GetListenForList()
{
    // The USP Reco Engine Apapter expects the reference grammars in the format
    // "{string:string[/string]*}" and will translate to string/string[/string]*

    std::list<std::string> retVal;

    for (auto classReference : m_referencedGrammars)
    {
        for (auto grammar : classReference.second->GetListenForList())
        {
            retVal.push_back("{" + PAL::ToString(m_id) + ":" + PAL::ToString(classReference.first) + "/" + grammar + "}");
        }
    }

    return retVal;
}

}}}} // Microsoft::CognitiveServices::Speech::Impl

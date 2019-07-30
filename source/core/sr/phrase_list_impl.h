//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include <string>
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class ISpxPhraseListImpl : public ISpxPhraseList, public ISpxGrammar
{
public:

    // --- ISpxPhraseList ---
    void InitPhraseList(const wchar_t* name) override
    {
        SPX_IFTRUE_THROW_HR(!m_name.empty(), SPXERR_ALREADY_INITIALIZED);
        m_name = name;
    }

    std::wstring GetName() override
    {
        return m_name;
    }

    void AddPhrase(std::shared_ptr<ISpxPhrase> phrase) override
    {
        m_phrases.push_back(phrase);
    }

    void Clear() override
    {
        m_phrases.clear();
    }

    std::list<std::string> GetListenForList() override
    {
        std::list<std::string> listenForList;
        for (auto item = m_phrases.begin(); item!= m_phrases.end(); item++)
        {
            auto phrase = *item;
            listenForList.push_back(PAL::ToString(phrase->GetPhrase()));
        }
        return listenForList;
    }

private:

    std::wstring m_name;
    std::list<std::shared_ptr<ISpxPhrase>> m_phrases;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

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

class CSpxClassLanguageModel : public ISpxClassLanguageModel, public ISpxGrammar
{
public:

    CSpxClassLanguageModel() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxClassLanguageModel)
        SPX_INTERFACE_MAP_ENTRY(ISpxGrammar)
    SPX_INTERFACE_MAP_END()

    // ISpxClassLanguageModel
    void InitClassLanguageModel(const wchar_t *id) override;
    void AssignClass(const wchar_t *className, std::shared_ptr<ISpxGrammar> grammar) override;

    // ISpxGrammar
    std::list<std::string> GetListenForList() override;

private:

    DISABLE_COPY_AND_MOVE(CSpxClassLanguageModel);

    std::wstring m_id;
    std::list<std::pair<std::wstring, std::shared_ptr<ISpxGrammar>>> m_referencedGrammars;
};

}}}} // Microsoft::CognitiveServices::Speech::Impl
#pragma once

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include <string>
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "phrase_list_impl.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxPhraseListGrammar : public ISpxPhraseListImpl, public ISpxGrammar
{
public:

    CSpxPhraseListGrammar() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxGrammar)
        SPX_INTERFACE_MAP_ENTRY(ISpxPhraseList)
    SPX_INTERFACE_MAP_END()

private:

    DISABLE_COPY_AND_MOVE(CSpxPhraseListGrammar);
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_grammar.cpp: Public API definitions for Grammar related C methods
//

#include "stdafx.h"
#include "create_object_helpers.h"
#include "handle_helpers.h"
#include "resource_manager.h"


using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI_(bool) grammar_handle_is_valid(SPXGRAMMARHANDLE hgrammar)
{
    return Handle_IsValid<SPXGRAMMARHANDLE, ISpxGrammar>(hgrammar);
}

SPXAPI grammar_handle_release(SPXGRAMMARHANDLE hgrammar)
{
    return Handle_Close<SPXGRAMMARHANDLE, ISpxGrammar>(hgrammar);
}

SPXAPI phrase_list_grammar_from_recognizer_by_name(SPXGRAMMARHANDLE* hgrammar, SPXRECOHANDLE hreco, const char* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *hgrammar = SPXHANDLE_INVALID;

        auto reco = CSpxSharedPtrHandleTableManager::GetPtr<ISpxRecognizer, SPXRECOHANDLE>(hreco);
        auto grammarlist = SpxQueryInterface<ISpxGrammarList>(reco);
        auto grammar = grammarlist->GetPhraseListGrammar(PAL::ToWString(name).c_str());

        *hgrammar = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxGrammar, SPXGRAMMARHANDLE>(grammar);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI phrase_list_grammar_add_phrase(SPXGRAMMARHANDLE hgrammar, SPXPHRASEHANDLE hphrase)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto grammar = CSpxSharedPtrHandleTableManager::GetPtr<ISpxGrammar, SPXGRAMMARHANDLE>(hgrammar);
        auto phraselist = SpxQueryInterface<ISpxPhraseList>(grammar);

        auto phrase = CSpxSharedPtrHandleTableManager::GetPtr<ISpxPhrase, SPXPHRASEHANDLE>(hphrase);
        phraselist->AddPhrase(phrase);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI phrase_list_grammar_clear(SPXGRAMMARHANDLE hgrammar)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto grammar = CSpxSharedPtrHandleTableManager::GetPtr<ISpxGrammar, SPXGRAMMARHANDLE>(hgrammar);
        auto phraselist = SpxQueryInterface<ISpxPhraseList>(grammar);
        phraselist->Clear();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) grammar_phrase_handle_is_valid(SPXPHRASEHANDLE hphrase)
{
    return Handle_IsValid<SPXPHRASEHANDLE, ISpxPhrase>(hphrase);
}

SPXAPI grammar_phrase_handle_release(SPXPHRASEHANDLE hphrase)
{
    return Handle_Close<SPXPHRASEHANDLE, ISpxPhrase>(hphrase);
}

SPXAPI grammar_phrase_create_from_text(SPXPHRASEHANDLE* hphrase, const char* text)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *hphrase = SPXHANDLE_INVALID;

        auto phrase = SpxCreateObjectWithSite<ISpxPhrase>("CSpxPhrase", SpxGetRootSite());
        phrase->InitPhrase(PAL::ToWString(text).c_str());

        *hphrase = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxPhrase, SPXPHRASEHANDLE>(phrase);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

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
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, name == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hgrammar == nullptr);

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
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hgrammar == nullptr);

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
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, text == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hphrase == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hphrase = SPXHANDLE_INVALID;

        auto phrase = SpxCreateObjectWithSite<ISpxPhrase>("CSpxPhrase", SpxGetRootSite());
        SPX_RETURN_HR_IF(SPXERR_RUNTIME_ERROR, phrase == nullptr);

        phrase->InitPhrase(PAL::ToWString(text).c_str());

        *hphrase = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxPhrase, SPXPHRASEHANDLE>(phrase);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI grammar_create_from_storage_id(SPXGRAMMARHANDLE *hgrammar, const char *id)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hgrammar == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, id == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hgrammar = SPXHANDLE_INVALID;

        auto grammar = SpxCreateObjectWithSite<ISpxStoredGrammar>("CSpxStoredGrammar", SpxGetRootSite());
        SPX_RETURN_HR_IF(SPXERR_RUNTIME_ERROR, grammar == nullptr);

        grammar->InitStoredGrammar(PAL::ToWString(id).c_str());

        auto g = SpxQueryInterface<ISpxGrammar>(grammar);
        SPX_RETURN_HR_IF(SPXERR_INVALID_HANDLE, g == nullptr);

        *hgrammar = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxGrammar, SPXGRAMMARHANDLE>(g);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI grammar_list_from_recognizer(SPXGRAMMARHANDLE *hgrammarlist, SPXRECOHANDLE hreco)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hgrammarlist == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hreco == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hgrammarlist = SPXHANDLE_INVALID;

        auto reco = CSpxSharedPtrHandleTableManager::GetPtr<ISpxRecognizer, SPXRECOHANDLE>(hreco);
        SPX_RETURN_HR_IF(SPXERR_INVALID_HANDLE, reco == nullptr);

        auto grammarlist = SpxQueryInterface<ISpxGrammar>(reco);
        SPX_RETURN_HR_IF(SPXERR_INVALID_HANDLE, grammarlist == nullptr);

        *hgrammarlist = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxGrammar, SPXGRAMMARHANDLE>(grammarlist);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI grammar_list_add_grammar(SPXGRAMMARHANDLE hgrammarlist, SPXGRAMMARHANDLE hgrammar)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hgrammarlist == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hgrammar == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto glg = CSpxSharedPtrHandleTableManager::GetPtr<ISpxGrammar, SPXGRAMMARHANDLE>(hgrammarlist);
        SPX_RETURN_HR_IF(SPXERR_INVALID_HANDLE, glg == nullptr);

        auto gl = SpxQueryInterface<ISpxGrammarList>(glg);
        SPX_RETURN_HR_IF(SPXERR_INVALID_HANDLE, gl == nullptr);

        auto grammar = CSpxSharedPtrHandleTableManager::GetPtr<ISpxGrammar, SPXGRAMMARHANDLE>(hgrammar);
        SPX_RETURN_HR_IF(SPXERR_INVALID_HANDLE, grammar == nullptr);

        gl->AddGrammar(grammar);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI class_language_model_from_storage_id(SPXGRAMMARHANDLE* hclm, const char *storageid)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hclm == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, storageid == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hclm = SPXHANDLE_INVALID;

        auto clm = SpxCreateObjectWithSite<ISpxClassLanguageModel>("CSpxClassLanguageModel", SpxGetRootSite());
        SPX_RETURN_HR_IF(SPXERR_RUNTIME_ERROR, clm == nullptr);

        clm->InitClassLanguageModel(PAL::ToWString(storageid).c_str());

        auto g = SpxQueryInterface<ISpxGrammar>(clm);

        *hclm = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxGrammar, SPXGRAMMARHANDLE>(g);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI class_language_model_assign_class(SPXGRAMMARHANDLE hclm, const char *classname, SPXGRAMMARHANDLE hgrammar)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hclm == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, classname == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hgrammar == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, *classname == '\0');

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto g = CSpxSharedPtrHandleTableManager::GetPtr<ISpxGrammar, SPXGRAMMARHANDLE>(hclm);

        auto clm = SpxQueryInterface<ISpxClassLanguageModel>(g);
        SPX_RETURN_HR_IF(SPXERR_RUNTIME_ERROR, clm == nullptr);

        auto grammar = CSpxSharedPtrHandleTableManager::GetPtr<ISpxGrammar, SPXGRAMMARHANDLE>(hgrammar);
        SPX_RETURN_HR_IF(SPXERR_INVALID_HANDLE, grammar == nullptr);

        clm->AssignClass(PAL::ToWString(classname).c_str(), grammar);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_grammar.h: Public API declarations for Grammar related C methods and typedefs
//

#pragma once
#include <speechapi_c_common.h>


SPXAPI_(bool) grammar_handle_is_valid(SPXGRAMMARHANDLE hgrammar);
SPXAPI phrase_list_grammar_from_recognizer_by_name(SPXGRAMMARHANDLE* hgrammar, SPXRECOHANDLE hreco, const char* name);
SPXAPI grammar_handle_release(SPXGRAMMARHANDLE hgrammar);

SPXAPI phrase_list_grammar_add_phrase(SPXGRAMMARHANDLE hgrammar, SPXPHRASEHANDLE hphrase);
SPXAPI phrase_list_grammar_clear(SPXGRAMMARHANDLE hgrammar);

SPXAPI_(bool) grammar_phrase_handle_is_valid(SPXPHRASEHANDLE hphrase);
SPXAPI grammar_phrase_create_from_text(SPXPHRASEHANDLE* hphrase, const char* phrase);
SPXAPI grammar_phrase_handle_release(SPXPHRASEHANDLE hphrase);

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
// intentapi_cxx_exports.h: Export/import macros for the standalone intent recognizer DLL
//

#pragma once

#if defined(_WIN32)
#ifdef INTENTRECOGNIZER_EXPORTS
#define INTENT_API __declspec(dllexport)
#else
#define INTENT_API __declspec(dllimport)
#endif
#else
#define INTENT_API
#endif

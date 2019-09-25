//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
#pragma once

#include "spxcore_common.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "ispxinterfaces.h"
#include "stdafx.h"
#include <sstream>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxLanguageListUtils
{
public:
    static void AddLangToList(const std::string& lang, std::string& languageList);
    static void RemoveLangFromList(const std::string& lang, std::string& languageList);
};
} } } } // Microsoft::CognitiveServices::Speech::Impl

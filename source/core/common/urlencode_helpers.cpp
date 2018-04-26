//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_helpers.cpp: Implementation definitions for *Http* helper methods
//

#include "stdafx.h"
#include "urlencode_helpers.h"
#include <string.h>
#include "azure_c_shared_utility_urlencode_wrapper.h"
#include <cstdlib>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

std::string UrlEncode(const std::string& text)
{
    STRING_HANDLE urlEncodedText = URL_EncodeString(text.c_str());
    std::string urlencodedQuery = STRING_c_str(urlEncodedText);
    STRING_delete(urlEncodedText);

    return urlencodedQuery;
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

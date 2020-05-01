//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_utils.cpp: General utility classes and functions.
//

#pragma once

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Utils {

SPXSTRING TryLoadString(SPXEVENTHANDLE hevent, SPXHR(SPXAPI_CALLTYPE * func)(SPXEVENTHANDLE, char*, uint32_t *))
{
    std::unique_ptr<char[]> psz;
    try
    {
        // query the string length
        uint32_t length = 0;

        // don't use SPX_THROW_ON_FAIL since that creates a handle for exceptions that will leak
        // since we don't care about them.
        SPXHR hr = func(hevent, nullptr, &length);
        if (SPX_FAILED(hr) || length == 0)
        {
            return SPXSTRING{};
        }

        psz = std::unique_ptr<char[]>(new char[length]);
        hr = func(hevent, psz.get(), &length);
        if (SPX_FAILED(hr))
        {
            return SPXSTRING{};
        }

        return Utils::ToSPXString(psz.get());
    }
    catch (...)
    {
        // ignore errors since not all participants have the properties we need
        return SPXSTRING{};
    }
}

}}}}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#pragma once
#include <speechapi_c_common.h>
#include "handle_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

/// <summary>
/// Helper method to read a string value.
/// </summary>
/// <param name="handle">The handle to the object to read from</param>
/// <param name="psz">A pointer to null terminated string. We will copy the string to here. Set this
/// to nullptr to query the length of the string.</param>
/// <param name="pcch">A pointer to an in/out length value. If psz is a valid pointer, this will be
/// used to determine the length of the psz buffer available. On success, this will always be set
/// be set to the length of the buffer needed or the number of bytes copied.</param>
/// <param name="getter">The pointer to an instance method to retrieve a string value</param>
template<typename TInterface, typename THandle, typename TString>
SPXHR _RetrieveStringValue(THandle handle, char * psz, uint32_t * pcch, TString(TInterface::*getter)() const)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pcch == nullptr);

    auto evt = TryGetInstance<TInterface>(handle);
    SPX_IFTRUE_RETURN_HR(evt == nullptr, SPXERR_INVALID_HANDLE);

    std::string str = Utils::ToUTF8((evt.get()->*getter)());

    if (psz == nullptr)
    {
        // we are querying the string length so just set that and return
        *pcch = static_cast<int32_t>(str.length() + 1);
    }
    else
    {
        // copy over the string value with a trailing \0. This will truncate
        // strings that are too long
        size_t length = str.length() + 1;
        if (*pcch < length)
        {
            length = static_cast<size_t>(*pcch);
        }

        *pcch = static_cast<uint32_t>(snprintf(psz, length, "%s", str.c_str()));
    }

    return SPX_NOERROR;
}


/// <summary>
/// Retrieves strings capturing exceptions to handle
/// </summary>
template<typename TInterface, typename THandle, typename TString>
SPXHR RetrieveStringValue(THandle handle, char * psz, uint32_t * pcch, TString(TInterface::*getter)() const)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        return _RetrieveStringValue(handle, psz, pcch, getter);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

/// <summary>
/// Retrieves strings *without* capturing exceptions to handle
/// </summary>
template<typename TInterface, typename THandle, typename TString>
SPXHR TryRetrieveStringValue(THandle handle, char * psz, uint32_t * pcch, TString(TInterface::*getter)() const)
{
    try
    {
        return _RetrieveStringValue(handle, psz, pcch, getter);
    }
    catch (...)
    {
        uint32_t oldLength = pcch == nullptr ? 0 : *pcch;
        *pcch = 0;
        if (oldLength > 0)
        {
            *psz = '\0';
        }

        return SPX_NOERROR;
    }
}
 }}}}

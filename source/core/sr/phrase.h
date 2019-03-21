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


class CSpxPhrase : public ISpxPhrase
{
public:

    CSpxPhrase() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxPhrase)
    SPX_INTERFACE_MAP_END()

    // --- ISpxPhrase ---
    void InitPhrase(const wchar_t* phrase) override;
    std::wstring GetPhrase() const override;

private:

    DISABLE_COPY_AND_MOVE(CSpxPhrase);

    std::wstring m_phrase;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

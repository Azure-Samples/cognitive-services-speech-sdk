//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// word_boundary_event_args.h: Implementation declarations for CSpxWordBoundaryEventArgs C++ class
//

#pragma once
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxWordBoundaryEventArgs :
    public ISpxWordBoundaryEventArgs,
    public ISpxWordBoundaryEventArgsInit
{
public:

    CSpxWordBoundaryEventArgs();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxWordBoundaryEventArgs)
        SPX_INTERFACE_MAP_ENTRY(ISpxWordBoundaryEventArgsInit)
    SPX_INTERFACE_MAP_END()

    // --- ISpxWordBoundaryEventArgs
    uint64_t GetAudioOffset() override;
    uint32_t GetTextOffset() override;
    uint32_t GetWordLength() override;

    // --- ISpxWordBoundaryEventArgsInit
    void Init(uint64_t audioOffset, uint32_t textOffset, uint32_t wordLength) override;


private:

    CSpxWordBoundaryEventArgs(const CSpxWordBoundaryEventArgs&) = delete;
    CSpxWordBoundaryEventArgs(const CSpxWordBoundaryEventArgs&&) = delete;

    CSpxWordBoundaryEventArgs& operator=(const CSpxWordBoundaryEventArgs&) = delete;

    uint64_t m_audioOffset { 0 };
    uint32_t m_textOffset { 0 };
    uint32_t m_wordLength { 0 };
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

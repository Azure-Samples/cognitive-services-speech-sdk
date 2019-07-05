//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// word_boundary_event_args.cpp: Implementation definitions for CSpxWordBoundaryEventArgs C++ class
//

#include "stdafx.h"
#include "word_boundary_event_args.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxWordBoundaryEventArgs::CSpxWordBoundaryEventArgs()
{
}

uint64_t CSpxWordBoundaryEventArgs::GetAudioOffset()
{
    return m_audioOffset;
}

uint32_t CSpxWordBoundaryEventArgs::GetTextOffset()
{
    return m_textOffset;
}

uint32_t CSpxWordBoundaryEventArgs::GetWordLength()
{
    return m_wordLength;
}

void CSpxWordBoundaryEventArgs::Init(uint64_t audioOffset, uint32_t textOffset, uint32_t wordLength)
{
    m_audioOffset = audioOffset;
    m_textOffset = textOffset;
    m_wordLength = wordLength;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_wav_file_reader.h: Implementation definitions for CSpxMockWavFileReader C++ class
//

#pragma once
#include "stdafx.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "mock_audio_file_impl.h"
#include "mock_audio_reader_impl.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxMockWavFileReader : 
    public ISpxMockAudioFileImpl,
    public ISpxMockAudioReaderImpl
{
public:

    CSpxMockWavFileReader() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioReader)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioFile)
    SPX_INTERFACE_MAP_END()


private:

    CSpxMockWavFileReader(CSpxMockWavFileReader&&) = delete;
    CSpxMockWavFileReader(const CSpxMockWavFileReader&) = delete;
    CSpxMockWavFileReader& operator=(CSpxMockWavFileReader&&) = delete;
    CSpxMockWavFileReader& operator=(const CSpxMockWavFileReader&) = delete;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_wav_file_reader.h: Implementation definitions for CSpxMockWavFileReader C++ class
//

#pragma once
#include "stdafx.h"
#include "ispxinterfaces.h"
#include "mock_audio_file_impl.h"
#include "mock_audio_reader_impl.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxMockWavFileReader : 
    public ISpxMockAudioFileImpl,
    public ISpxMockAudioReaderImpl
{
public:

    CSpxMockWavFileReader() = default;


private:

    CSpxMockWavFileReader(CSpxMockWavFileReader&&) = delete;
    CSpxMockWavFileReader(const CSpxMockWavFileReader&) = delete;
    CSpxMockWavFileReader& operator=(CSpxMockWavFileReader&&) = delete;
    CSpxMockWavFileReader& operator=(const CSpxMockWavFileReader&) = delete;
};


} // CARBON_IMPL_NAMESPACE

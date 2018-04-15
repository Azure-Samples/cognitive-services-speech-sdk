//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_audio_reader.h: Implementation definitions for CSpxMockWavFileReader C++ class
//

#pragma once
#include "stdafx.h"
#include "mock_audio_reader_impl.h"
#include "interface_helpers.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxMockAudioReader : public ISpxMockAudioReaderImpl
{
public:

    CSpxMockAudioReader() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioReader)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioReaderRealTime)
    SPX_INTERFACE_MAP_END()


private:

    CSpxMockAudioReader(CSpxMockAudioReader&&) = delete;
    CSpxMockAudioReader(const CSpxMockAudioReader&) = delete;
    CSpxMockAudioReader& operator=(CSpxMockAudioReader&&) = delete;
    CSpxMockAudioReader& operator=(const CSpxMockAudioReader&) = delete;
};


} // CARBON_IMPL_NAMESPACE

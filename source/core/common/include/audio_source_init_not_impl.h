//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_init_not_impl.h: Implementation declarations/definitions for ISpxAudioSourceInitNotImpl
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxAudioSourceInitNotImpl : public ISpxAudioSourceInit
{
public:

    void InitFromMicrophone() override { SPX_THROW_HR(SPXERR_NOT_IMPL); }
    void InitFromFile(const wchar_t* pszFileName) override { UNUSED(pszFileName); SPX_THROW_HR(SPXERR_NOT_IMPL); }
    void InitFromStream(std::shared_ptr<ISpxAudioStream> stream) override { UNUSED(stream); SPX_THROW_HR(SPXERR_NOT_IMPL); }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// activity_event_args.h: Implementation declaration for CSpxActivityEventArgs C++ class.
//
#pragma once

#include "ispxinterfaces.h"
#include "interface_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxActivityEventArgs :
    public ISpxActivityEventArgs,
    public ISpxActivityEventArgsInit
{
public:

    CSpxActivityEventArgs();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxActivityEventArgs)
        SPX_INTERFACE_MAP_ENTRY(ISpxActivityEventArgsInit)
    SPX_INTERFACE_MAP_END()


    // --- ISpxActivityEventArgs

    virtual const std::string& GetActivity() const final;
    bool HasAudio() const final;
    virtual std::shared_ptr<ISpxAudioOutput> GetAudio() const final;

    // --- ISpxActivityEventArgsInit
    virtual void Init(std::string activity) final;
    virtual void Init(std::string activity, std::shared_ptr<ISpxAudioOutput> audio) final;
private:

    CSpxActivityEventArgs(const CSpxActivityEventArgs&) = delete;
    CSpxActivityEventArgs(CSpxActivityEventArgs&&) = delete;

    CSpxActivityEventArgs& operator=(const CSpxActivityEventArgs&) = delete;

    std::string m_activity;
    std::shared_ptr<ISpxAudioOutput> m_audio;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

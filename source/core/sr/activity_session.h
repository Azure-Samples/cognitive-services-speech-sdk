//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// activity_session.h: Implementation declarations for CSpxActivitySession C++ class
//
#pragma once

#include <iostream>
#include <memory>
#include "state_machine.h"
#include "usp.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxUspRecoEngineAdapter;

class CSpxActivitySession
{
public:
    enum class State : unsigned short
    {
        Start = 0,
        ActivityReceived,
        AudioReceived,
        End
    };

    CSpxActivitySession(std::weak_ptr<Microsoft::CognitiveServices::Speech::Impl::CSpxUspRecoEngineAdapter> wk_ptr);

    CSpxActivitySession(const CSpxActivitySession&) = delete;
    CSpxActivitySession& operator=(const CSpxActivitySession&) = delete;

    CSpxActivitySession(CSpxActivitySession&&) = default;
    CSpxActivitySession& operator=(CSpxActivitySession&&) = default;

    ~CSpxActivitySession();

    void Switch(State state, const std::string* activityMsg, const USP::AudioOutputChunkMsg* audioMsg);

private:
        using machine_t = typename Microsoft::CognitiveServices::Speech::Impl::state_machine<State, State::Start, std::function<void(const std::string*, const USP::AudioOutputChunkMsg*)>>;

        std::weak_ptr<CSpxUspRecoEngineAdapter> m_adapter;
        machine_t m_state_machine;

        std::shared_ptr<ISpxActivity> m_activity;
        std::shared_ptr<ISpxAudioOutput> m_output_stream;

        void BuildActivityMsg(const std::string* activityMsg);
        void WriteToOutputStream(const USP::AudioOutputChunkMsg* audioMsg);
        void FireActivityResult();
    };

}}}} // Microsoft::CognitiveServices::Speech::Impl


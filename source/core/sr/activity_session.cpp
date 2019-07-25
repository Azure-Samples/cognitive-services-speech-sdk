//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// activity_session.cpp: Implementation definitions for CSpxActivitySession C++ class
//
#include "stdafx.h"
#include "activity_session.h"
#include "usp_reco_engine_adapter.h"
#include "site_helpers.h"
#include "create_object_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

namespace ExpectedDataStreamType
{
    constexpr uint32_t TextToSpeechAudio = 1;
}

CSpxActivitySession::CSpxActivitySession(std::weak_ptr<CSpxUspRecoEngineAdapter> adapter) : m_adapter{ adapter }, m_activity{ nullptr }, m_output_stream{ nullptr }
{
    m_state_machine =
    {
        {
            State::Start,
            {
                {
                    State::ActivityReceived,
                    [this](const std::string* msg, const USP::AudioOutputChunkMsg*)
                    {
                        BuildActivityMsg(msg);
                        FireActivityResult();
                    }
                }
            }
        },
        {
            State::ActivityReceived,
            {
                {
                    State::AudioReceived,
                    [this](const std::string*, const USP::AudioOutputChunkMsg* audioMsg)
                    {
                        WriteToOutputStream(audioMsg);
                    }
                },
                {
                    State::End,
                    [](const std::string*, const USP::AudioOutputChunkMsg*)
                    {
                        /* Noop */
                    }
                }
            }
        },
        {
            State::AudioReceived,
            {
                {
                    State::AudioReceived,
                    [this](const std::string*, const USP::AudioOutputChunkMsg* audioMsg)
                    {
                        WriteToOutputStream(audioMsg);
                    }
                },
                {
                    State::End,
                    [this](const std::string*, const USP::AudioOutputChunkMsg*)
                    {
                        m_output_stream->Close();
                    }
                }
            }
        }
    };
}

CSpxActivitySession::~CSpxActivitySession()
{
}

void CSpxActivitySession::BuildActivityMsg(const std::string* activityMsg)
{
    auto messageJSON = nlohmann::json::parse(*activityMsg);
    /* We first build the activity */
    auto activityJSON = messageJSON["messagePayload"];
    m_activity = SpxCreateObjectWithSite<ISpxActivity>("CSpxActivity", SpxGetRootSite());
    m_activity->LoadJSON(activityJSON);
    /* We now check if there is a stream associated */
    if (messageJSON["messageDataStreamType"].is_number_unsigned())
    {
        auto dataStreamType = messageJSON["messageDataStreamType"].get<uint32_t>();
        if (dataStreamType & ExpectedDataStreamType::TextToSpeechAudio)
        {
            m_output_stream = SpxCreateObjectWithSite<ISpxAudioOutput>("CSpxPullAudioOutputStream", SpxGetRootSite());
        }
    }
}

void CSpxActivitySession::WriteToOutputStream(const USP::AudioOutputChunkMsg* audioMsg)
{
    auto size = (uint32_t)audioMsg->audioLength;

    if (m_output_stream == nullptr)
    {
        /* If we get here, this should never be nullptr */
        SPX_THROW_HR(SPXERR_INVALID_STATE);
    }

    /* HACKHACK: Need to update Write to take const T* */
    m_output_stream->Write(const_cast<uint8_t *>(audioMsg->audioBuffer), size);
}

void CSpxActivitySession::FireActivityResult()
{
    if (auto adapter = m_adapter.lock())
    {
        adapter->FireActivityResult(m_activity, m_output_stream);
    }
}

void CSpxActivitySession::Switch(State state, const std::string* activiyMsg, const USP::AudioOutputChunkMsg* audioMsg)
{
    m_state_machine.transition(state, activiyMsg, audioMsg);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

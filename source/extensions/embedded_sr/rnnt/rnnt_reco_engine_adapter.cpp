//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rnnt_reco_engine_adapter.cpp: Implementation definitions for CSpxRnntRecoEngineAdapter C++ class
//

#include <cinttypes>
#include <exception>

#include "stdafx.h"
#include "rnnt_reco_engine_adapter.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "property_id_2_name_map.h"
#include "string_utils.h"
#include "error_info.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

CSpxRnntRecoEngineAdapter::CSpxRnntRecoEngineAdapter() :
    m_audioState(AudioState::Idle),
    m_rnntState(RnntState::Idle)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);
}

CSpxRnntRecoEngineAdapter::~CSpxRnntRecoEngineAdapter()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);
    SPX_DBG_ASSERT(m_rnntCallbacks == nullptr);
    SPX_DBG_ASSERT(m_rnntClient == nullptr);
}

void CSpxRnntRecoEngineAdapter::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);

    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    SPX_IFTRUE_THROW_HR(m_rnntCallbacks != nullptr, SPXERR_ALREADY_INITIALIZED);

    SPX_DBG_ASSERT(IsState(AudioState::Idle) && IsState(RnntState::Idle));
}

void CSpxRnntRecoEngineAdapter::Term()
{
    SPX_DBG_TRACE_SCOPE("Terminating CSpxRnntRecoEngineAdapter...", "Terminating CSpxRnntRecoEngineAdapter... Done!");
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);

    if (ChangeState(RnntState::Terminating))
    {
        SPX_DBG_TRACE_VERBOSE("%s: Terminating RNN-T client (0x%8p)", __FUNCTION__, (void*)m_rnntClient.get());
#ifdef _DEBUG
        m_audioLogger.reset();
#endif
        RnntTerminate();
        ChangeState(RnntState::Zombie);
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED RNN-T State transition ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }
}

void CSpxRnntRecoEngineAdapter::SetAdapterMode(bool singleShot)
{
    SPX_DBG_TRACE_VERBOSE("%s: singleShot=%d", __FUNCTION__, singleShot);
    m_singleShot = singleShot;
}

void CSpxRnntRecoEngineAdapter::OpenConnection(bool singleShot)
{
    SPX_DBG_TRACE_VERBOSE("%s: Open connection.", __FUNCTION__);

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_RNNT_SITE_FAILURE);
    const char* recoModePropertyName = GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode);
    std::string currentRecoMode = properties->GetStringValue(recoModePropertyName);
    std::string recoModeToSet;

    // Get recognizer type.
    uint16_t countSpeech, countIntent, countTranslation, countDialog, countTranscriber;
    GetSite()->GetScenarioCount(&countSpeech, &countIntent, &countTranslation, &countDialog, &countTranscriber);
    SPX_DBG_ASSERT(countSpeech + countIntent + countTranslation + countDialog + countTranscriber == 1); // currently only support one recognizer

    if (countSpeech == 1)
    {
        recoModeToSet = singleShot ? g_recoModeInteractive : g_recoModeConversation;
    }
    else
    {
        SPX_DBG_TRACE_INFO("%s: RNN-T engine does not support non-speech recognition scenarios.", __FUNCTION__);
        SPX_THROW_HR(SPXERR_CHANGE_CONNECTION_STATUS_NOT_ALLOWED);
        return;
    }

    // Set reco mode.
    if (currentRecoMode.empty())
    {
        properties->SetStringValue(recoModePropertyName, recoModeToSet.c_str());
    }
    else
    {
        // If the reco mode is set to dictation (which can only be set before starting any recognition), just use it.
        // But switching between interactive and conversation after connection setup is not allowed.
        SPX_IFTRUE_THROW_HR((currentRecoMode.compare(g_recoModeDictation) != 0 && currentRecoMode != recoModeToSet), SPXERR_SWITCH_MODE_NOT_ALLOWED);
    }

    // Initialize the RNN-T engine.
    EnsureRnntInit();
}

void CSpxRnntRecoEngineAdapter::CloseConnection()
{
    SPX_DBG_TRACE_VERBOSE("%s: Close connection.", __FUNCTION__);

    // Terminate the RNN-T engine.
    RnntTerminate();
}

void CSpxRnntRecoEngineAdapter::SetFormat(const SPXWAVEFORMATEX* pformat)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);

    SPX_DBG_TRACE_VERBOSE_IF(pformat == nullptr, "%s - pformat == nullptr", __FUNCTION__);
    SPX_DBG_TRACE_VERBOSE_IF(pformat != nullptr, "%s\n  wFormatTag:      %s\n  nChannels:       %d\n  nSamplesPerSec:  %d\n  nAvgBytesPerSec: %d\n  nBlockAlign:     %d\n  wBitsPerSample:  %d\n  cbSize:          %d",
        __FUNCTION__,
        pformat->wFormatTag == WAVE_FORMAT_PCM ? "PCM" : std::to_string(pformat->wFormatTag).c_str(),
        pformat->nChannels,
        pformat->nSamplesPerSec,
        pformat->nAvgBytesPerSec,
        pformat->nBlockAlign,
        pformat->wBitsPerSample,
        pformat->cbSize);

    SPX_THROW_HR_IF(SPXERR_UNSUPPORTED_FORMAT,
        pformat != nullptr &&
        (pformat->wFormatTag != WAVE_FORMAT_PCM ||
            pformat->nChannels != CHANNELS ||
            pformat->nSamplesPerSec != SAMPLES_PER_SECOND ||
            pformat->wBitsPerSample != BITS_PER_SAMPLE));

    if (IsState(RnntState::Zombie))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/rnntState=%d/%d) RNNT-ZOMBIE", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }
    else if (IsBadState() && !IsState(RnntState::Terminating))
    {
        // In case of error, there still can be some calls to SetFormat in flight.
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
        if (pformat == nullptr)
        {
            // We still need to do this in order for the session to not go into a non-recoverable state.
            InvokeOnSite([this](const SitePtr& p) { p->AdapterCompletedSetFormatStop(this); });
        }
    }
    else if (pformat != nullptr && IsState(RnntState::Idle) && ChangeState(AudioState::Idle, AudioState::Ready))
    {
        // We could call site when errors happen.
        // The m_audioState is Ready at this time. So, if we have two SetFormat calls in a row, the next one won't come in here
        // it goes to the else.
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p)->PrepareFirstAudioReadyState()", __FUNCTION__, (void*)this);
        PrepareFirstAudioReadyState(pformat);
    }
    else if (pformat == nullptr && (ChangeState(AudioState::Idle) || IsState(RnntState::Terminating)))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) site->AdapterCompletedSetFormatStop()", __FUNCTION__, (void*)this);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterCompletedSetFormatStop(this); });

        m_format.reset();
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED RNN-T State transition ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }
}

void CSpxRnntRecoEngineAdapter::ProcessAudio(const DataChunkPtr& audioChunk)
{
    auto size = audioChunk->size;
    if (IsState(RnntState::Zombie) && size == 0)
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... size=0 ... (audioState/rnntState=%d/%d) RNNT-ZOMBIE", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }
    else if (IsBadState())
    {
        // In case of error, there still can be some calls to ProcessAudio in flight.
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }
    else if (size > 0 && ChangeState(AudioState::Ready, RnntState::Idle, AudioState::Sending, RnntState::WaitingForTurnStart))
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterStartingTurn()", __FUNCTION__);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterStartingTurn(this); });

#ifdef _DEBUG
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p)->PrepareRnntAudioStream() ... size=%d", __FUNCTION__, (void*)this, size);
        PrepareRnntAudioStream();
#endif
        ProcessAudioChunk(audioChunk);
    }
    else if (size > 0 && IsState(AudioState::Sending))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) Sending Audio ... size=%d", __FUNCTION__, (void*)this, size);
        ProcessAudioChunk(audioChunk);
    }
    else if (size == 0 && IsState(AudioState::Sending))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) Flushing Audio ... size=0 RNNT-FLUSH", __FUNCTION__, (void*)this);
        FlushAudio();
    }
    else if (!IsState(AudioState::Sending))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) Ignoring audio size=%d ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, size, m_audioState, m_rnntState);
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED RNN-T State transition ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }
}

void CSpxRnntRecoEngineAdapter::EnsureRnntInit()
{
    if (m_rnntClient == nullptr)
    {
        RnntInitialize();
    }

    // Start RNN-T client.
    if (m_rnntClient != nullptr && !m_rnntClient->Running())
    {
        m_rnntClient->Start();
    }
}

void CSpxRnntRecoEngineAdapter::RnntInitialize()
{
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);
    SPX_IFTRUE_THROW_HR(m_rnntClient != nullptr, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(m_rnntCallbacks != nullptr, SPXERR_ALREADY_INITIALIZED);

    // Get the named property service.
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_RNNT_SITE_FAILURE);

    // Get the thread service.
    auto threadService = SpxQueryService<ISpxThreadService>(GetSite());
    SPX_IFTRUE_THROW_HR(threadService == nullptr, SPXERR_UNEXPECTED_RNNT_SITE_FAILURE);

    auto modelSpec = PAL::ToWString(properties->GetStringValue("CARBON-INTERNAL-RNNT-ModelSpec"));
    if (modelSpec.empty())
    {
        SPX_TRACE_ERROR("The property CARBON-INTERNAL-RNNT-ModelSpec has an empty value. RNN-T model spec is required.");
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }

    auto tokenFile = PAL::ToWString(properties->GetStringValue("CARBON-INTERNAL-RNNT-Tokens"));
    if (tokenFile.empty())
    {
        SPX_TRACE_ERROR("The property CARBON-INTERNAL-RNNT-Tokens has an empty value. RNN-T token file name is required.");
        SPX_THROW_HR(SPXERR_INVALID_ARG);
    }

    auto displayText = PAL::ToBool(properties->GetStringValue("CARBON-INTERNAL-RNNT-DisplayText", "true"));
    auto maxEndPaddingDim = std::stoul(properties->GetStringValue("CARBON-INTERNAL-RNNT-MaxEndPaddingDim", "0"));

    // Default timeout settings are as follows:
    // Decoder silence timeout = 50 frames * 30 ms per frame = 1.5 seconds
    // Start timeout = 166 ~= 5 seconds
    // Total audio length = 333 ~= 10 seconds
    auto decoderInSilenceTimeout = std::stoul(properties->GetStringValue("CARBON-INTERNAL-RNNT-DecoderInSilenceTimeout", "50"));
    auto startTimeout = std::stoul(properties->GetStringValue("CARBON-INTERNAL-RNNT-StartTimeout", "166"));
    auto totalAudioLengthTimeout = std::stoul(properties->GetStringValue("CARBON-INTERNAL-RNNT-TotalAudioLengthTimeout", "333"));

    // Create the RNN-T client.
    m_rnntCallbacks = SpxCreateObjectWithSite<ISpxRnntCallbacks>("CSpxRnntCallbackWrapper", this);
    try
    {
        m_rnntClient = RNNT::CreateRnntClient(m_rnntCallbacks, threadService, modelSpec, tokenFile, displayText, maxEndPaddingDim);
    }
    catch (const std::exception& e)
    {
        SPX_TRACE_ERROR("Error: Failed to create RNN-T client: %s", e.what());
        OnError(e.what());
    }
    catch (...)
    {
        SPX_TRACE_ERROR("Error: Unexpected exception occurred while creating RNN-T client");
        OnError("Error: Unexpected exception occurred while creating RNN-T client");
    }
    SPX_IFTRUE_RETURN(m_rnntClient == nullptr);

    SetRnntRecoMode(properties, m_rnntClient);
    m_rnntClient->SetSegmentationTimeouts(decoderInSilenceTimeout, startTimeout, totalAudioLengthTimeout);
}

void CSpxRnntRecoEngineAdapter::RnntTerminate()
{
    // Stop RNN-T client.
    if (m_rnntClient != nullptr && m_rnntClient->Running())
    {
        m_rnntClient->Stop();
    }

    // Term the callbacks first and then delete the client object.
    SpxTermAndClear(m_rnntCallbacks); // After this ... we won't be called back on ISpxRnntCallbacks ever again...

    // NOTE: Even if we are in a callback from the RNN-T on another thread, we won't be destroyed until those functions release their shared ptrs
    m_rnntClient.reset();
}

void CSpxRnntRecoEngineAdapter::SetRnntRecoMode(const std::shared_ptr<ISpxNamedProperties>& properties, RNNT::IRnntClientPtr& rnntClient)
{
    // Set reco mode based on recognizer type.
    RNNT::RecognitionMode mode = RNNT::RecognitionMode::Interactive;
    SPXHR checkHr = SPX_NOERROR;
    // Reco mode should be already set in properties.
    checkHr = GetRecoModeFromProperties(properties, mode);
    SPX_THROW_HR_IF(checkHr, SPX_FAILED(checkHr));
    m_isInteractiveMode = mode == RNNT::RecognitionMode::Interactive;
    rnntClient->SetRecognitionMode(mode);
    SPX_DBG_TRACE_VERBOSE("%s: recoMode=%d", __FUNCTION__, mode);
}

SPXHR CSpxRnntRecoEngineAdapter::GetRecoModeFromProperties(const std::shared_ptr<ISpxNamedProperties>& properties, RNNT::RecognitionMode& recoMode) const
{
    SPXHR hr = SPX_NOERROR;

    // Get the property that indicates what reco mode to use.
    auto value = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode));

    if (value.empty())
    {
        hr = SPXERR_NOT_FOUND;
    }
    else if (PAL::stricmp(value.c_str(), g_recoModeInteractive) == 0)
    {
        recoMode = RNNT::RecognitionMode::Interactive;
    }
    else if (PAL::stricmp(value.c_str(), g_recoModeConversation) == 0)
    {
        recoMode = RNNT::RecognitionMode::Conversation;
    }
    else if (PAL::stricmp(value.c_str(), g_recoModeDictation) == 0)
    {
        recoMode = RNNT::RecognitionMode::Dictation;
    }
    else
    {
        SPX_DBG_ASSERT_WITH_MESSAGE(false, "Unknown RecognitionMode in RNNT::RecognitionMode.");
        SPX_TRACE_ERROR("Unknown RecognitionMode value %s", value.c_str());
        hr = SPXERR_INVALID_ARG;
    }

    return hr;
}

void CSpxRnntRecoEngineAdapter::ProcessAudioChunk(const DataChunkPtr& audioChunk)
{
    RnntWrite(audioChunk);

#ifdef _DEBUG
    if (m_audioLogger != nullptr)
    {
        m_audioLogger->WriteAudio(audioChunk->data.get(), audioChunk->size);
    }
#endif
}

void CSpxRnntRecoEngineAdapter::RnntWrite(const DataChunkPtr& audioChunk)
{
    SPX_DBG_TRACE_VERBOSE("%s(..., %d)", __FUNCTION__, audioChunk->size);
    SPX_DBG_ASSERT(m_rnntClient != nullptr || IsState(RnntState::Terminating) || IsState(RnntState::Zombie));
    if (!IsState(RnntState::Terminating) && !IsState(RnntState::Zombie) && m_rnntClient != nullptr)
    {
        m_rnntClient->ProcessAudio(audioChunk);
    }
    else
    {
        SPX_TRACE_ERROR("%s: unexpected RNN-T engine state:%d. Not sending audio chunk (size=%d).", __FUNCTION__, m_rnntState, audioChunk->size);
    }
}

void CSpxRnntRecoEngineAdapter::FlushAudio()
{
    // We should only ever be asked to Flush when we're in a valid state.
    SPX_DBG_ASSERT(m_rnntClient != nullptr || IsState(RnntState::Terminating) || IsState(RnntState::Zombie));
    if (!IsState(RnntState::Terminating) && !IsState(RnntState::Zombie) && m_rnntClient != nullptr)
    {
        m_rnntClient->FlushAudio();
    }
}

void CSpxRnntRecoEngineAdapter::OnSpeechStartDetected(const RNNT::SpeechStartDetectedMsg& message)
{
    // The message for SpeechStartDetected isn't what it might sound like in all "reco modes".
    // * In INTERACTIVE mode, it works as it sounds. It indicates the beginning of speech for the "phrase" message that will arrive later.
    // * In CONTINUOUS modes, however, it corresponds to the time of the beginning of speech for the FIRST "phrase" message of many inside one turn.

    SPX_DBG_TRACE_VERBOSE("Response: SpeechStartDetected message. Speech starts at offset %" PRIu64 " (100ns).\n", message.offset);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/rnntState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_rnntState, IsState(RnntState::Terminating) ? "(RNNT-TERMINATING)" : "********** RNNT-UNEXPECTED !!!!!!");
    }
    else if (IsState(RnntState::WaitingForPhrase))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) site->AdapterDetectedSpeechStart()", __FUNCTION__, (void*)this);
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterDetectedSpeechStart(this, message.offset); });
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED RNN-T State transition ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }
}

void CSpxRnntRecoEngineAdapter::OnSpeechEndDetected(const RNNT::SpeechEndDetectedMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: SpeechEndDetected message. Speech ends at offset %" PRIu64 " (100ns)\n", message.offset);

    auto requestMute = ChangeState(AudioState::Sending, AudioState::Mute);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/rnntState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_rnntState, IsState(RnntState::Terminating) ? "(RNNT-TERMINATING)" : "********** RNNT-UNEXPECTED !!!!!!");
    }
    else if (IsStateBetweenIncluding(RnntState::WaitingForPhrase, RnntState::WaitingForTurnEnd) && (IsState(AudioState::Idle) || IsState(AudioState::Mute)))
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) site->AdapterDetectedSpeechEnd()", __FUNCTION__, (void*)this);
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterDetectedSpeechEnd(this, message.offset); });
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED RNN-T State transition ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
        return;
    }

    SPX_DBG_TRACE_VERBOSE("%s: Flush ... (audioState/rnntState=%d/%d)  RNNT-FLUSH", __FUNCTION__, m_audioState, m_rnntState);

    FlushAudio();
    if (requestMute && !IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterRequestingAudioMute(true) ... (audioState/rnntState=%d/%d)", __FUNCTION__, m_audioState, m_rnntState);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterRequestingAudioMute(this, true); });
    }
}

void CSpxRnntRecoEngineAdapter::OnSpeechHypothesis(const RNNT::SpeechHypothesisMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: SpeechHypothesis message. Starts at offset %" PRIu64 ", with duration %" PRIu64 " (100ns). Text: %ls\n", message.offset, message.duration, message.text.c_str());

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/rnntState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_rnntState, IsState(RnntState::Terminating) ? "(RNNT-TERMINATING)" : "********** RNNT-UNEXPECTED !!!!!!");
    }
    else if (IsState(RnntState::WaitingForPhrase))
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->FireAdapterResult_Intermediate()", __FUNCTION__);

        InvokeOnSite([&](const SitePtr& site)
            {
                auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
                auto result = factory->CreateIntermediateResult(message.text.c_str(), message.offset, message.duration);
                site->FireAdapterResult_Intermediate(this, message.offset, result);
            });
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED RNN-T State transition ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }
}

void CSpxRnntRecoEngineAdapter::OnSpeechPhrase(const RNNT::SpeechPhraseMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: SpeechPhrase message. Status: %d, Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 " (100ns).\n", message.recognitionStatus, message.text.c_str(), message.offset, message.duration);
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/rnntState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_rnntState, IsState(RnntState::Terminating) ? "(RNNT-TERMINATING)" : "********** RNNT-UNEXPECTED !!!!!!");
    }
    else if ((m_isInteractiveMode && ChangeState(RnntState::WaitingForPhrase, RnntState::WaitingForTurnEnd)) ||
        (!m_isInteractiveMode && ChangeState(RnntState::WaitingForPhrase, RnntState::WaitingForPhrase)))
    {
        if (message.recognitionStatus == RNNT::RecognitionStatus::EndOfDictation)
        {
            InvokeOnSite([&](const SitePtr& site)
                {
                    site->AdapterEndOfDictation(this, message.offset, message.duration);
                });
        }
        else
        {
            SPX_DBG_TRACE_VERBOSE("%s: FireFinalResultNow()", __FUNCTION__);
            FireFinalResultNow(message);
        }
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED RNN-T State transition ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }
}

void CSpxRnntRecoEngineAdapter::OnTurnStart(const RNNT::TurnStartMsg& message)
{
    SPX_DBG_TRACE_VERBOSE("Response: TurnStart message. Context.Tag: %s\n", message.contextTag.c_str());
    SPX_DBG_TRACE_VERBOSE("%s: this=0x%8p", __FUNCTION__, (void*)this);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/rnntState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_rnntState, IsState(RnntState::Terminating) ? "(RNNT-TERMINATING)" : "********** RNNT-UNEXPECTED !!!!!!");
    }
    else if (ChangeState(RnntState::WaitingForTurnStart, RnntState::WaitingForPhrase))
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterStartedTurn()", __FUNCTION__);
        SPX_DBG_ASSERT(GetSite());
        InvokeOnSite([this, &message](const SitePtr& p) { p->AdapterStartedTurn(this, message.contextTag); });
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED RNN-T State transition ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }

#ifdef _DEBUG
    if (m_audioLogger)
    {
        m_audioLogger->SetDumpInstanceId(message.contextTag);
    }
#endif
}

void CSpxRnntRecoEngineAdapter::OnTurnEnd()
{
    SPX_DBG_TRACE_SCOPE("CSpxRnntRecoEngineAdapter::OnTurnEnd ... started... ", "CSpxRnntRecoEngineAdapter::OnTurnEnd ... DONE!");
    SPX_DBG_TRACE_VERBOSE("Response: TurnEnd message.\n");

    auto adapterTurnStopped = false;

    auto prepareReady = !m_singleShot &&
        (ChangeState(AudioState::Sending, AudioState::Ready) ||
            ChangeState(AudioState::Mute, AudioState::Ready));

    auto requestMute = m_singleShot && ChangeState(AudioState::Sending, AudioState::Mute);

    if (IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: (0x%8p) IGNORING... (audioState/rnntState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_rnntState, IsState(RnntState::Terminating) ? "(RNNT-TERMINATING)" : "********** RNNT-UNEXPECTED !!!!!!");
    }
    else if ((m_isInteractiveMode && ChangeState(RnntState::WaitingForTurnEnd, RnntState::Idle)) ||
        (!m_isInteractiveMode && ChangeState(RnntState::WaitingForPhrase, RnntState::Idle)))
    {
        adapterTurnStopped = true;
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED RNN-T State transition ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }

    if (prepareReady && !IsBadState())
    {
        SPX_DBG_TRACE_VERBOSE("%s: PrepareAudioReadyState()", __FUNCTION__);
        PrepareAudioReadyState();

        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterRequestingAudioMute(false) ... (audioState/rnntState=%d/%d)", __FUNCTION__, m_audioState, m_rnntState);
        InvokeOnSite([this](const SitePtr& p) { p->AdapterRequestingAudioMute(this, false); });
    }

    auto site = GetSite();
    if (!site)
    {
        return;
    }

    if (adapterTurnStopped)
    {
        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterStoppedTurn()", __FUNCTION__);
        site->AdapterStoppedTurn(this);
    }

    if (requestMute)
    {
        SPX_DBG_TRACE_VERBOSE("%s: FlushAudio()  RNNT-FLUSH", __FUNCTION__);
        FlushAudio();

        SPX_DBG_TRACE_VERBOSE("%s: site->AdapterRequestingAudioMute(true) ... (audioState/rnntState=%d/%d)", __FUNCTION__, m_audioState, m_rnntState);
        site->AdapterRequestingAudioMute(this, true);
    }

#ifdef _DEBUG
    m_audioLogger.reset();
#endif
}

void CSpxRnntRecoEngineAdapter::OnError(const std::string& errorMessage)
{
    SPX_TRACE_ERROR("Response: On Error: Message: %s.\n", errorMessage.c_str());

    if (IsBadState())
    {
        SPX_TRACE_ERROR("%s: (0x%8p) IGNORING... (audioState/rnntState=%d/%d) %s", __FUNCTION__, (void*)this, m_audioState, m_rnntState, IsState(RnntState::Terminating) ? "(RNNT-TERMINATING)" : "********** RNNT-UNEXPECTED !!!!!!");
    }
    else if (ChangeState(RnntState::Error))
    {
        SPX_TRACE_ERROR("%s: site->Error() ... error='%s'", __FUNCTION__, errorMessage.c_str());
        InvokeOnSite([this, errorMessage](const SitePtr& p) { p->Error(this, ErrorInfo::FromRuntimeMessage(errorMessage)); });
    }
    else
    {
        SPX_TRACE_ERROR("%s: (0x%8p) UNEXPECTED RNN-T State transition ... (audioState/rnntState=%d/%d)", __FUNCTION__, (void*)this, m_audioState, m_rnntState);
    }
}

ResultReason CSpxRnntRecoEngineAdapter::ToReason(RNNT::RecognitionStatus rnntRecognitionStatus)
{
    switch (rnntRecognitionStatus)
    {
    case RNNT::RecognitionStatus::Success:
        return ResultReason::RecognizedSpeech;

    case RNNT::RecognitionStatus::NoMatch:
    case RNNT::RecognitionStatus::InitialSilenceTimeout:
    case RNNT::RecognitionStatus::InitialBabbleTimeout:
        return ResultReason::NoMatch;

    case RNNT::RecognitionStatus::Error:
        return ResultReason::Canceled;

    default:
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to ResultReason.", rnntRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        return ResultReason::Canceled;
    }
}

CancellationReason CSpxRnntRecoEngineAdapter::ToCancellationReason(RNNT::RecognitionStatus rnntRecognitionStatus)
{
    switch (rnntRecognitionStatus)
    {
    case RNNT::RecognitionStatus::Success:
    case RNNT::RecognitionStatus::NoMatch:
    case RNNT::RecognitionStatus::InitialSilenceTimeout:
    case RNNT::RecognitionStatus::InitialBabbleTimeout:
        return REASON_CANCELED_NONE;

    case RNNT::RecognitionStatus::Error:
        return CancellationReason::Error;

    default:
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to CancellationReason.", rnntRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        return CancellationReason::Error;
    }
}

NoMatchReason CSpxRnntRecoEngineAdapter::ToNoMatchReason(RNNT::RecognitionStatus rnntRecognitionStatus)
{
    switch (rnntRecognitionStatus)
    {
    case RNNT::RecognitionStatus::Success:
    case RNNT::RecognitionStatus::Error:
        return NO_MATCH_REASON_NONE;

    case RNNT::RecognitionStatus::NoMatch:
        return NoMatchReason::NotRecognized;

    case RNNT::RecognitionStatus::InitialSilenceTimeout:
        return NoMatchReason::InitialSilenceTimeout;

    case RNNT::RecognitionStatus::InitialBabbleTimeout:
        return NoMatchReason::InitialBabbleTimeout;

    default:
        SPX_TRACE_ERROR("Unexpected recognition status %d when converting to NoMatchReason.", rnntRecognitionStatus);
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
        return NO_MATCH_REASON_NONE;
    }
}

void CSpxRnntRecoEngineAdapter::FireFinalResultNow(const RNNT::SpeechPhraseMsg& message)
{
    SPX_DBG_TRACE_SCOPE("FireFinalResultNow: Creating Result", "FireFinalResultNow: GetSite()->FireAdapterResult_FinalResult()  complete!");

    InvokeOnSite([&](const SitePtr& site)
        {
            // Create the result.
            auto factory = SpxQueryService<ISpxRecoResultFactory>(site);

            auto cancellationReason = ToCancellationReason(message.recognitionStatus);
            if (cancellationReason != REASON_CANCELED_NONE)
            {
                // The status above should have been treated as error event, so this should not happen here.
                SPX_TRACE_ERROR("Unexpected recognition status %d.", message.recognitionStatus);
                SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
            }
            auto result = factory->CreateFinalResult(ToReason(message.recognitionStatus), ToNoMatchReason(message.recognitionStatus), message.text.c_str(), message.offset, message.duration);

            site->FireAdapterResult_FinalResult(this, message.offset, result);
        });
}

bool CSpxRnntRecoEngineAdapter::ChangeState(AudioState fromAudioState, RnntState fromRnntState, AudioState toAudioState, RnntState toRnntState)
{
    if (fromAudioState == m_audioState &&       // are we in correct audio state, and ...
        fromRnntState == m_rnntState &&         // are we in correct RNN-T state? ... if so great! but ...
        ((fromRnntState != RnntState::Error &&          // don't allow transit from Error
            fromRnntState != RnntState::Zombie &&       // don't allow transit from Zombie
            fromRnntState != RnntState::Terminating) || // don't allow transit from Terminating ...
            ((fromRnntState == toRnntState) ||          // unless we're staying in that same RNN-T state
                (fromRnntState == RnntState::Error &&           // or we're going from Error to Terminating
                    toRnntState == RnntState::Terminating) ||
                (fromRnntState == RnntState::Terminating &&     // or we're going from Terminating to Zombie
                    toRnntState == RnntState::Zombie))))
    {
        SPX_DBG_TRACE_VERBOSE("%s; audioState/rnntState: %d/%d => %d/%d %s%s%s%s%s", __FUNCTION__,
            fromAudioState, fromRnntState,
            toAudioState, toRnntState,
            toRnntState == RnntState::Error ? "RNNT-ERRORERROR" : "",
            (fromAudioState == AudioState::Idle && fromRnntState == RnntState::Idle &&
                toAudioState == AudioState::Ready && toRnntState == RnntState::Idle) ? "RNNT-START" : "",
            (toAudioState == AudioState::Idle && toRnntState == RnntState::Idle) ? "RNNT-DONE" : "",
            toRnntState == RnntState::Terminating ? "RNNT-TERMINATING" : "",
            toRnntState == RnntState::Zombie ? "RNNT-ZOMBIE" : ""
        );

        m_audioState = toAudioState;
        m_rnntState = toRnntState;
        return true;
    }

    return false;
}

void CSpxRnntRecoEngineAdapter::PrepareFirstAudioReadyState(const SPXWAVEFORMATEX* format)
{
    SPX_DBG_ASSERT(IsState(AudioState::Ready, RnntState::Idle));

    auto sizeOfFormat = sizeof(SPXWAVEFORMATEX) + format->cbSize;
    m_format = SpxAllocWAVEFORMATEX(sizeOfFormat);
    memcpy(m_format.get(), format, sizeOfFormat);

    PrepareAudioReadyState();
}

void CSpxRnntRecoEngineAdapter::PrepareAudioReadyState()
{
    if (!IsState(AudioState::Ready, RnntState::Idle))
    {
        SPX_TRACE_ERROR("wrong state in PrepareAudioReadyState current audio state %d, rnnt state %d", m_audioState, m_rnntState);
    }

    EnsureRnntInit();
}

bool CSpxRnntRecoEngineAdapter::ShouldResetAfterError()
{
    return m_allowRnntResetAfterError && IsState(RnntState::Idle);
}

void CSpxRnntRecoEngineAdapter::ResetAfterError()
{
    SPX_DBG_ASSERT(ShouldResetAfterError());

    // Let's terminate our current rnnt engine and sever our callbacks.
    RnntTerminate();

    // Let's get ready for more audio!!!
    PrepareAudioReadyState();
}

#ifdef _DEBUG
void CSpxRnntRecoEngineAdapter::PrepareRnntAudioStream()
{
    SetupAudioDumpFile();
    ProcessAudioFormat(m_format.get());
}

void CSpxRnntRecoEngineAdapter::ProcessAudioFormat(SPXWAVEFORMATEX* pformat)
{
    if (m_audioLogger != nullptr)
    {
        auto wavHeaderDataPtr = MakeDataChunkForAudioFormat(pformat);
        m_audioLogger->WriteAudio(wavHeaderDataPtr->data.get(), wavHeaderDataPtr->size);
    }
}

void CSpxRnntRecoEngineAdapter::SetupAudioDumpFile()
{
    m_audioLogger.reset();

    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    auto audioDumpDir = properties->GetStringValue("CARBON-INTERNAL-DumpAudioToDir");
    SPX_DBG_TRACE_VERBOSE("CARBON-INTERNAL-DumpAudioToDir : %s", audioDumpDir.c_str());
    m_audioLogger = CSpxAudioFileLogger::Setup(audioDumpDir);
}

DataChunkPtr CSpxRnntRecoEngineAdapter::MakeDataChunkForAudioFormat(SPXWAVEFORMATEX* pformat)
{
    static const uint16_t cbTag = 4;
    static const uint16_t cbChunkType = 4;
    static const uint16_t cbChunkSize = 4;

    uint32_t cbFormatChunk = sizeof(SPXWAVEFORMAT) + pformat->cbSize;
    uint32_t cbRiffChunk = 0;
    uint32_t cbDataChunk = 0;

    uint32_t cbHeader =
        cbTag + cbChunkSize +       // 'RIFF' #size_of_RIFF#
        cbChunkType +               // 'WAVE'
        cbChunkType + cbChunkSize + // 'fmt ' #size_fmt#
        cbFormatChunk +             // actual format
        cbChunkType + cbChunkSize;  // 'data' #size_of_data#

    // Allocate the buffer, and create a ptr we'll use to advance through the buffer as we're writing stuff into it
    auto buffer = SpxAllocSharedAudioBuffer(cbHeader);
    auto ptr = buffer.get();

    // The 'RIFF' header (consists of 'RIFF' followed by size of payload that follows)
    ptr = FormatBufferWriteChars(ptr, "RIFF", cbTag);
    ptr = FormatBufferWriteNumber(ptr, cbRiffChunk);

    // The 'WAVE' chunk header
    ptr = FormatBufferWriteChars(ptr, "WAVE", cbChunkType);

    // The 'fmt ' chunk (consists of 'fmt ' followed by the total size of the SPXWAVEFORMAT(EX)(TENSIBLE), followed by the SPXWAVEFORMAT(EX)(TENSIBLE)
    ptr = FormatBufferWriteChars(ptr, "fmt ", cbChunkType);
    ptr = FormatBufferWriteNumber(ptr, cbFormatChunk);
    ptr = FormatBufferWriteBytes(ptr, (uint8_t*)pformat, cbFormatChunk);

    // The 'data' chunk is next
    ptr = FormatBufferWriteChars(ptr, "data", cbChunkType);
    ptr = FormatBufferWriteNumber(ptr, cbDataChunk);

    SPX_DBG_ASSERT(cbHeader == uint32_t(ptr - buffer.get()));
    auto wavHeaderDataPtr = std::make_shared<DataChunk>(buffer, cbHeader);
    wavHeaderDataPtr->isWavHeader = true;

    return wavHeaderDataPtr;
}

uint8_t* CSpxRnntRecoEngineAdapter::FormatBufferWriteBytes(uint8_t* buffer, const uint8_t* source, size_t bytes)
{
    std::memcpy(buffer, source, bytes);
    return buffer + bytes;
}

uint8_t* CSpxRnntRecoEngineAdapter::FormatBufferWriteNumber(uint8_t* buffer, uint32_t number)
{
    std::memcpy(buffer, &number, sizeof(number));
    return buffer + sizeof(number);
}

uint8_t* CSpxRnntRecoEngineAdapter::FormatBufferWriteChars(uint8_t* buffer, const char* psz, size_t cch)
{
    std::memcpy(buffer, psz, cch);
    return buffer + cch;
}
#endif

}}}}

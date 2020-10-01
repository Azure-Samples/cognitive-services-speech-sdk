//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <algorithm>

#include "output_reco_adapter.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "site_helpers.h"
#include "buffer_helpers.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

void CSpxOutputRecoEngineAdapter::SetAdapterMode(bool singleShot)
{
    /* This should only be used for single shot */
    SPX_THROW_HR_IF(SPXERR_SWITCH_MODE_NOT_ALLOWED, !singleShot);
}

void CSpxOutputRecoEngineAdapter::SetFormat(const SPXWAVEFORMATEX* format)
{
    SPX_IFTRUE_THROW_HR((format != nullptr) && (m_stream != nullptr), SPXERR_ALREADY_INITIALIZED);
    if (format != nullptr)
    {
        m_stream = SpxCreateObjectWithSite<ISpxAudioDataStream>("CSpxAudioDataStream", SpxGetRootSite());
        m_stream->InitFromFormat(*format, true);
        m_stream->SetStatus(StreamStatus::NoData);
        m_sink = SpxQueryInterface<ISpxAudioOutput>(m_stream);
        m_bytesPerSecond = (format->wBitsPerSample * format->nChannels * format->nSamplesPerSec) / 8;
        InvokeOnSite([this](const SitePtr& site)
        {
            site->AdapterStartingTurn(this);
            site->AdapterStartedTurn(this, "");
        });
        if (m_expectedInTicks == 0)
        {
            UpdateStatus(StreamStatus::PartialData);
        }
    }
    else
    {
        UpdateStatus(StreamStatus::AllData);
    }

}

void CSpxOutputRecoEngineAdapter::SetMinInputSize(const uint64_t sizeInTicks)
{
    std::lock_guard<std::mutex> lk{ m_stateMutex };
    if (!m_stream || (m_stream->GetStatus() == StreamStatus::NoData))
    {
        m_expectedInTicks = sizeInTicks;
    }
}

void CSpxOutputRecoEngineAdapter::ProcessAudio(const DataChunkPtr& audioChunk)
{
    std::lock_guard<std::mutex> lk{ m_stateMutex };
    if (GetStatus() == StreamStatus::AllData)
    {
        /* We can receive the 0 byte chunk after detaching */
        SPX_THROW_HR_IF(SPXERR_INVALID_STATE, audioChunk->size != 0);
        return;
    }
    m_size += audioChunk->size;
    m_sink->Write(audioChunk->data.get(), audioChunk->size);
    if (m_expectedInTicks != 0)
    {
        uint64_t sizeInTicks = BytesToDuration<tick>(audioChunk->size, m_bytesPerSecond).count();
        m_expectedInTicks -= std::min(m_expectedInTicks, sizeInTicks);
    }
    else
    {
        SetStatus(StreamStatus::PartialData);
    }
    m_cv.notify_all();
}

void CSpxOutputRecoEngineAdapter::DetachInput()
{
    if (m_detaching.exchange(true))
    {
        return;
    }
    /* We wait until state is PartialData to detach */
    WaitForStatus(StreamStatus::PartialData);
    InvokeOnSite([this](const SitePtr& site)
    {
        auto duration = BytesToDuration<tick>(m_size, m_bytesPerSecond);

        // If our site has been holding bytes to replay them, tell it that it can discard.
        auto replayer = SpxQueryInterface<ISpxAudioReplayer>(site);
        if (nullptr != replayer)
        {
            replayer->ShrinkReplayBuffer(duration.count());
        }

        auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
        auto result = factory->CreateFinalResult(ResultReason::RecognizedSpeech, NO_MATCH_REASON_NONE, L"", 0, 0);
        site->FireAdapterResult_FinalResult(this, duration.count(), result);
        site->AdapterStoppedTurn(this);
    });
    WaitForStatus(StreamStatus::AllData);
    InvokeOnSite([this](const SitePtr& site)
    {
        site->AdapterCompletedSetFormatStop(this);
    });
}

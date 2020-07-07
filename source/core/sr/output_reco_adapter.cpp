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
        m_sink = SpxQueryInterface<ISpxAudioOutput>(m_stream);
        InitFromFormat(*format, true);
        InvokeOnSite([this](const SitePtr& site)
        {
            site->AdapterStartingTurn(this);
            site->AdapterStartedTurn(this, "");
        });
        if (m_expectedInTicks == 0)
        {
            std::lock_guard<std::mutex> lk{ m_stateMutex };
            m_status = StreamStatus::PartialData;
            m_cv.notify_all();
        }
    }
    else
    {
        std::lock_guard<std::mutex> lk{ m_stateMutex };
        if (m_status == StreamStatus::AllData)
        {
            return;
        }
        m_status = StreamStatus::AllData;
        m_cv.notify_all();
    }

}

void CSpxOutputRecoEngineAdapter::SetMinInputSize(const uint64_t sizeInTicks)
{
    std::lock_guard<std::mutex> lk{ m_stateMutex };
    if (m_status == StreamStatus::NoData)
    {
        m_expectedInTicks = sizeInTicks;
    }
}

void CSpxOutputRecoEngineAdapter::ProcessAudio(const DataChunkPtr& audioChunk)
{
    if (m_status == StreamStatus::AllData)
    {
        return;
    }
    m_size += audioChunk->size;
    m_sink->Write(audioChunk->data.get(), audioChunk->size);
    if (m_expectedInTicks != 0)
    {
        std::lock_guard<std::mutex> lk{ m_stateMutex };
        uint64_t sizeInTicks = BytesToDuration<tick>(audioChunk->size, m_bytesPerSecond).count();
        m_expectedInTicks -= std::min(m_expectedInTicks, sizeInTicks);
        m_status = StreamStatus::PartialData;
        m_cv.notify_all();
    }
}

void CSpxOutputRecoEngineAdapter::DetachInput()
{
    if (m_detaching.exchange(true))
    {
        return;
    }
    /* We wait until state is PartialData to detach */
    WaitForState(StreamStatus::PartialData);
    InvokeOnSite([this](const SitePtr& site)
    {
        auto duration = BytesToDuration<tick>(m_size, m_bytesPerSecond);
        auto factory = SpxQueryService<ISpxRecoResultFactory>(site);
        auto result = factory->CreateFinalResult(nullptr, ResultReason::RecognizedSpeech, NO_MATCH_REASON_NONE, REASON_CANCELED_NONE, CancellationErrorCode::NoError, L"", 0, 0);
        site->FireAdapterResult_FinalResult(this, duration.count(), result);
        site->AdapterStoppedTurn(this);
    });
    WaitForState(StreamStatus::AllData);
    InvokeOnSite([this](const SitePtr& site)
    {
        site->AdapterCompletedSetFormatStop(this);
    });
    return;
}

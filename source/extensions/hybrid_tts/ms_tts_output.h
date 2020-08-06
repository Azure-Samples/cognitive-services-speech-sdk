//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// ms_tts_output.h: Implementation declarations for CSpxMsTtsOutput C++ class
//

#pragma once
#include "stdafx.h"
#include "mstts.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "site_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxMsTtsOutput : public IMSTTSOutput
{
public:

    CSpxMsTtsOutput() 
    {
        m_pullAudioOutputStream = SpxCreateObjectWithSite<ISpxAudioOutput>("CSpxPullAudioOutputStream", SpxGetRootSite());
    }
    virtual ~CSpxMsTtsOutput() {}

    void SetOutput(std::shared_ptr<ISpxAudioOutput> output)
    {
        auto audioStream = SpxQueryInterface<ISpxAudioStream>(output);

        auto requiredFormatSize = audioStream->GetFormat(nullptr, 0);
        auto format = SpxAllocWAVEFORMATEX(requiredFormatSize);
        audioStream->GetFormat(format.get(), requiredFormatSize);

        SetActualOutputFormat((const MSTTSWAVEFORMATEX *)(format.get()), false);
    }

    void SetAdapter(ISpxTtsEngineAdapter* pAdapter)
    {
        m_pAdapter = pAdapter;
    }

    void SetAdapterSite(const std::shared_ptr<ISpxTtsEngineAdapterSite> pSite)
    {
        m_pSite = pSite;
    }

    void SetRequestId(std::wstring requestId)
    {
        m_requestId = requestId;
    }

    void Close()
    {
        m_pullAudioOutputStream->Close();
    }

    uint32_t GetAudioData(uint8_t* buffer, uint32_t bufferSize)
    {
        auto audioDataStream = SpxQueryInterface<ISpxAudioOutputReader>(m_pullAudioOutputStream);
        SPX_DBG_ASSERT(audioDataStream != nullptr);
        return audioDataStream->Read(buffer, bufferSize);
    }

    // --- IMSTTSOutput ---
    const MSTTSWAVEFORMATEX* GetTargetOutputFormat() override
    {
        return &m_msTtsWaveFormat;
    }

    MSTTSERROR SetActualOutputFormat(const MSTTSWAVEFORMATEX* pActualFormat, bool fChanged) override
    {
        UNUSED(fChanged);

        m_msTtsWaveFormat.wFormatTag = pActualFormat->wFormatTag;
        m_msTtsWaveFormat.nChannels = pActualFormat->nChannels;
        m_msTtsWaveFormat.nSamplesPerSec = pActualFormat->nSamplesPerSec;
        m_msTtsWaveFormat.nAvgBytesPerSec = pActualFormat->nAvgBytesPerSec;
        m_msTtsWaveFormat.nBlockAlign = pActualFormat->nBlockAlign;
        m_msTtsWaveFormat.wBitsPerSample = pActualFormat->wBitsPerSample;
        m_msTtsWaveFormat.cbSize = 0;

        return 0;
    }

    MSTTSERROR Write(const char* pWaveSamples, int32_t nBytes) override
    {
        m_pullAudioOutputStream->Write((uint8_t *)pWaveSamples, nBytes);
        auto offlineProperty = std::make_shared<std::unordered_map<std::string, std::string>>();
        offlineProperty->insert(std::make_pair<std::string, std::string>("SynthesisFinishedBy", "offline"));
        m_pSite->Write(m_pAdapter, m_requestId, (uint8_t *)pWaveSamples, nBytes, offlineProperty);
        return 0;
    }

    uint64_t GetEventInterests() override
    {
        return 0;
    }

    MSTTSERROR AddEvent(const MSTTSEVENT* pEvent) override
    {
        UNUSED(pEvent);
        return 0;
    }


private:

    CSpxMsTtsOutput(const CSpxMsTtsOutput&) = delete;
    CSpxMsTtsOutput(const CSpxMsTtsOutput&&) = delete;

    CSpxMsTtsOutput& operator=(const CSpxMsTtsOutput&) = delete;


private:

    MSTTSWAVEFORMATEX m_msTtsWaveFormat;
    std::shared_ptr<ISpxAudioOutput> m_pullAudioOutputStream;
    ISpxTtsEngineAdapter* m_pAdapter;
    std::shared_ptr<ISpxTtsEngineAdapterSite> m_pSite;
    std::wstring m_requestId;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

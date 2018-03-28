//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_reco_engine_adapter.h: Implementation declarations for CSpxUspRecoEngineAdapter C++ class
//

#pragma once
#include <memory>
#include "spxcore_common.h"
#include "ispxinterfaces.h"
#include "recognition_result.h"
#include "service_helpers.h"
#include "usp.h"



namespace CARBON_IMPL_NAMESPACE() {

class CSpxUspRecoEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxRecoEngineAdapterSite>,
    public ISpxRecoEngineAdapter,
    protected USP::Callbacks
{
public:

    CSpxUspRecoEngineAdapter();

    // --- ISpxObject

    void Init() override;
    void Term() override;

    // --- ISpxAudioProcessor

    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;

protected:
    // --- USP::Callbacks
    virtual void OnSpeechStartDetected(const USP::SpeechStartDetectedMsg& message) override;
    virtual void OnSpeechEndDetected(const USP::SpeechEndDetectedMsg& message) override;
    virtual void OnSpeechHypothesis(const USP::SpeechHypothesisMsg& message) override;
    virtual void OnSpeechFragment(const USP::SpeechFragmentMsg& message) override;
    virtual void OnSpeechPhrase(const USP::SpeechPhraseMsg& message) override;
    virtual void OnTurnStart(const USP::TurnStartMsg& message) override;
    virtual void OnTurnEnd(const USP::TurnEndMsg& message) override;
    virtual void OnError(const std::string& error) override;

private:

    CSpxUspRecoEngineAdapter(const CSpxUspRecoEngineAdapter&) = delete;
    CSpxUspRecoEngineAdapter(const CSpxUspRecoEngineAdapter&&) = delete;

    CSpxUspRecoEngineAdapter& operator=(const CSpxUspRecoEngineAdapter&) = delete;

    void UspInitialize();

    USP::Client& SetUspEndpoint(USP::Client& client);
    USP::Client& SetUspRecoMode(USP::Client& client);
    USP::Client& SetUspModelId(USP::Client& client);
    USP::Client& SetUspLanguage(USP::Client& client);
    USP::Client& SetUspAuthentication(USP::Client& client);

    void UspWrite(const uint8_t* buffer, size_t byteToWrite);
    void UspWriteFormat(WAVEFORMATEX* pformat);
    void UspWrite_Actual(const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Buffered(const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Flush();

    ISpxRecoEngineAdapterSite::ResultPayload_Type ResultPayloadFrom(const USP::SpeechPhraseMsg& message)
    {
        SPX_DBG_ASSERT(GetSite());
        auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());

        // TODO: RobCh: Do something with the other fields in USP::SpeechPhrase
        switch (message.recognitionStatus)
        {
        case USP::RecognitionStatus::Success:
            return factory->CreateFinalResult(nullptr, message.displayText.c_str());
        case USP::RecognitionStatus::NoMatch:
        case USP::RecognitionStatus::BabbleTimeout:
        case USP::RecognitionStatus::InitialSilenceTimeout:
        case USP::RecognitionStatus::Error:
            // TODO: RobCh: Construct appropriate result
            return factory->CreateNoMatchResult();
        default:
            SPX_DBG_ASSERT_WITH_MESSAGE(false, "Did someone add a new value to the USP::RecognitionStatus enumeration?");
        }
        return nullptr;
    }

    ISpxRecoEngineAdapterSite::AdditionalMessagePayload_Type AdditionalMessagePayloadFrom(const USP::TurnStartMsg& message) { UNUSED(message); return nullptr; } // TODO: RobCh: Implement this
    ISpxRecoEngineAdapterSite::AdditionalMessagePayload_Type AdditionalMessagePayloadFrom(const USP::TurnEndMsg& message) { UNUSED(message); return nullptr; } // TODO: RobCh: Implement this

    ISpxRecoEngineAdapterSite::ErrorPayload_Type ErrorPayloadFrom(const std::string& error) { UNUSED(error); return SPXERR_NOT_IMPL; } // TODO: RobCh: Implement this

    uint8_t* FormatBufferWriteBytes(uint8_t* buffer, const uint8_t* source, size_t bytes);

    uint8_t* FormatBufferWriteNumber(uint8_t* buffer, uint32_t number);

    uint8_t* FormatBufferWriteChars(uint8_t* buffer, const char* psz, size_t cch);

    uint32_t EndianConverter(uint32_t number)
    {
        return ((uint32_t)(number & 0x000000ff) << 24) |
               ((uint32_t)(number & 0x0000ff00) << 8) |
               ((uint32_t)(number & 0x00ff0000) >> 8) |
               ((uint32_t)(number & 0xff000000) >> 24);
    }

    void DumpFileInit();
    void DumpFileWrite(const uint8_t* buffer, size_t bytesToWrite);
    void DumpFileClose();


private:

    USP::ConnectionPtr m_handle;

    const size_t m_servicePreferedMilliseconds = 600;
    size_t m_servicePreferedBufferSize;

    const bool m_fUseBufferedImplementation = true;
    
    std::shared_ptr<uint8_t> m_buffer;
    size_t m_bytesInBuffer;

    uint8_t* m_ptrIntoBuffer;
    size_t m_bytesLeftInBuffer;

    FILE* m_hfile = 0;

    int static m_instanceCounter;
};


} // CARBON_IMPL_NAMESPACE

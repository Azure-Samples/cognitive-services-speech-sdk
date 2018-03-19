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

extern "C"
{
    #include "usp.h"
}


namespace CARBON_IMPL_NAMESPACE() {


class CSpxUspRecoEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxRecoEngineAdapterSite>,
    public ISpxRecoEngineAdapter
{
public:

    CSpxUspRecoEngineAdapter();

    // --- ISpxObject

    void Init() override;
    void Term() override;

    // --- ISpxAudioProcessor

    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;


private:

    CSpxUspRecoEngineAdapter(const CSpxUspRecoEngineAdapter&) = delete;
    CSpxUspRecoEngineAdapter(const CSpxUspRecoEngineAdapter&&) = delete;

    CSpxUspRecoEngineAdapter& operator=(const CSpxUspRecoEngineAdapter&) = delete;

    bool IsUspHandleValid(UspHandle handle);

    void UspInitialize(UspHandle* handle, UspCallbacks *callbacks, void* callbackContext);

    UspEndpointType GetUspEndpointType();
    std::string GetUspCustomEndpoint();

    UspRecognitionMode GetUspRecoMode();
    std::string GetUspLanguage();
    std::string GetUspModelId();

    std::string GetUspAuthenticationData(UspAuthenticationType* pauthType);

    void UspWrite(UspHandle handle, const uint8_t* buffer, size_t byteToWrite);
    void UspWriteFormat(UspHandle handle, WAVEFORMATEX* pformat);
    void UspWrite_Actual(UspHandle handle, const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Buffered(UspHandle handle, const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Flush(UspHandle handle);
    void UspShutdown(UspHandle handle);

    static std::shared_ptr<CSpxUspRecoEngineAdapter> From(UspHandle handle, void* callbackContext)
    {
        UNUSED(handle);
        auto ptrClass = static_cast<CSpxUspRecoEngineAdapter*>(callbackContext);
        auto ptrProcessor = ((ISpxAudioProcessor*)ptrClass);
        auto shared = std::dynamic_pointer_cast<CSpxUspRecoEngineAdapter>(ptrProcessor->shared_from_this());
        return shared;
    }

    void InitCallbacks(UspCallbacks* pcallbacks);

    void UspOnSpeechStartDetected(UspMsgSpeechStartDetected *message);
    void UspOnSpeechEndDetected(UspMsgSpeechEndDetected *message);
    void UspOnSpeechHypothesis(UspMsgSpeechHypothesis *message);
    void UspOnSpeechFragment(UspMsgSpeechFragment *message);
    void UspOnSpeechPhrase(UspMsgSpeechPhrase *message);
    void UspOnTurnStart(UspMsgTurnStart *message);
    void UspOnTurnEnd(UspMsgTurnEnd *message);
    void UspOnError(const UspError* error);

    ISpxRecoEngineAdapterSite::ResultPayload_Type ResultPayloadFrom(UspMsgSpeechPhrase* message)
    {
        SPX_DBG_ASSERT(GetSite());
        auto factory = SpxQueryService<ISpxRecoResultFactory>(GetSite());

        // TODO: RobCh: Do something with the other fields in UspMsgSpeechPhrase
        ISpxRecoEngineAdapterSite::ResultPayload_Type payload;
        if (message->recognitionStatus == USP_RECOGNITON_SUCCESS)
        {
            payload = factory->CreateFinalResult(nullptr, message->displayText);
        }
        else if (message->recognitionStatus == USP_RECOGNITION_NO_MATCH)
        {
            payload = factory->CreateNoMatchResult();
        }
        else if (message->recognitionStatus == USP_RECOGNITION_INITIAL_SILENCE_TIMEOUT)
        {
            // TODO: RobCh: Construct appropriate result
            payload = factory->CreateNoMatchResult();
        }
        else if (message->recognitionStatus == USP_RECOGNITION_BABBLE_TIMEOUT)
        {
            // TODO: RobCh: Construct appropriate result
            payload = factory->CreateNoMatchResult();
        }
        else if (message->recognitionStatus == USP_RECOGNITION_ERROR)
        {
            // TODO: RobCh: Construct appropriate result
            payload = factory->CreateNoMatchResult();
        }
        else
        {
            SPX_DBG_ASSERT_WITH_MESSAGE(false, "Did someone add a new RECOGNITION_* value to the UspRecognitionStatus enumeration?");
        }

        return payload;
    }

    ISpxRecoEngineAdapterSite::AdditionalMessagePayload_Type AdditionalMessagePayloadFrom(UspMsgTurnStart* message) { UNUSED(message); return nullptr; } // TODO: RobCh: Implement this
    ISpxRecoEngineAdapterSite::AdditionalMessagePayload_Type AdditionalMessagePayloadFrom(UspMsgTurnEnd* message) { UNUSED(message); return nullptr; } // TODO: RobCh: Implement this

    ISpxRecoEngineAdapterSite::ErrorPayload_Type ErrorPayloadFrom(const UspError* error) { return error->errorCode; } // TODO: RobCh: Implement this

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

    UspHandle m_handle;
    UspCallbacks m_callbacks; // TODO: ZhouWang: When I call UspInitialize(..., &m_callbacks, ...), if I give you a stack based variable, it doesn't work. you're not making a copy? So ... for now I'll just keep this as a member variable

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

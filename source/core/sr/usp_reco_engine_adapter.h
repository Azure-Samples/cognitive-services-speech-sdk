//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_reco_engine_adapter.h: Implementation declarations for CSpxUspRecoEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <spxcore_common.h>
#include "ispxinterfaces.h"
#include "session.h"
#include "recognition_result.h"

extern "C"
{
    #include "usp.h"
};


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

    std::shared_ptr<ISpxRecoEngineAdapterSite> GetSite()
    {
        return m_site.lock();
    }

    bool IsUspHandleValid(UspHandle handle);

    void UspInitialize(UspHandle* handle, UspCallbacks *callbacks, void* callbackContext);
    void UspWrite(UspHandle handle, const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Actual(UspHandle handle, const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Buffered(UspHandle handle, const uint8_t* buffer, size_t byteToWrite);
    void UspWrite_Buffered_Flush();
    void UspShutdown(UspHandle handle);

    static std::shared_ptr<CSpxUspRecoEngineAdapter> From(UspHandle handle, void* callbackContext)
    {
        auto ptrClass = static_cast<CSpxUspRecoEngineAdapter*>(callbackContext);
        auto ptrProcessor = ((ISpxAudioProcessor*)ptrClass);
        auto shared = std::dynamic_pointer_cast<CSpxUspRecoEngineAdapter>(ptrProcessor->shared_from_this());
        return shared;
    }

    void InitCallbacks(UspCallbacks* pcallbacks);

    void UspOnSpeechStartDetected(UspHandle handle, void* context, UspMsgSpeechStartDetected *message);
    void UspOnSpeechEndDetected(UspHandle handle, void* context, UspMsgSpeechEndDetected *message);
    void UspOnSpeechHypothesis(UspHandle handle, void* context, UspMsgSpeechHypothesis *message);
    void UspOnSpeechPhrase(UspHandle handle, void* context, UspMsgSpeechPhrase *message);
    void UspOnTurnStart(UspHandle handle, void* context, UspMsgTurnStart *message);
    void UspOnTurnEnd(UspHandle handle, void* context, UspMsgTurnEnd *message);
    void UspOnError(UspHandle handle, void* context, UspResult error);

    ISpxRecoEngineAdapterSite::ResultPayload_Type ResultPayloadFrom(UspMsgSpeechHypothesis* message)
    {
        // TODO: RobCh: Do something with the other fields in UspMsgSpeechHypothesis
        // TODO: RobCh: Next: ResultId?
        auto result = SpxMakeShared<CSpxRecognitionResult, ISpxRecognitionResult>(nullptr, message->text);
        return result;
    }

    ISpxRecoEngineAdapterSite::ResultPayload_Type ResultPayloadFrom(UspMsgSpeechPhrase* message)
    {
        // TODO: RobCh: Do something with the other fields in UspMsgSpeechPhrase
        ISpxRecoEngineAdapterSite::ResultPayload_Type payload;
        if (message->recognitionStatus == RECOGNITON_SUCCESS)
        {
            // TODO: RobCh: Next: ResultId?
            payload = SpxMakeShared<CSpxRecognitionResult, ISpxRecognitionResult>(nullptr, message->displayText);
        }
        else if (message->recognitionStatus == RECOGNITION_NO_MATCH)
        {
            payload = SpxMakeShared<CSpxRecognitionResult, ISpxRecognitionResult>(CSpxRecognitionResult::NoMatch);
        }
        else if (message->recognitionStatus == RECOGNITION_INITIAL_SILENCE_TIMEOUT)
        {
            // TODO: RobCh: Construct appropriate result
            payload = SpxMakeShared<CSpxRecognitionResult, ISpxRecognitionResult>(CSpxRecognitionResult::NoMatch);
        }
        else if (message->recognitionStatus == RECOGNITION_BABBLE_TIMEOUT)
        {
            // TODO: RobCh: Construct appropriate result
            payload = SpxMakeShared<CSpxRecognitionResult, ISpxRecognitionResult>(CSpxRecognitionResult::NoMatch);
        }
        else if (message->recognitionStatus == RECOGNITION_ERROR)
        {
            // TODO: RobCh: Construct appropriate result
            payload = SpxMakeShared<CSpxRecognitionResult, ISpxRecognitionResult>(CSpxRecognitionResult::NoMatch);
        }
        else
        {
            SPX_DBG_ASSERT_WITH_MESSAGE(false, "Did someone add a new RECOGNITION_* value to the UspRecognitionStatus enumeration?");
        }

        return payload;
    }

    ISpxRecoEngineAdapterSite::AdditionalMessagePayload_Type AdditionalMessagePayloadFrom(UspMsgTurnStart* message) { return nullptr; } // TODO: RobCh: Implement this
    ISpxRecoEngineAdapterSite::AdditionalMessagePayload_Type AdditionalMessagePayloadFrom(UspMsgTurnEnd* message) { return nullptr; } // TODO: RobCh: Implement this

    ISpxRecoEngineAdapterSite::ErrorPayload_Type ErrorPayloadFrom(UspResult error) { return error; } // TODO: RobCh: Implement this

    uint8_t* BufferWriteBytes(uint8_t* buffer, const uint8_t* source, size_t bytes)
    {
        memcpy(buffer, source, bytes);
        return buffer + bytes;
    }

    uint8_t* BufferWriteNumber(uint8_t* buffer, uint32_t number)
    {
        memcpy(buffer, &number, sizeof(number));
        return buffer + sizeof(number);
    }

    uint8_t* BufferWriteChars(uint8_t* buffer, const char* psz, size_t cch)
    {
        memcpy(buffer, psz, cch);
        return buffer + cch;
    }

    uint32_t EndianConverter(uint32_t number)
    {
        return ((uint32_t)(number & 0x000000ff) << 24) |
               ((uint32_t)(number & 0x0000ff00) << 8) |
               ((uint32_t)(number & 0x00ff0000) >> 8) |
               ((uint32_t)(number & 0xff000000) >> 24);
    }

private:

    UspHandle m_handle;
    UspCallbacks m_callbacks; // TODO: ZhouWang: When I call UspInitialize(..., &m_callbacks, ...), if I give you a stack based variable, it doesn't work. you're not making a copy? So ... for now I'll just keep this as a member variable

    std::unique_ptr<uint8_t> m_bufferForUspWrite;
    uint8_t* m_ptrIntoBufferForUspWrite;
};


}; // CARBON_IMPL_NAMESPACE()

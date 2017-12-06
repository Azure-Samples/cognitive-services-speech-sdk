//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_reco_engine_adapter.cpp: Implementation definitions for CSpxUspRecoEngineAdapter C++ class
//

#include "stdafx.h"
#include "usp_reco_engine_adapter.h"
#include "handle_table.h"


#define INVALID_USP_HANDLE ((void*)-1)


namespace CARBON_IMPL_NAMESPACE() {


CSpxUspRecoEngineAdapter::CSpxUspRecoEngineAdapter() :
    m_handle(INVALID_USP_HANDLE)
{
    InitCallbacks(&m_callbacks);
}

void CSpxUspRecoEngineAdapter::Init()
{
    SPX_IFTRUE_THROW_HR(IsUspHandleValid(m_handle), SPXERR_ALREADY_INITIALIZED);

    UspInitialize(&m_handle, &m_callbacks, static_cast<void*>(this));
}

void CSpxUspRecoEngineAdapter::Term()
{
    UspShutdown(m_handle);
    m_handle = INVALID_USP_HANDLE;
}

void CSpxUspRecoEngineAdapter::SetFormat(WAVEFORMATEX* pformat)
{
    bool fUspImplReady = false; // TODO: RobCh: Once Zhou has the USP ready, should be able to change this to true (better: remove condition)
    SPX_IFTRUE_THROW_HR(fUspImplReady && !IsUspHandleValid(m_handle), SPXERR_UNINITIALIZED);

    if (pformat != nullptr)
    {
        static const uint16_t cbTag = 4;
        static const uint16_t cbChunkType = 4;
        static const uint16_t cbChunkSize = 4;

        uint32_t cbFormatChunk = sizeof(WAVEFORMAT) + pformat->cbSize;
        uint32_t cbRiffChunk = 0;       // NOTE: This isn't technically accurate for a RIFF/WAV file, but it's fine for Truman/Newman/Skyman
        uint32_t cbDataChunk = 0;       // NOTE: Similarly, this isn't technically correct for the 'data' chunk, but it's fine for Truman/Newman/Skyman

        size_t cbHeader =
            cbTag + cbChunkSize +       // 'RIFF' #size_of_RIFF#
            cbChunkType +               // 'WAVE'
            cbChunkType + cbChunkSize + // 'fmt ' #size_fmt#
            cbFormatChunk +             // actual format
            cbChunkType + cbChunkSize;  // 'data' #size_of_data#

        // Allocate the buffer, and create a ptr we'll use to advance thru the buffer as we're writing stuff into it
        std::unique_ptr<uint8_t> header(new uint8_t[cbHeader]);
        auto ptr = header.get();

        // The 'RIFF' header (consists of 'RIFF' followed by size of paylaod that folows)
        ptr = BufferWriteChars(ptr, "RIFF", cbTag);
        ptr = BufferWriteNumber(ptr, cbRiffChunk);

        // The 'WAVE' chunk header
        ptr = BufferWriteChars(ptr, "WAVE", cbChunkType);

        // The 'fmt ' chunk (consists of 'fmt ' followed by the total size of the WAVEFORMAT(EX)(TENSIBLE), followed by the WAVEFORMAT(EX)(TENSIBLE)
        ptr = BufferWriteChars(ptr, "fmt ", cbChunkType);
        ptr = BufferWriteNumber(ptr, cbFormatChunk);
        ptr = BufferWriteBytes(ptr, (uint8_t*)pformat, cbFormatChunk);

        // The 'data' chunk is next
        ptr = BufferWriteChars(ptr, "data", cbChunkType);
        ptr = BufferWriteNumber(ptr, cbDataChunk);

        // Now that we've prepared the header/buffer, send it along to Truman/Newman/Skyman via UspWrite
        SPX_DBG_ASSERT(cbHeader == (ptr - header.get()));
        UspWrite(m_handle, header.get(), cbHeader);
    }
}

void CSpxUspRecoEngineAdapter::ProcessAudio(AudioData_Type data, uint32_t size)
{
    UspWrite(m_handle, data.get(), size);
}

bool CSpxUspRecoEngineAdapter::IsUspHandleValid(UspHandle handle)
{
    return handle != INVALID_USP_HANDLE;
}

FILE* hfile = 0;

void CSpxUspRecoEngineAdapter::UspInitialize(UspHandle* handle, UspCallbacks *callbacks, void* callbackContext)
{
    SPX_DBG_TRACE_VERBOSE("%s(0x%x)", __FUNCTION__, handle);

    // SPX_IFFAILED_THROW_HR(::UspInitialize(handle, callbacks, callbackContext);
    SPX_IFFAILED_THROW_HR_IFNOT(::UspInitialize(handle, callbacks, callbackContext), USP_NOT_IMPLEMENTED);

    fopen_s(&hfile, "output.wav", "wb");
}

void CSpxUspRecoEngineAdapter::UspWrite(UspHandle handle, const uint8_t* buffer, size_t byteToWrite)
{
    SPX_DBG_TRACE_VERBOSE_IF(byteToWrite == 0, "%s(..., %d)", __FUNCTION__, byteToWrite);

    static const bool fBuffer = true;
    auto fn = !fBuffer
        ? &CSpxUspRecoEngineAdapter::UspWrite_Actual
        : &CSpxUspRecoEngineAdapter::UspWrite_Buffered;

    (this->*fn)(handle, buffer, byteToWrite);

    fwrite(buffer, 1, byteToWrite, hfile);
}

void CSpxUspRecoEngineAdapter::UspWrite_Actual(UspHandle handle, const uint8_t* buffer, size_t byteToWrite)
{
    SPX_INIT_HR(hr);

    hr = ::UspWrite(handle, buffer, byteToWrite);
    hr = (byteToWrite == 0 && hr == USP_WRITE_ERROR) ? SPX_NOERROR : hr; // ::UspWrite currently returns USP_WRITE_ERROR on zero bytes, but there's no other way to flush buffer...

    SPX_IFFAILED_THROW_HR(hr);
}

void CSpxUspRecoEngineAdapter::UspWrite_Buffered(UspHandle handle, const uint8_t* buffer, size_t byteToWrite)
{
    if (m_bufferForUspWrite.get() == nullptr)
    {
        auto ptr = new uint8_t[1024 * 1024];
        std::unique_ptr<uint8_t> buffer(ptr);
        m_bufferForUspWrite = std::move(buffer);

        m_ptrIntoBufferForUspWrite = m_bufferForUspWrite.get();
    }

    memcpy(m_ptrIntoBufferForUspWrite, buffer, byteToWrite);
    m_ptrIntoBufferForUspWrite += byteToWrite;

    if (byteToWrite == 0)
    {
        UspWrite_Buffered_Flush();
    }
}

void CSpxUspRecoEngineAdapter::UspWrite_Buffered_Flush()
{
    auto bufferStart = m_bufferForUspWrite.get();
    UspWrite_Actual(m_handle, bufferStart, m_ptrIntoBufferForUspWrite - bufferStart);

    m_ptrIntoBufferForUspWrite = nullptr;
    m_bufferForUspWrite = nullptr;
}

void CSpxUspRecoEngineAdapter::UspShutdown(UspHandle handle)
{
    SPX_DBG_TRACE_VERBOSE("%s(0x%x)", __FUNCTION__, handle);

    // SPX_IFFAILED_THROW_HR(::UspShutdown(handle));
    SPX_IFFAILED_THROW_HR_IFNOT(::UspShutdown(handle), USP_NOT_IMPLEMENTED);

    fclose(hfile);
}

void CSpxUspRecoEngineAdapter::InitCallbacks(UspCallbacks* pcallbacks)
{
    pcallbacks->size = sizeof(UspCallbacks);
    pcallbacks->version = (uint16_t)USP_VERSION;

    pcallbacks->onSpeechStartDetected = [](UspHandle handle, void* context, UspMsgSpeechStartDetected *message) {
        SPX_DBG_TRACE_VERBOSE("Response: Speech.StartDetected message. Speech starts at offset %u (100ns).\n", message->offset);
        CSpxUspRecoEngineAdapter::From(handle, context)->UspOnSpeechStartDetected(handle, context, message);
    };

    pcallbacks->onSpeechEndDetected = [](UspHandle handle, void* context, UspMsgSpeechEndDetected *message) {
        SPX_DBG_TRACE_VERBOSE("Response: Speech.EndDetected message. Speech ends at offset %u (100ns)\n", message->offset);
        CSpxUspRecoEngineAdapter::From(handle, context)->UspOnSpeechEndDetected(handle, context, message);
    };

    pcallbacks->onSpeechHypothesis = [](UspHandle handle, void* context, UspMsgSpeechHypothesis *message) {
        SPX_DBG_TRACE_VERBOSE("Response: Speech.Hypothesis message. Text: %S, starts at offset %u, with duration %u (100ns).\n", message->text, message->offset, message->duration);
        CSpxUspRecoEngineAdapter::From(handle, context)->UspOnSpeechHypothesis(handle, context, message);
    };

    pcallbacks->onSpeechPhrase = [](UspHandle handle, void* context, UspMsgSpeechPhrase *message) {
        SPX_DBG_TRACE_VERBOSE("Response: Speech.Phrase message. Status: %d, Text: %S, starts at %u, with duration %u (100ns).\n", message->recognitionStatus, message->displayText, message->offset, message->duration);
        CSpxUspRecoEngineAdapter::From(handle, context)->UspOnSpeechPhrase(handle, context, message);
    };

    pcallbacks->onTurnStart = [](UspHandle handle, void* context, UspMsgTurnStart *message) {
        SPX_DBG_TRACE_VERBOSE("Response: Turn.Start message. Context.ServiceTag: %S\n", message->contextServiceTag);
        CSpxUspRecoEngineAdapter::From(handle, context)->UspOnTurnStart(handle, context, message);
    };

    pcallbacks->onTurnEnd = [](UspHandle handle, void* context, UspMsgTurnEnd *message) {
        SPX_DBG_TRACE_VERBOSE("Response: Turn.End message.\n");
        CSpxUspRecoEngineAdapter::From(handle, context)->UspOnTurnEnd(handle, context, message);
    };

    pcallbacks->OnError = [](UspHandle handle, void* context, UspResult error) {
        SPX_DBG_TRACE_VERBOSE("Response: On Error: 0x%x.\n", error);
        CSpxUspRecoEngineAdapter::From(handle, context)->UspOnError(handle, context, error);
    };
}

void CSpxUspRecoEngineAdapter::UspOnSpeechStartDetected(UspHandle handle, void* context, UspMsgSpeechStartDetected *message)
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->SpeechStartDetected(this, message->offset);
}

void CSpxUspRecoEngineAdapter::UspOnSpeechEndDetected(UspHandle handle, void* context, UspMsgSpeechEndDetected *message)
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->SpeechEndDetected(this, message->offset);
}

void CSpxUspRecoEngineAdapter::UspOnSpeechHypothesis(UspHandle handle, void* context, UspMsgSpeechHypothesis *message)
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->IntermediateResult(this, message->offset, ResultPayloadFrom(message));
}

void CSpxUspRecoEngineAdapter::UspOnSpeechPhrase(UspHandle handle, void* context, UspMsgSpeechPhrase *message)
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->FinalResult(this, message->offset, ResultPayloadFrom(message));
}

void CSpxUspRecoEngineAdapter::UspOnTurnStart(UspHandle handle, void* context, UspMsgTurnStart *message)
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->AdditionalMessage(this, 0, AdditionalMessagePayloadFrom(message));
}

void CSpxUspRecoEngineAdapter::UspOnTurnEnd(UspHandle handle, void* context, UspMsgTurnEnd *message)
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->DoneProcessingAudio(this);
}

void CSpxUspRecoEngineAdapter::UspOnError(UspHandle handle, void* context, UspResult error)
{
    SPX_DBG_ASSERT(GetSite());
    GetSite()->Error(this, ErrorPayloadFrom(error));
}


}; // CARBON_IMPL_NAMESPACE()

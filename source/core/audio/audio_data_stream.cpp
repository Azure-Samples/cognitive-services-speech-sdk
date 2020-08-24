//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_data_stream.cpp: Implementation definitions for CSpxAudioDataStream C++ class
//

#include "stdafx.h"
#include "create_object_helpers.h"
#include "site_helpers.h"
#include "property_id_2_name_map.h"
#include "audio_data_stream.h"
#include <algorithm>
#include <cstring>
#include <chrono>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxAudioDataStream::~CSpxAudioDataStream()
{
    DisconnectSynthEvents();
}

void CSpxAudioDataStream::InitFromFile(const char* fileName)
{
    // open the audio file using the wav reader
    auto audioFile = SpxCreateObjectWithSite<ISpxAudioFile>("CSpxWavFileReader", GetSite());
    audioFile->Open(PAL::ToWString(fileName).c_str());

    // get the reader interface, and the format
    auto reader = SpxQueryInterface<ISpxAudioStreamReader>(audioFile);
    auto formatSize = reader->GetFormat(nullptr, 0);
    auto format = SpxAllocWAVEFORMATEX(formatSize);
    reader->GetFormat(format.get(), formatSize);

    // loop thru the audio data, 1 second at a time, copying buffers from the reader
    auto readSize = format->nAvgBytesPerSec;
    auto buffer = SpxAllocSharedUint8Buffer(readSize);
    while (readSize > 0)
    {
        readSize = reader->Read(buffer.get(), readSize);
        this->Write(buffer.get(), readSize);
    }

    // mark that we're done reading ...
    m_writingEnded = true;
}

void CSpxAudioDataStream::InitFromSynthesisResult(std::shared_ptr<ISpxSynthesisResult> result)
{
    // Set request ID
    m_requestId = result->GetRequestId();

    // Set reason
    m_beginningReason = result->GetReason();
    UpdateStatusFromReason(m_beginningReason);

    // Set audio format
    auto requiredFormatSize = result->GetFormat(nullptr, 0);
    m_format = SpxAllocWAVEFORMATEX(requiredFormatSize);
    result->GetFormat(m_format.get(), requiredFormatSize);
    m_hasHeader = result->HasHeader();

    // Set audio data
    if (result->GetReason() != ResultReason::SynthesizingAudio)
    {
        // Shouldn't set for synthesizing event
        // Since the synthesizing event for current chunk will be fired immediately after this stream is created
        auto audioData = result->GetRawAudioData();
        Write(audioData->data(), static_cast<uint32_t>(audioData->size()));
    }

    if (result->GetReason() == ResultReason::SynthesizingAudioCompleted || result->GetReason() == ResultReason::Canceled)
    {
        m_writingEnded = true;

        // Set cancellation details if it's canceled result
        if (result->GetReason() == ResultReason::Canceled)
        {
            m_cancellationReason = result->GetCancellationReason();
            m_error = result->GetError();
            auto properties = SpxQueryInterface<ISpxNamedProperties>(result);
            auto cancellationDetailedText = properties->GetStringValue(GetPropertyName(PropertyId::CancellationDetails_ReasonDetailedText));
            SetStringValue(GetPropertyName(PropertyId::CancellationDetails_ReasonDetailedText), cancellationDetailedText.c_str());
        }
    }

    // Define synthesis event handling functions for audio data stream

    m_pfnSynthesizing = [this](std::shared_ptr<ISpxSynthesisEventArgs> e) {
        std::unique_lock<std::mutex> lock(m_eventMutex);

        // Check consistency of request ID, to make sure the event is handled by the right CSpxAudioDataStream instance
        auto result = e->GetResult();
        const auto requestId = result->GetRequestId();
        if (requestId != m_requestId)
        {
            SPX_TRACE_ERROR(
                "The request id of this data stream (%s) is different from the request id of current event (%s), ignored.",
                PAL::ToString(m_requestId).c_str(), PAL::ToString(requestId).c_str());
            return;
        }

        // Update reason
        UpdateStatusFromReason(result->GetReason());

        auto audioData = result->GetRawAudioData();
        Write(audioData->data(), static_cast<uint32_t>(audioData->size()));
    };

    m_pfnSynthesisStopped = [this](std::shared_ptr<ISpxSynthesisEventArgs> e) {
        std::unique_lock<std::mutex> lock(m_eventMutex);

        // Check consistency of request ID, to make sure the event is handled by the right CSpxAudioDataStream instance
        auto result = e->GetResult();
        const auto requestId = result->GetRequestId();
        if (requestId != m_requestId)
        {
            SPX_TRACE_ERROR(
                "The request id of this data stream (%s) is different from the request id of current event (%s), ignored.",
                PAL::ToString(m_requestId).c_str(), PAL::ToString(requestId).c_str());
            return;
        }

        // Update reason
        UpdateStatusFromReason(result->GetReason());

        // Set cancellation details if it's cancellation event
        if (result->GetReason() == ResultReason::Canceled)
        {
            m_cancellationReason = result->GetCancellationReason();
            m_error = result->GetError();
            auto properties = SpxQueryInterface<ISpxNamedProperties>(result);
            auto cancellationDetailedText = properties->GetStringValue(GetPropertyName(PropertyId::CancellationDetails_ReasonDetailedText));
            SetStringValue(GetPropertyName(PropertyId::CancellationDetails_ReasonDetailedText), cancellationDetailedText.c_str());
        }

        // Close the stream
        Close();

        if (auto synthEvents = result->GetEvents())
        {
            // Disconnect synthesizing event when SynthesisCompleted/SynthesisCanceled event is fired
            synthEvents->DisconnectSynthesizingCallback(static_cast<void*>(this), m_pfnSynthesizing);

            // Disconnect synthesis completed event when SynthesisCompleted/SynthesisCanceled event is fired
            synthEvents->DisconnectSynthesisCompletedCallback(static_cast<void*>(this), m_pfnSynthesisStopped);

            // Disconnect synthesis canceled event when SynthesisCompleted/SynthesisCanceled event is fired
            synthEvents->DisconnectSynthesisCanceledCallback(static_cast<void*>(this), m_pfnSynthesisStopped);
        }
    };

    // Connect events
    if (auto synthEvents = result->GetEvents())
    {
        m_synthEvents = synthEvents;

        if (result->GetReason() == ResultReason::SynthesizingAudioStarted || result->GetReason() == ResultReason::SynthesizingAudio)
        {
            // Connect synthesizing event
            synthEvents->ConnectSynthesizingCallback(static_cast<void*>(this), m_pfnSynthesizing);

            // Connect synthesis completed event
            synthEvents->ConnectSynthesisCompletedCallback(static_cast<void*>(this), m_pfnSynthesisStopped);

            // Connect synthesis canceled event
            synthEvents->ConnectSynthesisCanceledCallback(static_cast<void*>(this), m_pfnSynthesisStopped);
        }
    }
    else
    {
        m_writingEnded = true;
    }
}

void CSpxAudioDataStream::InitFromFormat(const SPXWAVEFORMATEX& format, bool hasHeader)
{
    m_format = SpxCopyWAVEFORMATEX(format);
    m_hasHeader = hasHeader;
}

void CSpxAudioDataStream::UpdateStatusFromReason(ResultReason reason)
{
    auto status = StreamStatus::Unknown;
    switch(reason)
    {
    case ResultReason::SynthesizingAudioStarted:
        status = StreamStatus::NoData;
        break;

    case ResultReason::SynthesizingAudio:
        if (m_inventorySize > 0)
        {
            status = StreamStatus::PartialData;
        }
        else
        {
            status = StreamStatus::NoData;
        }

        break;

    case ResultReason::SynthesizingAudioCompleted:
        if (m_beginningReason == ResultReason::SynthesizingAudio)
        {
            status = StreamStatus::PartialData;
        }
        else
        {
            status = StreamStatus::AllData;
        }

        break;

    case ResultReason::Canceled:
        status = StreamStatus::Canceled;
        break;

    default:
        break;
    }
    m_status = status;
}

StreamStatus CSpxAudioDataStream::GetStatus() const noexcept
{
    return m_status;
}

void CSpxAudioDataStream::SetStatus(StreamStatus status) noexcept
{
    m_status = status;
}

CancellationReason CSpxAudioDataStream::GetCancellationReason()
{
    return m_cancellationReason;
}

std::shared_ptr<ISpxErrorInformation> CSpxAudioDataStream::GetError()
{
    return m_error;
}

bool CSpxAudioDataStream::CanReadData(uint32_t requestedSize)
{
    return requestedSize <= m_inventorySize - m_position;
}

bool CSpxAudioDataStream::CanReadData(uint32_t requestedSize, uint32_t pos)
{
    return pos <= m_inventorySize && requestedSize <= m_inventorySize - pos;
}

void CSpxAudioDataStream::SaveToWaveFile(const wchar_t* fileName)
{
    // Re-use the CSpxWavFileWriter class to write the audio to file
    auto waveFileWriter = SpxCreateObjectWithSite<ISpxAudioFile>("CSpxWavFileWriter", SpxGetRootSite());
    waveFileWriter->Open(fileName);

    auto formatInit = SpxQueryInterface<ISpxAudioStreamInitFormat>(waveFileWriter);
    formatInit->SetFormat(m_format.get());

    auto outputFormatInit = SpxQueryInterface<ISpxAudioOutputInitFormat>(waveFileWriter);
    outputFormatInit->SetHeader(m_hasHeader);

    // Re-use Read method to collect the audio data from audio list to a buffer
    auto buffer = SpxAllocSharedAudioBuffer(m_inventorySize - m_position);
    auto dataSize = Read(buffer.get(), m_inventorySize - m_position);

    auto audioOutput = SpxQueryInterface<ISpxAudioOutput>(waveFileWriter);
    audioOutput->Write(buffer.get(), dataSize);
    audioOutput->Close();
}

uint32_t CSpxAudioDataStream::GetPosition()
{
    return m_position;
}

void CSpxAudioDataStream::SetPosition(uint32_t pos)
{
    SPX_IFTRUE_THROW_HR(pos > m_inventorySize, SPXERR_INVALID_ARG);

    m_position = pos;
}

uint32_t CSpxAudioDataStream::Read(uint8_t* buffer, uint32_t bufferSize)
{
    SPX_DBG_TRACE_VERBOSE("CSpxAudioDataStream::Read: is called");
    SPX_IFTRUE_THROW_HR(buffer == nullptr, SPXERR_INVALID_ARG);

    // Wait until either enough data is collected, or writing is finished, otherwise it's unexpected
    if (!WaitForMoreData(m_position + bufferSize) && !m_writingEnded)
    {
        SPX_THROW_HR(SPXERR_UNEXPECTED_AUDIO_OUTPUT_FAILURE);
    }

    return FillBuffer(buffer, bufferSize, m_position);
}

uint32_t CSpxAudioDataStream::Read(uint8_t* buffer, uint32_t bufferSize, uint32_t pos)
{
    SPX_DBG_TRACE_VERBOSE("CSpxAudioDataStream::Read: is called");
    SPX_IFTRUE_THROW_HR(buffer == nullptr, SPXERR_INVALID_ARG);
    SPX_IFTRUE_THROW_HR(pos > m_inventorySize, SPXERR_INVALID_ARG);

    // Wait until either enough data is collected, or writing is finished, otherwise it's unexpected
    if (!WaitForMoreData(pos + bufferSize) && !m_writingEnded)
    {
        SPX_THROW_HR(SPXERR_UNEXPECTED_AUDIO_OUTPUT_FAILURE);
    }

    return FillBuffer(buffer, bufferSize, pos);
}

uint32_t CSpxAudioDataStream::Write(uint8_t* buffer, uint32_t size)
{
    SPX_DBG_TRACE_VERBOSE("CSpxAudioDataStream::Write buffer %p size=%d", (void*)buffer, size);

    if (size == 0)
    {
        return 0;
    }

    SPX_IFTRUE_THROW_HR(buffer == nullptr, SPXERR_INVALID_ARG);

    // Allocate the buffer for the audio, and make a copy of the data
    auto newBuffer = SpxAllocSharedAudioBuffer(size);
    memcpy(newBuffer.get(), buffer, size);

    // Store the buffer into our audio list
    std::unique_lock<std::mutex> lock(m_mutex);
    m_writingEnded = false;
    m_audioList.emplace_back(newBuffer, size);
    m_inventorySize += size;
    m_cv.notify_all();

    return size;
}

void CSpxAudioDataStream::Close()
{
    SignalEndOfWriting();
}

uint32_t CSpxAudioDataStream::FillBuffer(uint8_t* buffer, uint32_t bufferSize, uint32_t pos)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    // Set position
    m_position = pos;

    // Calculate the count of bytes to be read
    uint32_t totalBytesToBeRead = std::min(m_inventorySize - m_position, bufferSize);
    uint32_t remainedBytesToBeRead = totalBytesToBeRead;
    uint32_t offsetInBuffer = 0;

    // Seek to m_position
    uint32_t scannedSize = 0;
    auto iterator = m_audioList.begin();
    while (iterator != m_audioList.end() && scannedSize + iterator->second <= m_position)
    {
        scannedSize += iterator->second;
        ++iterator;
    }

    uint32_t positionInItem = m_position - scannedSize;

    // Read data from current item
    if (remainedBytesToBeRead > 0)
    {
        SPX_DBG_ASSERT_WITH_MESSAGE(iterator != m_audioList.end(), "m_position is out of m_audioList, which is unexpected.");
        uint32_t bytesToBeRead = std::min(iterator->second - positionInItem, remainedBytesToBeRead);
        memcpy(buffer, iterator->first.get() + positionInItem, bytesToBeRead);
        remainedBytesToBeRead -= bytesToBeRead;
        offsetInBuffer += bytesToBeRead;
        m_position += bytesToBeRead;
    }

    if (remainedBytesToBeRead > 0)
    {
        SPX_DBG_ASSERT_WITH_MESSAGE(iterator != m_audioList.end(), "m_position is out of m_audioList, which is unexpected.");
        ++iterator;
    }

    // Read data from more items until remainedBytesToBeRead == 0
    while (iterator != m_audioList.end() && remainedBytesToBeRead > 0)
    {
        uint32_t bytesToBeRead = std::min(iterator->second, remainedBytesToBeRead);
        memcpy(buffer + offsetInBuffer, iterator->first.get(), bytesToBeRead);
        remainedBytesToBeRead -= bytesToBeRead;
        offsetInBuffer += bytesToBeRead;
        m_position += bytesToBeRead;

        ++iterator;
    }

    SPX_DBG_TRACE_VERBOSE("CSpxAudioDataStream::Read: bytesRead=%d", totalBytesToBeRead);
    return totalBytesToBeRead;
}

void CSpxAudioDataStream::DisconnectSynthEvents()
{
    std::unique_lock<std::mutex> lock(m_eventMutex);

    if (auto synthEvents = m_synthEvents.lock())
    {
        // Disconnect synthesizing event
        synthEvents->DisconnectSynthesizingCallback(static_cast<void *>(this), nullptr);

        // Disconnect synthesis completed event
        synthEvents->DisconnectSynthesisCompletedCallback(static_cast<void *>(this), nullptr);

        // Disconnect synthesis canceled event
        synthEvents->DisconnectSynthesisCanceledCallback(static_cast<void *>(this), nullptr);
    }
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

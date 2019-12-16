//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// single_to_many_stream_reader_adapter.cpp: Implementation definitions for CSpxSingleToManyStreamReaderAdapter C++ class
//

// CSpxSingleToManyStreamReaderAdapter creates a layer on top of a ISpxAudioStreamReader that exists only as a singleton like in the case of a
// PMA audio source CPmaAudioReader instantiated by CSpxInteractiveMicrophone. This will allow for more than one reader to read from the
// stream synchrnously which may be a requirement (two recognizers in parallel) or a robustness feature (a recognizer may take longer to
// shutdown which will cause a crash)
//
//  
//    
//    
//                                 -------------------------------+                                                          
//                                 | CSpxSingleToManyStreamReader |                                                          
//                                 |                              |                                                          
//                                 +------------------------------||                                                         
//                                   ------------------------------+|                                                        
//                                    ------------------------------+                                                        
//                                                      ^                                                                    
//    ---------------------------------------------------\-------------------------------------------+                       
//    |  +------------------------------------+           |                                          |                       
//    |  | CSpxSingleToManyStreamReaderAdapter            \                                          |                       
//    |  +------------------------------------+            \                                         |                       
//    |                                         +-----------|------------------------------|         |                       
//    |                                         -CSpxAudioProcessorWriteToAudioSourceBuffer|         |                       
//    |                                         -------------------------------------------+         |                       
//    |                                                             ^                                |                       
//    |                                                             |                                |                       
//    |                                         +-------------------|-----------------------         |                       
//    |                                         -              CSpxAudioPump               |         |                       
//    |                                        --------------------------------------------+         |                       
//    |                                                             ^                                |                       
//    |                                                             |                                |                       
//    +-------------------------------------------------------------|--------------------------------|                       
//                                                                  |                                                        
//                                                                  |                                                        
//                                                                  |                                                        
//                                                                  |                                                        
//                                                      +-----------|----------|                                             
//                                                      |ISpxAudioStreamReader |                                             
//                                                      -----------------------+                                             
//    
#include "stdafx.h"
#include "create_object_helpers.h"
#include "single_to_many_stream_reader_adapter.h"
#include "single_to_many_stream_reader.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxSingleToManyStreamReaderAdapter::CSpxSingleToManyStreamReaderAdapter()
{
    SPX_DBG_TRACE_VERBOSE("CSpxSingleToManyStreamReaderAdapter::CSpxSingleToManyStreamReaderAdapter");
    m_clientCount = 0;
}

CSpxSingleToManyStreamReaderAdapter::~CSpxSingleToManyStreamReaderAdapter()
{
    SPX_DBG_TRACE_VERBOSE("CSpxSingleToManyStreamReaderAdapter::~CSpxSingleToManyStreamReaderAdapter");
    SPX_DBG_ASSERT(m_clientCount == 0);
    SPX_DBG_ASSERT(m_readersMap.size() == 0);

    Term();
}

void CSpxSingleToManyStreamReaderAdapter::Term()
{
    ClosePumpAndStream();
    m_sourceSingletonStreamReader.reset();
}

void CSpxSingleToManyStreamReaderAdapter::SetSingletonReader(std::shared_ptr<ISpxAudioStreamReader> singletonReader)
{
    m_sourceSingletonStreamReader = singletonReader;
    InitializeServices();

    // BUGBUG: When an object is created with site, its Init is called when SetSite() is called. The model is not
    // very obvious we should probably change it.
    m_sourceStreamReaderInitNeeded = false;
    m_audioStarted = false;

    SPX_TRACE_INFO("CSpxSingleToManyStreamReaderAdapter::SetSingletonReader: %p", (void*)m_sourceSingletonStreamReader.get());
}

void CSpxSingleToManyStreamReaderAdapter::InitializeServices()
{
    SPX_DBG_TRACE_VERBOSE("CSpxSingleToManyStreamReaderAdapter::InitializeServices");

    // We can set "AudioSourceBufferDataSizeInBytes" and "AudioSourceBufferAllowOverflow" for the ring buffer
    SetStringValue("AudioSourceBufferAllowOverflow", "true");
    
    // Create an audio pump, and set the reader
    auto pumpInit = SpxCreateObjectWithSite<ISpxAudioPumpInit>("CSpxAudioPump", this);
    pumpInit->SetReader(m_sourceSingletonStreamReader);

    m_singletonAudioPump = SpxQueryInterface<ISpxAudioPump>(pumpInit);

    // Get the audio format. We pass this information to the individual readers.
    auto requiredFormatSize = m_sourceSingletonStreamReader->GetFormat(nullptr, 0);
    m_sourceFormat = SpxAllocWAVEFORMATEX(requiredFormatSize);
    m_sourceSingletonStreamReader->GetFormat(m_sourceFormat.get(), requiredFormatSize);

    // This object retrieves the buffer and properties through the site. 
    m_audioProcessorBufferWriter = SpxCreateObjectWithSite<ISpxAudioProcessor>("CSpxAudioProcessorWriteToAudioSourceBuffer", this);
}

void CSpxSingleToManyStreamReaderAdapter::InitAudioProcessing()
{
    // Various query service try to get the buffer info, we will make sure we have it set early. We create a new buffer every time we start anew.
    InitAudioSourceBuffer();
    InitBufferProperties();
}

void CSpxSingleToManyStreamReaderAdapter::ResetAudioProcessing()
{
    m_bufferData.reset();
    m_bufferProperties.reset();
}


void CSpxSingleToManyStreamReaderAdapter::EnsureAudioStreamStarted()
{
    SPX_DBG_TRACE_VERBOSE("CSpxSingleToManyStreamReaderAdapter::EnsureInitSourceStreamReader");

    // TODO: Can't call init twice for USB android reader
    //if (m_sourceStreamReaderInitNeeded)
    //{
    //    auto initializer = SpxQueryInterface<ISpxObjectInit>(m_sourceSingletonStreamReader);

    //    // initializer->Init();
    //    m_sourceStreamReaderInitNeeded = false;
    //}

    if (!m_audioStarted)
    {
        InitAudioProcessing();
        m_singletonAudioPump->StartPump(m_audioProcessorBufferWriter);
        m_audioStarted = true;

        SPX_DBG_TRACE_VERBOSE("CSpxSingleToManyStreamReaderAdapter::EnsureInitSourceStreamReader: PumpStarted on singleton reader %p", (void*)m_sourceSingletonStreamReader.get());
    }
}

void CSpxSingleToManyStreamReaderAdapter::ClosePumpAndStream()
{
    if (m_audioStarted)
    {
        m_singletonAudioPump->StopPump();

        SPX_TRACE_INFO("CSpxSingleToManyStreamReaderAdapter::ClosePumpAndStream: Closing the singleton: %p", (void*)m_sourceSingletonStreamReader.get());
        m_sourceSingletonStreamReader->Close();
        ResetAudioProcessing();

        // TODO: Not used
        m_sourceStreamReaderInitNeeded = true;
        m_audioStarted = false;
    }
}

void CSpxSingleToManyStreamReaderAdapter::ReconnectClient(long clientId, std::shared_ptr<ISpxAudioStreamReader>&& reader)
{
    // Since the object is not going away at 0 we can't use InterlockedIncrement/Decrement here 
    std::lock_guard<std::mutex> lock(m_clientLifetimeLock);
    SPX_DBG_TRACE_INFO("CSpxSingleToManyStreamReaderAdapter::ReconnectClient: %d (client id: %ld)", m_clientCount, clientId);
    m_readersMap.emplace(std::make_pair(clientId, reader));
    EnsureAudioStreamStarted();
    m_clientCount++;
    SPX_DBG_ASSERT((size_t)m_clientCount == m_readersMap.size());
}

void CSpxSingleToManyStreamReaderAdapter::DisconnectClient(long clientId)
{
    std::lock_guard<std::mutex> lock(m_clientLifetimeLock);
    m_readersMap.erase(clientId);

    if (m_clientCount > 0)
    {
        m_clientCount--;
        SPX_DBG_TRACE_INFO("CSpxSingleToManyStreamReaderAdapter::DisconnectClient[%ld]: %d", clientId, m_clientCount);
        SPX_DBG_ASSERT((size_t)m_clientCount == m_readersMap.size());

        if (m_clientCount == 0)
        {
            ClosePumpAndStream();
        }
    }
    else
    {
        SPX_TRACE_ERROR("CSpxSingleToManyStreamReaderAdapter::DisconnectClient[%ld]: 0 clients", clientId);
    }
}

std::shared_ptr<ISpxAudioStreamReader> CSpxSingleToManyStreamReaderAdapter::CreateReader()
{
    SPX_DBG_TRACE_FUNCTION();

    auto clientId = m_nextClientId++;

    std::shared_ptr<CSpxSingleToManyStreamReader> demuxAudioReader = std::make_shared<CSpxSingleToManyStreamReader>(clientId, m_sourceFormat);
    auto sharedSitePtr = SpxSharedPtrFromThis<ISpxGenericSite>(this);

    // lock scope
    {
        // Initialize the audio reading before creating and returning the object. 
        std::lock_guard<std::mutex> lock(m_clientLifetimeLock);
        EnsureAudioStreamStarted();
    }

    // This calls Init as well which in turn will call ReconnectClient which will assure the pump initialization if needed.
    // No locks needed in this area
    demuxAudioReader->SetSite(sharedSitePtr);

    auto newReader = SpxQueryInterface<ISpxAudioStreamReader>(demuxAudioReader);

    SPX_DBG_TRACE_INFO("CSpxAudioDataStream::CreateReader: %d (client id: %ld)", m_clientCount, clientId);
    return newReader;
}

// ISpxAudioPumpSite::Error
void CSpxSingleToManyStreamReaderAdapter::Error(const std::string& error)
{
    // TODO: Direct this to clients
    if (!error.empty())
    {
        SPX_TRACE_ERROR("CSpxSingleToManyStreamReaderAdapter::Error: '%s'", error.c_str());

        // We may need to get this on a separate thread if locks overlap.
        HandleDownstreamError(error);
    }
}

std::shared_ptr<ISpxInterfaceBase> CSpxSingleToManyStreamReaderAdapter::QueryServiceAudioSourceBuffer(const char* serviceName)
{
    if (PAL::stricmp(serviceName, "AudioSourceBufferData") == 0)
    {
        return GetAudioSourceBuffer();
    }
    else if (PAL::stricmp(serviceName, "AudioSourceBufferProperties") == 0)
    {
        return GetBufferProperties();
    }
    return nullptr;
}

std::shared_ptr<ISpxAudioSourceBufferData> CSpxSingleToManyStreamReaderAdapter::InitAudioSourceBuffer()
{
    SPX_DBG_ASSERT(m_bufferData == nullptr);
    return m_bufferData = SpxCreateObjectWithSite<ISpxAudioSourceBufferData>("CSpxAudioSourceBufferData", this);
}

std::shared_ptr<ISpxAudioSourceBufferData> CSpxSingleToManyStreamReaderAdapter::GetAudioSourceBuffer()
{
    SPX_DBG_ASSERT(m_bufferData != nullptr);
    return m_bufferData;
}

std::shared_ptr<ISpxAudioSourceBufferProperties> CSpxSingleToManyStreamReaderAdapter::InitBufferProperties()
{
    SPX_DBG_ASSERT(m_bufferProperties == nullptr);
    auto site = SpxQueryInterface<ISpxGenericSite>(GetAudioSourceBuffer());
    return m_bufferProperties = SpxCreateObjectWithSite<ISpxAudioSourceBufferProperties>("CSpxAudioSourceBufferProperties", site);
}

std::shared_ptr<ISpxAudioSourceBufferProperties> CSpxSingleToManyStreamReaderAdapter::GetBufferProperties()
{
    SPX_DBG_ASSERT(m_bufferProperties != nullptr);
    return m_bufferProperties;
}

void CSpxSingleToManyStreamReaderAdapter::HandleDownstreamError(const std::string& error)
{
    std::lock_guard<std::mutex> lock(m_clientLifetimeLock);

    // Provide the error to all the connected clients
    for (auto crtReader : m_readersMap)
    {
        auto setErrorInfo = SpxQueryInterface<ISpxSetErrorInfo>(crtReader.second);
        setErrorInfo->SetError(error);
    }

    // Inform the audio processor of the error it will unblock readers if any. 
    auto processorErrorInfo = SpxQueryInterface<ISpxSetErrorInfo>(m_audioProcessorBufferWriter);
    processorErrorInfo->SetError(error);

    // We close the audio even if we have clients connected. 
    ClosePumpAndStream();
}

void CSpxSingleToManyStreamReaderAdapter::TermAudioSourceBuffer()
{
    SpxTermAndClear(m_bufferData);
    SPX_DBG_ASSERT(m_bufferData == nullptr);
}

}}}} // Microsoft.CognitiveServices.Speech.Impl

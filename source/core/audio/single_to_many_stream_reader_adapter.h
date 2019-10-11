//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// single_to_many_stream_reader_adapter.h: Declarations for CSpxSingleToManyStreamReaderAdapter C++ class
//

#pragma once
#include "null_audio_output.h"
#include "interface_helpers.h"
#include "service_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxSingleToManyStreamReaderAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxAudioStreamReaderFactory,
    public ISpxSingleToManyStreamReaderAdapter,
    public ISpxSingleToManyStreamReaderAdapterSite,
    public ISpxAudioPumpSite,
    public ISpxGenericSite,
    public ISpxServiceProvider
{
    public:
        CSpxSingleToManyStreamReaderAdapter();
        virtual ~CSpxSingleToManyStreamReaderAdapter();

        SPX_INTERFACE_MAP_BEGIN()
            SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamReaderFactory)
            SPX_INTERFACE_MAP_ENTRY(ISpxSingleToManyStreamReaderAdapter)
            SPX_INTERFACE_MAP_ENTRY(ISpxSingleToManyStreamReaderAdapterSite)
            SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
            SPX_INTERFACE_MAP_ENTRY(ISpxAudioPumpSite)
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
            SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_END()

        // --- IServiceProvider
        SPX_SERVICE_MAP_BEGIN()
        // SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
        SPX_SERVICE_MAP_ENTRY_FUNC(QueryServiceAudioSourceBuffer)
        SPX_SERVICE_MAP_ENTRY_OBJECT(ISpxAudioSourceBufferData, GetAudioSourceBuffer())
        SPX_SERVICE_MAP_ENTRY_OBJECT(ISpxAudioSourceBufferDataWriter, GetAudioSourceBuffer())
        SPX_SERVICE_MAP_ENTRY_OBJECT(ISpxAudioSourceBufferProperties, GetBufferProperties())
        SPX_SERVICE_MAP_END()

        // ISpxSingleToManyStreamReaderAdapter
        virtual void SetSingletonReader(std::shared_ptr<ISpxAudioStreamReader> singletonReader) override;

        // ISpxSingleToManyStreamReaderAdapterSite
        virtual void ReconnectClient(long clientId) override;
        virtual void DisconnectClient(long clientId) override;

        // ISpxAudioStreamReaderFactory
        virtual std::shared_ptr<ISpxAudioStreamReader> CreateReader() override;

        // --- ISpxAudioPumpSite
        void Error(const std::string& error) override;

        // --- ISpxObjectInit
        void Term() override;

    private:
        int m_clientCount = 0;
        std::atomic_long m_nextClientId {0l};
        std::mutex m_clientLifetimeLock;

        // TODO: remove lock when adding the circular buffer, it should not be needed here anymore
        std::mutex m_dataBufferAccessLock;
        SpxWAVEFORMATEX_Type m_sourceFormat;
        std::shared_ptr<ISpxAudioStreamReader> m_sourceSingletonStreamReader;
        bool m_audioStarted{ false };
        bool m_sourceStreamReaderInitNeeded{ false };

        // Writes the audio from the pump into the buffer. Finds the buffer to write into by querying the site for AudioSourceBuffer.
        // At this time is of CSpxAudioProcessorWriteToAudioSourceBuffer type;
        std::shared_ptr<ISpxAudioProcessor> m_audioProcessorBufferWriter;

        // this pump gets audio form the singleton stream into the curicular buffer through the means of the audio processor
        std::shared_ptr<ISpxAudioPump> m_singletonAudioPump;

        // Buffer shared between the readers and in which the Pump adds data.
        std::shared_ptr<ISpxAudioSourceBufferData> m_bufferData;
        std::shared_ptr<ISpxAudioSourceBufferProperties> m_bufferProperties;

        void EnsureAudioStreamStarted();
        void ClosePumpAndStream();
        void InitializeServices();
        std::shared_ptr<ISpxInterfaceBase> QueryServiceAudioSourceBuffer(const char* serviceName);
        std::shared_ptr<ISpxAudioSourceBufferData> InitAudioSourceBuffer();
        std::shared_ptr<ISpxAudioSourceBufferData> GetAudioSourceBuffer();
        std::shared_ptr<ISpxAudioSourceBufferProperties> GetBufferProperties();
        std::shared_ptr<ISpxAudioSourceBufferProperties> InitBufferProperties();
        void TermAudioSourceBuffer();
};

}}}}

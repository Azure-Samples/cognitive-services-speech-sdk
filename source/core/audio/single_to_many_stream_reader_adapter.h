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
#include <object_with_site_init_impl.h>

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
    public ISpxServiceProvider,
    public ISpxPropertyBagImpl
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
            SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_END()

        // --- IServiceProvider
        SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
        SPX_SERVICE_MAP_ENTRY_FUNC(QueryServiceAudioSourceBuffer)
        SPX_SERVICE_MAP_ENTRY_OBJECT(ISpxBufferData, GetAudioSourceBuffer())
        SPX_SERVICE_MAP_ENTRY_OBJECT(ISpxBufferDataWriter, GetAudioSourceBuffer())
        SPX_SERVICE_MAP_ENTRY_OBJECT(ISpxBufferProperties, GetBufferProperties())
        SPX_SERVICE_MAP_END()

        // ISpxSingleToManyStreamReaderAdapter
        virtual void SetSingletonReader(std::shared_ptr<ISpxAudioStreamReader> singletonReader) override;

        // ISpxSingleToManyStreamReaderAdapterSite
        virtual void ReconnectClient(long clientId, std::shared_ptr<ISpxAudioStreamReader>&& reader) override;
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
        std::shared_ptr<ISpxBufferData> m_bufferData;
        std::shared_ptr<ISpxBufferProperties> m_bufferProperties;

        void EnsureAudioStreamStarted();
        void ClosePumpAndStream();
        void InitializeServices();
        void InitAudioProcessing();
        void ResetAudioProcessing();
        std::shared_ptr<ISpxInterfaceBase> QueryServiceAudioSourceBuffer(const char* serviceName);
        std::shared_ptr<ISpxBufferData> InitAudioSourceBuffer();
        std::shared_ptr<ISpxBufferData> GetAudioSourceBuffer();
        std::shared_ptr<ISpxBufferProperties> GetBufferProperties();
        std::shared_ptr<ISpxBufferProperties> InitBufferProperties();
        void TermAudioSourceBuffer();
        void HandleDownstreamError(const std::string& error);

        std::map<long, std::shared_ptr<ISpxAudioStreamReader>> m_readersMap;
};

}}}}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// single_to_many_stream_reader.h: Declarations for CSpxSingleToManyStreamReader C++ class
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

class CSpxSingleToManyStreamReader :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxAudioStream,
    public ISpxAudioStreamReader,
    public ISpxSetErrorInfo
{
    public:
        CSpxSingleToManyStreamReader(long id, SpxWAVEFORMATEX_Type sourceFormat);
        virtual ~CSpxSingleToManyStreamReader();

        SPX_INTERFACE_MAP_BEGIN()
            SPX_INTERFACE_MAP_ENTRY(ISpxAudioStream)
            SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamReader)
            SPX_INTERFACE_MAP_ENTRY(ISpxSetErrorInfo)
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
            SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_END()

        // ISpxAudioStreamReader
        virtual uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat) override;
        virtual uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer) override;
        virtual void Close() override;

        // ISpxObjectInit
        virtual void Init() override;
        virtual void Term() override;

        // ISpxSetErrorInfo
        virtual void SetError(const std::string& error) override;

    private:
        long m_id;
        SpxWAVEFORMATEX_Type m_sourceFormat;
        bool m_streamOpened;
        std::shared_ptr<ISpxBufferData> m_bufferData;
        uint64_t m_bufferOffset {0};
        std::string m_lastError;

        void InitBufferDataFromSite();
        void ResetBufferData();
        bool IsDownstreamError();
};

}}}}


//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// null_audio_output.h: Implementation declarations for CSpxNullAudioOutput C++ class
//

#pragma once
#include "spxcore_common.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "property_bag_impl.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxNullAudioOutput : public ISpxAudioOutput, public ISpxAudioStream, public ISpxAudioStreamInitFormat
{
public:

    CSpxNullAudioOutput();
    ~CSpxNullAudioOutput();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioOutput)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStream)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamInitFormat)
    SPX_INTERFACE_MAP_END()

    // --- ISpxAudioOutput ---

    uint32_t Write(uint8_t* buffer, uint32_t size) override;
    void Close() override;

    // --- ISpxAudioStream ---

    uint16_t GetFormat(SPXWAVEFORMATEX* pformat, uint16_t cbFormat) override;
    bool HasHeader() override;
    std::string GetFormatString() override;
    std::string GetRawFormatString() override;

    // --- ISpxAudioStreamInitFormat ---

    void SetFormat(SPXWAVEFORMATEX* pformat) override;
    void SetHeader(bool hasHeader) override;
    void SetFormatString(const std::string& formatString) override;
    void SetRawFormatString(const std::string& rawFormatString) override;


protected:

    SpxWAVEFORMATEX_Type m_format;
    bool m_hasHeader = false;
    std::string m_formatString;
    std::string m_rawFormatString;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

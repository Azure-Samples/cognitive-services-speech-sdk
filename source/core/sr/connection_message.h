//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// connection_message.h: Implementation declarations for CSpxConnectionMessage C++ class
//

#pragma once

#include <memory>
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxConnectionMessage :
    public ISpxConnectionMessage,
    public ISpxConnectionMessageInit,
    public ISpxPropertyBagImpl
{
public:

    CSpxConnectionMessage();
    virtual ~CSpxConnectionMessage();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionMessage)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionMessageInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
    SPX_INTERFACE_MAP_END()

    // --- ISpxConnectionMessage
    const std::string& GetHeaders() const override;
    const std::string& GetPath() const override;
    const uint8_t* GetBuffer() const override;
    uint32_t GetBufferSize() const override;
    bool IsBufferBinary() const override;

    // -- ISpxConnectionMessageInit
    void Init(const std::string& headers, const std::string& path, const uint8_t* buffer, uint32_t bufferSize, bool bufferIsBinary) override;


private:

    CSpxConnectionMessage(const CSpxConnectionMessage&) = delete;
    CSpxConnectionMessage(const CSpxConnectionMessage&&) = delete;

    CSpxConnectionMessage& operator=(const CSpxConnectionMessage&) = delete;

    void InitHeaderMap();

    std::string m_path;
    std::string m_headers;

    std::shared_ptr<uint8_t> m_buffer;
    uint32_t m_bufferSize;
    bool m_bufferIsBinary;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

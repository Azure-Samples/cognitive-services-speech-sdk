//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// connection.h: Implementation declarations for CSpxConnection C++ class
//

#pragma once

#include <memory>
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include "object_with_site_init_impl.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxConnection :
    public ISpxConnection,
    public ISpxConnectionInit,
    public ISpxMessageParamFromUser,
    public ISpxPropertyBagImpl,
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>
{
public:

    CSpxConnection();
    virtual ~CSpxConnection();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxConnection)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxMessageParamFromUser)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
    SPX_INTERFACE_MAP_END()

    // --- ISpxConnection
    void Open(bool forContinuousRecognition) override;
    void Close() override;
    std::shared_ptr<ISpxRecognizer> GetRecognizer() override;

    // -- ISpxConnectionInit
    void Init(std::weak_ptr<ISpxRecognizer> recognizer, std::weak_ptr<ISpxMessageParamFromUser> setter) override;

    // --- ISpxUspMessageParamFromUser
    void SetParameter(std::string&& path, std::string&& name, std::string&& value) override;
    CSpxAsyncOp<bool> SendNetworkMessage(std::string&& path, std::string&& payload) override;
    CSpxAsyncOp<bool> SendNetworkMessage(std::string&& path, std::vector<uint8_t>&& payload) override;

    // --- ISpxObjectInit
    void Init() override
    {
    }
private:

    CSpxConnection(const CSpxConnection&) = delete;
    CSpxConnection(const CSpxConnection&&) = delete;

    CSpxConnection& operator=(const CSpxConnection&) = delete;

    std::weak_ptr<ISpxRecognizer> m_recognizer;
    std::weak_ptr<ISpxMessageParamFromUser> m_setMessageParamFromUser;

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

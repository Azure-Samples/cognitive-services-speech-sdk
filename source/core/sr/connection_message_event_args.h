//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// connection_message.h: Implementation declarations for CSpxConnectionMessageEventArgs C++ class
//

#pragma once

#include <memory>
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxConnectionMessageEventArgs :
    public ISpxConnectionMessageEventArgs,
    public ISpxConnectionMessageEventArgsInit
{
public:

    CSpxConnectionMessageEventArgs();
    virtual ~CSpxConnectionMessageEventArgs();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionMessageEventArgs)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionMessageEventArgsInit)
    SPX_INTERFACE_MAP_END()

    // --- ISpxConnectionMessageEventArgs
    std::shared_ptr<ISpxConnectionMessage> GetMessage() const override;

    // -- ISpxConnectionMessageEventArgsInit
    void Init(std::shared_ptr<ISpxConnectionMessage> message) override;


private:

    CSpxConnectionMessageEventArgs(const CSpxConnectionMessageEventArgs&) = delete;
    CSpxConnectionMessageEventArgs(const CSpxConnectionMessageEventArgs&&) = delete;

    CSpxConnectionMessageEventArgs& operator=(const CSpxConnectionMessageEventArgs&) = delete;

    std::shared_ptr<ISpxConnectionMessage> m_message;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// synthesis_event_args.h: Implementation declarations for CSpxSynthesisEventArgs C++ class
//

#pragma once
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxSynthesisEventArgs :
    public ISpxSynthesisEventArgs, 
    public ISpxSynthesisEventArgsInit
{
public:

    CSpxSynthesisEventArgs();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxSynthesisEventArgs)
        SPX_INTERFACE_MAP_ENTRY(ISpxSynthesisEventArgsInit)
    SPX_INTERFACE_MAP_END()

    // --- ISpxSynthesisEventArgs
    virtual std::shared_ptr<ISpxSynthesisResult> GetResult() override;

    // --- ISpxSynthesisEventArgsInit
    virtual void Init(std::shared_ptr<ISpxSynthesisResult> result) override;


private:

    CSpxSynthesisEventArgs(const CSpxSynthesisEventArgs&) = delete;
    CSpxSynthesisEventArgs(const CSpxSynthesisEventArgs&&) = delete;

    CSpxSynthesisEventArgs& operator=(const CSpxSynthesisEventArgs&) = delete;

    std::shared_ptr<ISpxSynthesisResult> m_result;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

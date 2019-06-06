//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// unidec_config.h: Implementation declarations for CSpxUnidecConfig C++ class
//

#pragma once
#include <memory>
#include "spxcore_common.h"
#include "ispxinterfaces.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

const std::string g_unidecBaseModelPath = "BaseModelPath";
const std::string g_unidecHCLGSpec = "IN_HCLGSpec";
const std::string g_unidecHCLGSpecBase = "IN_HCLGSpecBase";
const std::string g_unidecEnableSegmentation = "EnableSegmentation";

class CSpxUnidecConfig
{
public:

    template<class Config>
    CSpxUnidecConfig(Config& config)
    {
        BaseModelPath = config[g_unidecBaseModelPath];
        HCLGSpec = config[g_unidecHCLGSpec];
        HCLGSpecBase = config[g_unidecHCLGSpecBase];
        EnableSegmentation = IsTrue(config[g_unidecEnableSegmentation]);
    }

    std::wstring BaseModelPath;
    std::wstring HCLGSpec;
    std::wstring HCLGSpecBase;
    bool EnableSegmentation;
    
    std::wstring GetHCLGSpecString() const
    {
        return HCLGSpec;
    }

private:

    bool IsTrue(const std::wstring& str)
    {
        return (str == L"true" || str == L"1");
    }
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

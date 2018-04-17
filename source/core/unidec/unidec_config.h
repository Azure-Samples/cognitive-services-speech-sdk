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
#include "unidecruntime.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxUnidecConfig
{
public:

    template<class Config>
    CSpxUnidecConfig(Config& config)
    {
        BaseModelPath = config["BaseModelPath"];
        FeatureName = config["FeatureName"];
        HCLGSpecBase = config["HCLGSpecBase"];
        BeamThreshold = config["BeamThreshold"];
        BeamSize = config["BeamSize"];
        NBestBeamSize = config["NBestBeamSize"];
        LocaleId = config["LocaleId"];
        BaseModelId = config["BaseModelId"];
        InDnnSpecPrefix = config["InDnnSpecPrefix"];
        InDnnSpecPostfix = config["InDnnSpecPostfix"];
        FrameCopyCount = config["FrameCopyCount"];
        FeFileName = config["FeFileName"];
        LmsFileName = config["LmsFileName"];
        HclgFileName = config["HclgFileName"];
        AmFileName = config["AmFileName"];
        EnableSegmentation = IsTrue(config["EnableSegmentation"]);
    }

    std::wstring BaseModelPath;
    std::wstring FeatureName;
    std::wstring HCLGSpecBase;
    std::wstring BeamThreshold;
    std::wstring BeamSize;
    std::wstring NBestBeamSize;
    std::wstring LocaleId;
    std::wstring BaseModelId;

    std::wstring InDnnSpecPrefix;
    std::wstring InDnnSpecPostfix;
    std::wstring FrameCopyCount;

    std::wstring FeFileName;
    std::wstring LmsFileName;
    std::wstring HclgFileName;
    std::wstring AmFileName;

    bool EnableSegmentation;
    
    std::wstring GetFronEndSpecString() const
    {
        std::wstring fronEndSpecStr = L"audio(";
        fronEndSpecStr += BaseModelPath;
        fronEndSpecStr += FeFileName;
        fronEndSpecStr += L",";
        fronEndSpecStr += FeatureName;
        fronEndSpecStr += L")";

        return fronEndSpecStr;
    }

    std::wstring GetDnnSpecString() const
    {
        std::wstring dnnSpecStr = InDnnSpecPrefix;
        dnnSpecStr += BaseModelPath;
        dnnSpecStr += AmFileName;
        dnnSpecStr += InDnnSpecPostfix;

        // add frameCopyCount
        dnnSpecStr += L",";
        dnnSpecStr += L"frameCopyCount(";
        dnnSpecStr += FrameCopyCount;
        dnnSpecStr += L")";

        return dnnSpecStr;
    }

    std::wstring GetHCLGSpecString() const
    {
        std::wstring hclgSpecStr = L"default(";
        hclgSpecStr += BaseModelPath;
        hclgSpecStr += HclgFileName;
        hclgSpecStr += L",";
        hclgSpecStr += BaseModelPath;
        hclgSpecStr += LmsFileName;
        hclgSpecStr += L")";

        return hclgSpecStr;
    }

    float GetBeamThresholdValue() const
    {
        auto BeamThresholdValue = (BeamThreshold.length() == 0) ? 99999.0f : std::stof(BeamThreshold);
        SPX_DBG_ASSERT(0 < BeamThresholdValue);

        return BeamThresholdValue;
    }

private:

    bool IsTrue(const std::wstring& str)
    {
        return _wcsicmp(str.c_str(), L"true") == 0 || _wcsicmp(str.c_str(), L"1") == 0;
    }
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

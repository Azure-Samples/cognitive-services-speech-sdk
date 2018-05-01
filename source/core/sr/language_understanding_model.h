//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include <string>
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxLanguageUnderstandingModel : public ISpxLanguageUnderstandingModel
{
public:

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxLanguageUnderstandingModel)
    SPX_INTERFACE_MAP_END()

    // --- ISpxLanguageUnderstandingModel ---
    void InitEndpoint(const wchar_t* uri) override;
    void InitSubscription(const wchar_t* subscription, const wchar_t* appId, const wchar_t* region) override;

    std::wstring GetEndpoint() const override { return m_endpoint; }
    std::wstring GetHostName() const override { return m_hostName; }
    std::wstring GetPathAndQuery() const override { return m_path; }

    std::wstring GetSubscriptionKey() const override { return m_subscription; }
    std::wstring GetAppId() const override { return m_appId; }
    std::wstring GetRegion() const override { return m_region; }

private: 

    void ParseEndpoint();
    void BuildEndpoint();

    std::wstring m_endpoint;
    std::wstring m_hostName;
    std::wstring m_path;

    std::wstring m_region;
    std::wstring m_subscription;
    std::wstring m_appId;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

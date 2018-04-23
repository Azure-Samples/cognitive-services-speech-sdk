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
    void InitSubscriptionInfo(const wchar_t* hostName, const wchar_t* subscriptionKey, const wchar_t* appId) override;

    std::wstring GetEndpoint() const override { return m_endpoint; }

    std::wstring GetHostName() const override { return m_hostName; }
    std::wstring GetSubscriptionKey() const override { return m_subscriptionKey; }
    std::wstring GetAppId() const override { return m_appId; }

private: 

    std::wstring m_endpoint;

    std::wstring m_hostName;
    std::wstring m_subscriptionKey;
    std::wstring m_appId;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

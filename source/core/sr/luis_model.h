#pragma once
#include <string>
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxLuisModel : public ISpxLuisModel
{
public:

    // --- ISpxLuisModel ---
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


} // CARBON_IMPL_NAMESPACE

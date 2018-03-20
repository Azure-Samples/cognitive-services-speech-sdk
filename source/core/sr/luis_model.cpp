#include "stdafx.h"
#include "luis_model.h"


namespace CARBON_IMPL_NAMESPACE() {


void CSpxLuisModel::InitEndpoint(const wchar_t* uri)
{
    SPX_IFTRUE_THROW_HR(!m_hostName.empty() || !m_appId.empty() || !m_subscriptionKey.empty(), SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!m_endpoint.empty(), SPXERR_ALREADY_INITIALIZED);
    m_endpoint = uri;
}

void CSpxLuisModel::InitSubscriptionInfo(const wchar_t* hostName, const wchar_t* subscriptionKey, const wchar_t* appId)
{
    SPX_IFTRUE_THROW_HR(!m_hostName.empty() || !m_appId.empty() || !m_subscriptionKey.empty(), SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!m_endpoint.empty(), SPXERR_ALREADY_INITIALIZED);
    m_hostName = hostName;
    m_subscriptionKey = subscriptionKey;
    m_appId = appId;
}


} // CARBON_IMPL_NAMESPACE

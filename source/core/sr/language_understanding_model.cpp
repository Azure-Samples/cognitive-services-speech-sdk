#include "stdafx.h"
#include "language_understanding_model.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


void CSpxLanguageUnderstandingModel::InitEndpoint(const wchar_t* uri)
{
    SPX_IFTRUE_THROW_HR(!m_hostName.empty() || !m_appId.empty() || !m_subscriptionKey.empty(), SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!m_endpoint.empty(), SPXERR_ALREADY_INITIALIZED);
    m_endpoint = uri;
}

void CSpxLanguageUnderstandingModel::InitSubscriptionInfo(const wchar_t* hostName, const wchar_t* subscriptionKey, const wchar_t* appId)
{
    SPX_IFTRUE_THROW_HR(!m_hostName.empty() || !m_appId.empty() || !m_subscriptionKey.empty(), SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!m_endpoint.empty(), SPXERR_ALREADY_INITIALIZED);
    m_hostName = hostName;
    m_subscriptionKey = subscriptionKey;
    m_appId = appId;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

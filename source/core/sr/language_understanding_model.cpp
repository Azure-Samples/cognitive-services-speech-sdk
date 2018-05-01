#include "stdafx.h"
#include "language_understanding_model.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


void CSpxLanguageUnderstandingModel::InitEndpoint(const wchar_t* uri)
{
    SPX_IFTRUE_THROW_HR(!m_region.empty() || !m_appId.empty() || !m_subscription.empty(), SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!m_endpoint.empty(), SPXERR_ALREADY_INITIALIZED);

    m_endpoint = uri;
    ParseEndpoint();
}

void CSpxLanguageUnderstandingModel::InitSubscription(const wchar_t* subscription, const wchar_t* appId, const wchar_t* region)
{
    SPX_IFTRUE_THROW_HR(!m_region.empty() || !m_appId.empty() || !m_subscription.empty(), SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(!m_endpoint.empty(), SPXERR_ALREADY_INITIALIZED);

    m_subscription = subscription;
    m_region = region;
    m_appId = appId;

    BuildEndpoint();
}

void CSpxLanguageUnderstandingModel::ParseEndpoint()
{
    // We're going to search the "endpoint" to find: region, appId, subscription key, hostname, and ... finally ... pathAndQuery
    //
    // NOTE: The endpoints that come to us should look like this: 
    // https://{region}.api.cognitive.microsoft.com/luis/v2.0/apps/{app-id}?subscription-key={subkey}&verbose=true&timezoneOffset=0&q=
    //
    // If they don't... we'll throw an SPXERR_INVALID_URL exception

    // skip past the protocol...
    auto charPtr = m_endpoint.c_str();
    while (*charPtr != '\0' && *charPtr != ':')
        charPtr++;

    // ensure there's a well formed "://" after that ... the host name starts immediately after that... as does the region
    SPX_IFTRUE_THROW_HR(*charPtr != ':' && charPtr[1] != '/' && charPtr[2] != '/', SPXERR_INVALID_URL);
    charPtr += 3;
    auto hostPtr = charPtr;
    auto regionPtr = charPtr;

    // keep going until we find the first '.' ... that'll be the end of the region ...
    while (*charPtr !='\0' && *charPtr != '.' && *charPtr != '/' )
        charPtr++;

    // ensure we found the period separating the region from the rest of the host name
    SPX_IFTRUE_THROW_HR(*charPtr != '.', SPXERR_INVALID_URL);
    auto regionEndPtr = charPtr;

    // keep going until we find the beginning of the relative path part...
    while (*charPtr != '\0' && *charPtr != '/')
        charPtr++;

    // ensure that we found the beginning of the relative path... that's also the end of the host name (with domain name suffix)
    SPX_IFTRUE_THROW_HR(*charPtr != '/', SPXERR_INVALID_URL);
    auto hostEndPtr = charPtr;
    auto pathPtr = charPtr;

    // now skip forward until we reach the end of the url, or, until we find the trailing "&q=" part... 
    while (*charPtr != '\0')
    {
        if (*charPtr == '&' && charPtr[1] == 'q' && charPtr[2] == '=')
        {
            // we found the "&q=" part ... it had better be at the very end of the url
            SPX_IFTRUE_THROW_HR(charPtr[3] != '\0', SPXERR_INVALID_URL);
            break;
        }
        charPtr++;
    }
    auto pathEndPtr = charPtr;

    // we should have most of what we need now ... 
    m_hostName = std::wstring(hostPtr, hostEndPtr - hostPtr);
    m_region = std::wstring(regionPtr, regionEndPtr - regionPtr);
    m_path = std::wstring(pathPtr, pathEndPtr - pathPtr);

    // but we still need to tease out the subscription and the app id from the path
    // which looks like this: "/luis/v2.0/apps/{app-id}?subscription-key={subkey}&verbose=true&timezoneOffset=0&q="

    // so ... let's first find the token immediately before the app-id
    auto pos = m_path.find(L"/apps/");
    if (pos != std::string::npos)
    {
        pos += wcslen(L"/apps/"); // increment over that token, and store the app id start ptr
        auto appIdPtr = pathPtr + pos;

        // now let's find the token immediately after the app id (the '?')
        pos = m_path.find(L'?', pos);
        if (pos != std::string::npos)
        {
            // store the app id end pointer
            auto appIdEndPtr = pathPtr + pos;

            // now let's get the subscription key
            pos = m_path.find(L"subscription-key=", pos);
            if (pos != std::string::npos)
            {
                pos += wcslen(L"subscription-key="); // increment over that token, and store the app id end pointer
                auto subscriptionPtr = pathPtr + pos;

                // and finally we'll find the end of that (by finding the token immediately after it, the next '&' (or end of string))
                pos = m_path.find(L'&', pos);
                if (pos == std::string::npos)
                    pos = m_path.length();

                // store the subscription end ptr
                auto subscriptionEndPtr = pathPtr + pos;

                // finally ... we have all we need... for the subscription and the app id
                m_subscription = std::wstring(subscriptionPtr, subscriptionEndPtr - subscriptionPtr);
                m_appId = std::wstring(appIdPtr, appIdEndPtr - appIdPtr);
            }
        }
    }

    BuildEndpoint(); // this will ensure we did everything properly and we'll also put the trailing "&q=" back on the end...
}

void CSpxLanguageUnderstandingModel::BuildEndpoint()
{
    constexpr auto defaultHostNameSuffix = LR"(.api.cognitive.microsoft.com)";

    m_hostName = m_region;
    if (m_region.find('.') == std::string::npos) {
        m_hostName += std::wstring(defaultHostNameSuffix);
    }

    m_path = LR"(/luis/v2.0/apps/)";
    m_path += m_appId;
    m_path += LR"(?subscription-key=)";
    m_path += m_subscription;
    m_path += LR"(&q=)";

    m_endpoint = L"https://";
    m_endpoint += m_hostName;
    m_endpoint += m_path;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

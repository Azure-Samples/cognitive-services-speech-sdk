//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_language_understanding_model.h: Public API declarations for LanguageUnderstandingModel C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Intent {

/// <summary>
/// Represents language understanding model used for intent recognition.
/// </summary>
class LanguageUnderstandingModel
{
public:

    /// <summary>
    /// Creates a language understanding model using the specified endpoint url.
    /// </summary>
    /// <param name="uri">The endpoint url of a language understanding model.</param>
    /// <returns>A shared pointer to language understanding model.</returns>
    static std::shared_ptr<LanguageUnderstandingModel> FromEndpoint(const std::wstring& uri)
    {
        SPXLUMODELHANDLE hlumodel = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LanguageUnderstandingModel_Create_From_Uri(uri.c_str(), &hlumodel));
        return std::make_shared<LanguageUnderstandingModel>(hlumodel);
    }

    /// <summary>
    /// Creates a language understanding model using the specified app id.
    /// </summary>
    /// <param name="appId">A string that represents the application id of Language Understanding service.</param>
    /// <returns>A shared pointer to language understanding model.</returns>
    static std::shared_ptr<LanguageUnderstandingModel> FromAppId(const std::wstring& appId)
    {
        SPXLUMODELHANDLE hlumodel = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LanguageUnderstandingModel_Create_From_AppId(appId.c_str(), &hlumodel));
        return std::make_shared<LanguageUnderstandingModel>(hlumodel);
    }

    /// <summary>
    /// Creates a language understanding model using the specified hostname, subscription key and application id.
    /// </summary>
    /// <param name="subscriptionKey">A string that represents the subscription key of Language Understanding service.</param>
    /// <param name="appId">A string that represents the application id of Language Understanding service.</param>
    /// <param name="region">A String that represents the region of the Language Understanding service.</param>
    /// <returns>A shared pointer to language understanding model.</returns>
    static std::shared_ptr<LanguageUnderstandingModel> FromSubscription(const std::wstring& subscriptionKey, const std::wstring& appId, const std::wstring& region)
    {
        SPXLUMODELHANDLE hlumodel = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LanguageUnderstandingModel_Create_From_Subscription(subscriptionKey.c_str(), appId.c_str(), region.c_str(), &hlumodel));
        return std::make_shared<LanguageUnderstandingModel>(hlumodel);
    }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit LanguageUnderstandingModel(SPXLUMODELHANDLE hlumodel = SPXHANDLE_INVALID) : m_hlumodel(hlumodel) { }
    
    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~LanguageUnderstandingModel() { LanguageUnderstandingModel_Handle_Close(m_hlumodel); }

    /// <summary>
    /// Internal. Explicit conversion operator.
    /// </summary>
    explicit operator SPXLUMODELHANDLE() { return m_hlumodel; }

private:
    DISABLE_COPY_AND_MOVE(LanguageUnderstandingModel);

    SPXLUMODELHANDLE m_hlumodel;
};


} } } } // Microsoft::CognitiveServices::Speech::Intent

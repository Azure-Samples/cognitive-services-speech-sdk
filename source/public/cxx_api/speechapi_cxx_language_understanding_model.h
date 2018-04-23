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
namespace Recognition {
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
    static std::shared_ptr<LanguageUnderstandingModel> From(const std::wstring& uri)
    {
        SPXLUMODELHANDLE hlumodel = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LanguageUnderstandingModel_Create_From_Uri(uri.c_str(), &hlumodel));
        return std::make_shared<LanguageUnderstandingModel>(hlumodel);
    }

    /// <summary>
    /// Creates a language understanding model using the specified subscription key and application id.
    /// </summary>
    /// <param name="subscriptionKey">Subscription key.</param>
    /// <param name="appId">Application id.</param>
    /// <returns>A shared pointer to language understanding model.</returns>
    static std::shared_ptr<LanguageUnderstandingModel> From(const std::wstring& subscriptionKey, const std::wstring& appId)
    {
        SPXLUMODELHANDLE hlumodel = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LanguageUnderstandingModel_Create_From_Subscription(nullptr, subscriptionKey.c_str(), appId.c_str(), &hlumodel));
        return std::make_shared<LanguageUnderstandingModel>(hlumodel);
    }

    /// <summary>
    /// Creates a language understanding model using the specified hostname, subscription key and application id.
    /// </summary>
    /// <param name="hostName">Hostname.</param>
    /// <param name="subscriptionKey">Subscription key.</param>
    /// <param name="appId">Application id.</param>
    /// <returns>A shared pointer to language understanding model.</returns>
    static std::shared_ptr<LanguageUnderstandingModel> From(const std::wstring& hostName, const std::wstring& subscriptionKey, const std::wstring& appId)
    {
        SPXLUMODELHANDLE hlumodel = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LanguageUnderstandingModel_Create_From_Subscription(hostName.c_str(), subscriptionKey.c_str(), appId.c_str(), &hlumodel));
        return std::make_shared<LanguageUnderstandingModel>(hlumodel);
    }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    LanguageUnderstandingModel(SPXLUMODELHANDLE hlumodel = SPXHANDLE_INVALID) : m_hlumodel(hlumodel) { }
    
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


} } } } } // Microsoft::CognitiveServices::Speech::Recognition::Intent

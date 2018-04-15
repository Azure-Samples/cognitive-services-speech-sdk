//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_luis_model.h: Public API declarations for LuisModel C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Intent {

/// <summary>
/// Represents LUIS model used for intent recognition.
/// </summary>
class LuisModel
{
public:

    /// <summary>
    /// Creates a LUIS model using the specified endpoint url.
    /// </summary>
    /// <param name="uri">The endpoint url of a LUIS model.</param>
    /// <returns>A shared pointer to LUIS model.</returns>
    static std::shared_ptr<LuisModel> From(const wchar_t* uri)
    {
        SPXLUISHANDLE hluis = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LuisModel_Create_From_Uri(uri, &hluis));
        return std::make_shared<LuisModel>(hluis);
    }

    /// <summary>
    /// Creates a LUIS model using the specified subscription key and application id.
    /// </summary>
    /// <param name="subscriptionKey">Subscription key.</param>
    /// <param name="appId">Application id.</param>
    /// <returns>A shared pointer to LUIS model.</returns>
    static std::shared_ptr<LuisModel> From(const wchar_t* subscriptionKey, const wchar_t* appId)
    {
        SPXLUISHANDLE hluis = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LuisModel_Create_From_Subscription(nullptr, subscriptionKey, appId, &hluis));
        return std::make_shared<LuisModel>(hluis);
    }

    /// <summary>
    /// Creates a LUIS model using the specified hostname, subscription key and application id.
    /// </summary>
    /// <param name="hostName">Hostname.</param>
    /// <param name="subscriptionKey">Subscription key.</param>
    /// <param name="appId">Application id.</param>
    /// <returns>A shared pointer to LUIS model.</returns>
    static std::shared_ptr<LuisModel> From(const wchar_t* hostName, const wchar_t* subscriptionKey, const wchar_t* appId)
    {
        SPXLUISHANDLE hluis = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LuisModel_Create_From_Subscription(hostName, subscriptionKey, appId, &hluis));
        return std::make_shared<LuisModel>(hluis);
    }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    LuisModel(SPXLUISHANDLE hluis = SPXHANDLE_INVALID) : m_hluis(hluis) { }
    
    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~LuisModel() { LuisModel_Handle_Close(m_hluis); }

    /// <summary>
    /// Internal. Explicit conversion operator.
    /// </summary>
    explicit operator SPXLUISHANDLE() { return m_hluis; }

private:
    DISABLE_COPY_AND_MOVE(LuisModel);
    // DISABLE_DEFAULT_CTORS(LuisModel);

    SPXLUISHANDLE m_hluis;
};


} } } // CARBON_NAMESPACE_ROOT :: Recognition :: Intent

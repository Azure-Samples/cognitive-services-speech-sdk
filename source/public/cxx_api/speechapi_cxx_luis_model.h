//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_luis_model.h: Public API declarations for LuisModel C++ class
//

#pragma once
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Intent {


class LuisModel
{
public:

    static std::shared_ptr<LuisModel> From(const wchar_t* uri)
    {
        SPXLUISHANDLE hluis = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LuisModel_Create_From_Uri(uri, &hluis));
        return std::make_shared<LuisModel>(hluis);
    }

    static std::shared_ptr<LuisModel> From(const wchar_t* subscriptionKey, const wchar_t* appId)
    {
        SPXLUISHANDLE hluis = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LuisModel_Create_From_Subscription(nullptr, subscriptionKey, appId, &hluis));
        return std::make_shared<LuisModel>(hluis);
    }

    static std::shared_ptr<LuisModel> From(const wchar_t* hostName, const wchar_t* subscriptionKey, const wchar_t* appId)
    {
        SPXLUISHANDLE hluis = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(LuisModel_Create_From_Subscription(hostName, subscriptionKey, appId, &hluis));
        return std::make_shared<LuisModel>(hluis);
    }

    LuisModel(SPXLUISHANDLE hluis = SPXHANDLE_INVALID) : m_hluis(hluis) { }
    virtual ~LuisModel() { LuisModel_Handle_Close(m_hluis); }

    explicit operator SPXLUISHANDLE() { return m_hluis; }
    

private:

    LuisModel(LuisModel&&) = delete;
    LuisModel(const LuisModel&) = delete;
    LuisModel& operator=(LuisModel&&) = delete;
    LuisModel& operator=(const LuisModel&) = delete;

    SPXLUISHANDLE m_hluis;
};


} } } // CARBON_NAMESPACE_ROOT :: Recognition :: Intent

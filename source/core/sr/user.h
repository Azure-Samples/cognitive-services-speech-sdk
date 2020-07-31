//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// user.h: Private implementation declarations for user
//
#pragma once

#include "ispxinterfaces.h"
#include "service_helpers.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxUser :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxGenericSite,
    public ISpxPropertyBagImpl,
    public ISpxUser
{
public:

    CSpxUser();
    virtual ~CSpxUser();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxUser)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectWithSiteInit


    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxUser
    void InitFromUserId(const char* pszUserId) override;
    std::string GetId() const override;

protected:
    std::string m_userId;

private:
    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;

    DISABLE_COPY_AND_MOVE(CSpxUser);

};


} } } } // Microsoft::CognitiveServices::Speech::Impl

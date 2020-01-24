//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// extension_resource_manager.h: Implementation declarations for CSpxExtensionResourceManager C++ class

#pragma once
#include "property_bag_impl.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "service_provider_impl.h"
#include "shared_ptr_helpers.h"
#include "singleton.h"
#include "create_object_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxExtensionResourceManager :
    private CSpxSingleton<CSpxExtensionResourceManager, ISpxObjectFactory>,
    public ISpxServiceProviderImpl,
    public ISpxObjectFactory,
    public ISpxPropertyBagImpl,
    public ISpxGenericSite
{
public:

    ~CSpxExtensionResourceManager();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxInterfaceBase)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectFactory)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
    SPX_INTERFACE_MAP_END()

    // NOTE:  This GetObjectFactory() method SHOULD NOT be used by functions/methods other than the
    //        SpxCreateObjectWithSite() method. If you are an object running in the "Carbon" system
    //        and you need to create an object, either use SpxCreateObjectWithSite() with your site
    //        or call your site-specific creation/initialization method, or obtain the object
    //        factory yourself (via SpxQueryService()), and call CreateObject() yourself.
    //
    static std::shared_ptr<ISpxObjectFactory> GetObjectFactory()
    {
        return GetInterface();
    }

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxObjectFactory)
        SPX_SERVICE_MAP_ENTRY(ISpxAddServiceProvider)
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
    SPX_SERVICE_MAP_END()

protected:

    // --- ISpxObjectFactory
    void* CreateObject(const char* className, const char* interfaceName) override;

private:

    friend class CSpxSingleton<CSpxExtensionResourceManager, ISpxObjectFactory>;

    CSpxExtensionResourceManager();
    CSpxExtensionResourceManager(const CSpxExtensionResourceManager&) = delete;
    CSpxExtensionResourceManager(const CSpxExtensionResourceManager&&) = delete;

    CSpxExtensionResourceManager& operator =(const CSpxExtensionResourceManager&) = delete;
    CSpxExtensionResourceManager&& operator =(const CSpxExtensionResourceManager&&) = delete;

    std::list<std::shared_ptr<ISpxObjectFactory>> m_moduleFactories;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// module_factory.h: Implementation declarations for CSpxModuleFactory C++ class
//

#pragma once
#include "spxcore_common.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxModuleFactory : public ISpxObjectFactory
{
public:

    typedef void* (*PCREATE_MODULE_OBJECT_FUNC)(const char* className, const char* interfaceName);
    static std::mutex m_mutex;
    static std::shared_ptr<ISpxObjectFactory> Get(const std::string& filename);
    static std::shared_ptr<ISpxObjectFactory> Get(PCREATE_MODULE_OBJECT_FUNC filename);

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectFactory)
    SPX_INTERFACE_MAP_END()


private:

    CSpxModuleFactory(const std::string& filename);
    CSpxModuleFactory(PCREATE_MODULE_OBJECT_FUNC pFunc);

    CSpxModuleFactory() = delete;
    CSpxModuleFactory(const CSpxModuleFactory&) = delete;
    CSpxModuleFactory(const CSpxModuleFactory&&) = delete;

    CSpxModuleFactory& operator =(const CSpxModuleFactory&) = delete;
    CSpxModuleFactory&& operator =(const CSpxModuleFactory&&) = delete;

    PCREATE_MODULE_OBJECT_FUNC m_pfnCreateModuleObject;

    typedef std::map<std::string, std::weak_ptr<ISpxObjectFactory>> MapType;
    static std::unique_ptr<MapType> m_factoryMap;

protected:

    void* CreateObject(const char* className, const char* interfaceName) override;
    PCREATE_MODULE_OBJECT_FUNC GetCreateModuleObjectFunctionPointer(const std::string& filename);
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

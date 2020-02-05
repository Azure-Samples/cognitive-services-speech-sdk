//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// module_factory.cpp: Implementation definitions for CSpxModuleFactory C++ class
//

#include "stdafx.h"

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "dynamic_module.h"
#include "module_factory.h"
SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName);

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


std::unique_ptr<CSpxModuleFactory::MapType> CSpxModuleFactory::m_factoryMap = std::make_unique<CSpxModuleFactory::MapType>();
std::mutex CSpxModuleFactory::m_mutex;

std::shared_ptr<ISpxObjectFactory> CSpxModuleFactory::Get(PCREATE_MODULE_OBJECT_FUNC pFunc)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    auto item = m_factoryMap->find("carbon");
    if (item != m_factoryMap->end())
    {
        return item->second.lock();
    }

    std::shared_ptr<ISpxObjectFactory> factory(new CSpxModuleFactory(pFunc));
    m_factoryMap->emplace("carbon", factory);

    return factory;
}

std::shared_ptr<ISpxObjectFactory> CSpxModuleFactory::Get(const std::string& filename)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    auto item = m_factoryMap->find(filename);
    if (item != m_factoryMap->end())
    {
        return item->second.lock();
    }

    std::shared_ptr<ISpxObjectFactory> factory(new CSpxModuleFactory(filename));
    m_factoryMap->emplace(filename, factory);

    return factory;
}

CSpxModuleFactory::CSpxModuleFactory(const std::string& filename) :
    m_pfnCreateModuleObject(nullptr)
{
    m_pfnCreateModuleObject = GetCreateModuleObjectFunctionPointer(filename);
    SPX_TRACE_VERBOSE("Load Module Factory ('%s')... %s!", filename.c_str(), !m_pfnCreateModuleObject ? "NOT FOUND" : "SUCCEEDED");
}

CSpxModuleFactory::CSpxModuleFactory(PCREATE_MODULE_OBJECT_FUNC pFunc) :
    m_pfnCreateModuleObject(pFunc)
{
    SPX_TRACE_VERBOSE("Load Module Factory ('carbon')... %s!", !m_pfnCreateModuleObject ? "NOT FOUND" : "SUCCEEDED");
}

void* CSpxModuleFactory::CreateObject(const char* className, const char* interfaceName)
{
    return m_pfnCreateModuleObject != nullptr
        ? m_pfnCreateModuleObject(className, interfaceName)
        : nullptr;
}

CSpxModuleFactory::PCREATE_MODULE_OBJECT_FUNC CSpxModuleFactory::GetCreateModuleObjectFunctionPointer(const std::string& filename)
{
    auto pfn = CSpxDynamicModule::Get(filename)->GetModuleProcAddress("CreateModuleObject");
    return (CSpxModuleFactory::PCREATE_MODULE_OBJECT_FUNC)pfn;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

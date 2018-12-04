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

#include "module_factory.h"
SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName);


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


std::unique_ptr<CSpxModuleFactory::MapType> CSpxModuleFactory::m_factoryMap = std::make_unique<CSpxModuleFactory::MapType>();


std::shared_ptr<ISpxObjectFactory> CSpxModuleFactory::Get(const std::string& filename)
{
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

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
    SPX_DBG_TRACE_VERBOSE("Load Module Factory ('%s')... %s!", filename.c_str(), !m_pfnCreateModuleObject ? "NOT FOUND" : "SUCCEEDED");
}

void* CSpxModuleFactory::CreateObject(const char* className, const char* interfaceName)
{
    return m_pfnCreateModuleObject != nullptr
        ? m_pfnCreateModuleObject(className, interfaceName)
        : nullptr;
}

CSpxModuleFactory::PCREATE_MODULE_OBJECT_FUNC CSpxModuleFactory::GetCreateModuleObjectFunctionPointer(const std::string& filename)
{
    if (filename == "carbon")
    {
        // "carbon" is magic value which means that we don't need to load anything
        // (this code is already a part of a carbon lib linked to an executable that
        // uses carbon API).
        return CreateModuleObject;
    }

    #if _MSC_VER

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
    HMODULE handle = LoadLibraryA(filename.c_str());
#else // Windows Store WinRT app
    HMODULE handle = LoadPackagedLibrary(PAL::ToWString(filename).c_str(), 0);
#endif
    if (handle != NULL)
    {
        return (PCREATE_MODULE_OBJECT_FUNC)GetProcAddress(handle, "CreateModuleObject");
    }
    #else

    void* handle = dlopen(filename.c_str(), RTLD_LOCAL | RTLD_LAZY);
    SPX_DBG_TRACE_VERBOSE_IF(handle != NULL, "dlopen('%s') returned non-NULL", filename.c_str());
    // SPX_DBG_TRACE_VERBOSE_IF(handle == NULL, "dlopen('%s') returned NULL: %s", filename.c_str(), dlerror());

    if (handle != NULL)
    {
        auto pfn = (PCREATE_MODULE_OBJECT_FUNC)dlsym(handle, "CreateModuleObject");
        SPX_DBG_TRACE_VERBOSE_IF(pfn != NULL, "dlsym('CreateModuleObject') returned non-NULL");
        SPX_DBG_TRACE_VERBOSE_IF(pfn == nullptr, "dlsym('CreateModuleObject') returned NULL: %s",  dlerror());

        if (pfn == nullptr)
        {
            SPX_DBG_TRACE_VERBOSE("dlsym('CreateModuleObject') returned NULL: ... thus ... using libMicrosoft.CognitiveServices.Speech.so!CreateModuleObject directly");
            pfn = CreateModuleObject;
        }

        return pfn;
    }

    #endif

    return nullptr;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

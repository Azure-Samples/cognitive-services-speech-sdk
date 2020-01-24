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

#include "exception.h"
#include "module_factory.h"
SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName);


#if _MSC_VER
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#endif

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
#if _MSC_VER
    std::vector<char> basePath(_MAX_PATH); // zero-initialized
    if (::GetModuleFileNameA((HINSTANCE)&__ImageBase, &basePath[0], _MAX_PATH) != 0)
    {
        char *lastBackslash = ::strrchr(&basePath[0], '\\');
        if (lastBackslash)
        {
            // Terminate after final backslash
            *(lastBackslash + 1) = '\0';
        }
    }
    std::string fullPath = std::string(&basePath[0]) + filename;

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
    HMODULE handle = LoadLibraryA(fullPath.c_str());
#else // Windows Store WinRT app
    auto wideFilename = PAL::ToWString(filename);
    HMODULE handle = LoadPackagedLibrary(wideFilename.c_str(), 0);
#endif
    if (handle != NULL)
    {
        return (PCREATE_MODULE_OBJECT_FUNC)GetProcAddress(handle, "CreateModuleObject");
    }
    #else

    void* handle = dlopen(filename.c_str(), RTLD_LOCAL | RTLD_LAZY);
    SPX_TRACE_VERBOSE_IF(handle != NULL, "dlopen('%s') returned non-NULL", filename.c_str());
    SPX_TRACE_VERBOSE_IF(handle == NULL, "dlopen('%s') returned NULL: %s", filename.c_str(), dlerror());

    if (handle != NULL)
    {
        auto pfn = (PCREATE_MODULE_OBJECT_FUNC)dlsym(handle, "CreateModuleObject");
        SPX_TRACE_VERBOSE_IF(pfn != NULL, "dlsym('CreateModuleObject') returned non-NULL");
        SPX_TRACE_VERBOSE_IF(pfn == nullptr, "dlsym('CreateModuleObject') returned NULL: %s",  dlerror());

        if (pfn == nullptr)
        {
            SPX_TRACE_VERBOSE("dlsym('CreateModuleObject') returned NULL: ... thus ... using libMicrosoft.CognitiveServices.Speech.so!CreateModuleObject directly");
            std::string msg = "can't find 'CreateModuleObject' from " + filename;
            ThrowRuntimeError(msg);
        }

        return pfn;
    }

    #endif

    return nullptr;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// dynamic_module.cpp: Implementation definitions for CSpxDynamicModule C++ class
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
#include "exception.h"

#if _MSC_VER
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

/*static*/ std::shared_ptr<CSpxDynamicModule> CSpxDynamicModule::Get(const std::string& filename)
{
    return std::make_shared<CSpxDynamicModule>(filename, NoObject());
}

CSpxDynamicModule::CSpxDynamicModule(const std::string& filename, NoObject)
{
    m_filename = filename;
}

CSpxDynamicModule::SPX_MODULE_FUNC CSpxDynamicModule::GetModuleProcAddress(const std::string& procname)
{
    return GetModuleFunctionPointer(m_filename, procname);
}

CSpxDynamicModule::SPX_MODULE_FUNC CSpxDynamicModule::GetModuleFunctionPointer(
                                                            const std::string& filename,
                                                            const std::string& procname)
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
        return (SPX_MODULE_FUNC)GetProcAddress(handle, procname.c_str());
    }
#else

#if defined(__linux__) && !defined(ANDROID) && !defined(__ANDROID__) && !defined(__APPLE__)
    auto dlopen_flags = RTLD_LOCAL | RTLD_LAZY | RTLD_DEEPBIND;
#else
    auto dlopen_flags = RTLD_LOCAL | RTLD_LAZY;
#endif

    void* handle = dlopen(filename.c_str(), dlopen_flags);
    SPX_TRACE_VERBOSE_IF(handle != NULL, "dlopen('%s') returned non-NULL", filename.c_str());
    SPX_TRACE_VERBOSE_IF(handle == NULL, "dlopen('%s') returned NULL: %s", filename.c_str(), dlerror());

    if (handle != NULL)
    {
        auto pfn = (SPX_MODULE_FUNC)dlsym(handle, procname.c_str());
        SPX_TRACE_VERBOSE_IF(pfn != NULL, "dlsym('%s') returned non-NULL", procname.c_str());
        SPX_TRACE_VERBOSE_IF(pfn == nullptr, "dlsym('%s') returned NULL: %s", procname.c_str(),  dlerror());

        if (pfn == nullptr)
        {
            SPX_TRACE_VERBOSE("dlsym('%s') returned NULL: ... thus ... using libMicrosoft.CognitiveServices.Speech.so!%s directly", procname.c_str(), procname.c_str());
            std::string msg = "can't find '" + procname + "' from " + filename;
            ThrowRuntimeError(msg);
        }

        return pfn;
    }

#endif

    return nullptr;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

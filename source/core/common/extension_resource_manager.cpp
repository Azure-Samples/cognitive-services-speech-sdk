//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// extension_resource_manager.cpp: Implementation definitions for CSpxExtensionResourceManager C++ class
//

#include "stdafx.h"
#include "extension_resource_manager.h"
#include "module_factory.h"
#include "factory_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxExtensionResourceManager::CSpxExtensionResourceManager()
{
    SPX_DBG_TRACE_FUNCTION();
#ifdef __linux__
    m_moduleFactories.push_back(CSpxModuleFactory::Get("libMicrosoft.CognitiveServices.Speech.core.so"));
#elif __MACH__
    m_moduleFactories.push_back(CSpxModuleFactory::Get("libMicrosoft.CognitiveServices.Speech.core.dylib"));
#else
    m_moduleFactories.push_back(CSpxModuleFactory::Get("Microsoft.CognitiveServices.Speech.core.dll"));
#endif
}

CSpxExtensionResourceManager::~CSpxExtensionResourceManager()
{
    SPX_DBG_TRACE_FUNCTION();
}

void* CSpxExtensionResourceManager::CreateObject(const char* className, const char* interfaceName)
{
    // Loop through each of our module factories, and see if they can create the object.
    //
    // ExtensionResourceManager right now only loads one core.dll[.so|.dylib]".
    //
    //

    for (auto factory: m_moduleFactories)
    {
        auto obj = factory->CreateObject(className, interfaceName);
        if (obj != nullptr)
        {
            return obj;
        }
    }
    return nullptr;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

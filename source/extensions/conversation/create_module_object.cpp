//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <create_module_object.h>
#include <factory_helpers.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

#ifndef STATIC_CONVERSATION_LIB

    /// <summary>
    /// This method is invoked when linking dynamically at runtime by the factory methods
    /// </summary>
    /// <param name="className">The name of the class we want to instantiate</param>
    /// <param name="interfaceName">The interface of the class we want to instantiate</param>
    /// <returns>The instantiated instance, or a nullptr</returns>
    SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName)
    {
        SPX_FACTORY_MAP_BEGIN();
        SPX_FACTORY_MAP_ENTRY_FUNC(ConversationTranslationLib_CreateModuleObject);
        SPX_FACTORY_MAP_END();
    }

#endif // !STATIC_CONVERSATION_LIB

}}}} // Microsoft::CognitiveServices::Speech::Impl

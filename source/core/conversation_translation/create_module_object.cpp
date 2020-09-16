//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// create_module_object.cpp: Implementation definitions for *CreateModuleObject* methods
//

#include "common.h"
#include <factory_helpers.h>
#include "conversation_translator.h"
#include "conversation_impl.h"
#include "conversation_translator_connection.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    using namespace Microsoft::CognitiveServices::Speech::Impl::ConversationTranslation;

    /// <summary>
    /// Use this method when statically linking
    /// </summary>
    /// <param name="className">The name of the class we want to instantiate</param>
    /// <param name="interfaceName">The interface of the class we want to instantiate</param>
    /// <returns>The instantiated instance, or a nullptr</returns>
    SPX_EXTERN_C void* ConversationTranslationLib_CreateModuleObject(const char *className, const char* interfaceName)
    {
        SPX_FACTORY_MAP_BEGIN();
            SPX_FACTORY_MAP_ENTRY(CSpxConversationTranslator, ISpxConversationTranslator);
            SPX_FACTORY_MAP_ENTRY(CSpxConversationImpl, ISpxConversation);
            SPX_FACTORY_MAP_ENTRY(CSpxConversationTranslatorConnection, ISpxConnection);
        SPX_FACTORY_MAP_END();
    }

}}}} // Microsoft::CognitiveServices::Speech::Impl

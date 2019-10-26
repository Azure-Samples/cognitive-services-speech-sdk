//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//

#include "stdafx.h"
#include "handle_table.h"
#include "platform.h"
#include <memory>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using typeMap = std::map<const char*, void*>;
using termFnList = std::list<std::function<void(void)>>;

std::mutex CSpxSharedPtrHandleTableManager::s_mutex;

std::unique_ptr<typeMap> CSpxSharedPtrHandleTableManager::s_tables = std::make_unique<typeMap>();
std::unique_ptr<std::list<std::shared_ptr<CSpxHandleCounter>>> CSpxSharedPtrHandleTableManager::s_counters = std::make_unique<std::list<std::shared_ptr<CSpxHandleCounter>>>();

// On linux, this static member is destroyed before LibUnload (marked as __attribute__((destructor))).
// Using a deleter instead to clean everything up before shutting down.
CSpxSharedPtrHandleTableManager::deleted_unique_ptr<termFnList> CSpxSharedPtrHandleTableManager::s_termFns(new termFnList(), [] (termFnList* list) {
    CSpxSharedPtrHandleTableManager::Term();
    delete list;
});

} } } } // Microsoft::CognitiveServices::Speech::Impl


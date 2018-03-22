#include "stdafx.h"
#include "handle_table.h"
#include "platform.h"


namespace CARBON_IMPL_NAMESPACE() {

using typeMap = std::map<const char*, void*>;
using termFnList = std::list<std::function<void(void)>>;

std::mutex CSpxSharedPtrHandleTableManager::s_mutex;

std::unique_ptr<typeMap> CSpxSharedPtrHandleTableManager::s_tables = std::make_unique<typeMap>();

// On linux, this static member is destroyed before LibUnload (marked as __attribute__((destructor))). 
// Using a deleter instead to clean everything up before shutting down.
CSpxSharedPtrHandleTableManager::deleted_unique_ptr<termFnList> CSpxSharedPtrHandleTableManager::s_termFns(new termFnList(), [] (termFnList* list) {
    CSpxSharedPtrHandleTableManager::Term(); 
    delete list;
});

} // CARBON_IMPL_NAMESPACE


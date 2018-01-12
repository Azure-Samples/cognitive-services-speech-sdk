#include "stdafx.h"
#include "handle_table.h"
#include "platform.h"


namespace CARBON_IMPL_NAMESPACE() {

    
std::unique_ptr<std::map<const char*, void*>> CSpxSharedPtrHandleTableManager::s_tables = std::make_unique<std::map<const char*, void*>>();
std::unique_ptr<std::list<std::function<void(void)>>> CSpxSharedPtrHandleTableManager::s_termFns = std::make_unique<std::list<std::function<void(void)>>>();


}; // CARBON_IMPL_NAMESPACE()


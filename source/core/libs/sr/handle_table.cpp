#include "stdafx.h"
#include "handle_table.h"


namespace CARBON_IMPL_NAMESPACE() {

    
std::unique_ptr<std::map<const char*, void*>> CSpxHandleTableManager::m_tables = std::make_unique<std::map<const char*, void*>>();


}; // CARBON_IMPL_NAMESPACE()


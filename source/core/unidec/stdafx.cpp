#include "stdafx.h"
#include "factory_helpers.h"
#include "mock_reco_engine_adapter.h"
#include "unidec_reco_engine_adapter.h"


namespace CARBON_IMPL_NAMESPACE() {


#define SPX_FACTORY_MAP_ENTRY_MOCK(x, y, replaceWith)                           \
    if (PAL::stricmp(className, #x) == 0)                                       \
    {                                                                           \
        if (PAL::stricmp(interfaceName, PAL::GetTypeName<y>().c_str()) == 0)    \
        {                                                                       \
            return SpxFactoryEntryCreateObject<replaceWith, y>();               \
        }                                                                       \
    }


void* MockModuleFactory_CreateModuleObject(const char* className, const char* interfaceName)
{
    static bool mockModuleFactoryEnabled = false;
    if (mockModuleFactoryEnabled)
    {
        SPX_FACTORY_MAP_BEGIN();
        SPX_FACTORY_MAP_ENTRY_MOCK(CSpxUnidecRecoEngineAdapter, ISpxRecoEngineAdapter, CSpxMockRecoEngineAdapter);
        SPX_FACTORY_MAP_END();
    }

    return nullptr;
}


SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY_FUNC(MockModuleFactory_CreateModuleObject);
    SPX_FACTORY_MAP_ENTRY(CSpxUnidecRecoEngineAdapter, ISpxRecoEngineAdapter);
    SPX_FACTORY_MAP_END();
};


} // CARBON_IMPL_NAMESPACE

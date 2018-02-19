#include "stdafx.h"
#include "factory_helpers.h"
#include "unidec_reco_engine_adapter.h"


namespace CARBON_IMPL_NAMESPACE() {


SPX_EXTERN_C SPXDLL_EXPORT void* CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
    SPX_FACTORY_MAP_ENTRY(CSpxUnidecRecoEngineAdapter, ISpxRecoEngineAdapter);
    SPX_FACTORY_MAP_END();
};


} // CARBON_IMPL_NAMESPACE

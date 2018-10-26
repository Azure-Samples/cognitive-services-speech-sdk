project(unidec)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})

include_directories(${INC_DIR})
include_directories("${COMMON}/include")
include_directories("${CARBON_EXTERNAL}/unidec/include")

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/unidec_reco_engine_adapter.h"
    )

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/unidec_reco_engine_adapter.cpp"
    "${SRC_DIR}/stdafx.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

set(UNIDEC_LIBS "${CARBON_EXTERNAL}/unidec/lib/unidecruntime.lib")

if(WIN32)
    list(APPEND UNIDEC_LIBS rpcrt4.lib)
endif()

add_library(unidec SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET unidec PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (unidec PROPERTIES
    DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
    OUTPUT_NAME "carbon-unidec"
    FOLDER core
)

target_link_libraries(unidec PRIVATE ${UNIDEC_LIBS} common)

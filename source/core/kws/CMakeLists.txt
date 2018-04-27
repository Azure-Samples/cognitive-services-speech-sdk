project(kws)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})

include_directories(${INC_DIR})
include_directories("${COMMON}/include")
include_directories("${CARBON_EXTERNAL}/kws/inc")

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/ring_buffer.h"
    "${SRC_DIR}/sdk_kws_engine_adapter.h"
    )

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/ring_buffer.c"
    "${SRC_DIR}/sdk_kws_engine_adapter.cpp"
    "${SRC_DIR}/stdafx.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

set(KWS_LIBS "${CARBON_EXTERNAL}/kws/lib/windows/x64/kwsapi.lib")

add_library(kws SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

set_target_properties (kws PROPERTIES
    DEFINE_SYMBOL  "SPX_CONFIG_EXPORTAPIS" 
    OUTPUT_NAME "carbon-kws"
    FOLDER carbon_libs
)

target_link_libraries(kws PRIVATE ${KWS_LIBS} common ${AZURE_SHARED_LIB})

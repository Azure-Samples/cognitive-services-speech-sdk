project(core2)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
)

set(HEADERS_PRIVATE
    "${SRC_DIR}/create_module_object.h"
)

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/stdafx.cpp"
)

set_source_groups()

add_library(core2 SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(core2
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
    PUBLIC
        ${INC_DIR}
)

add_compile_definitions(USE_NON_CORE_ROOT_SITE)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET core2 PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties(core2 PROPERTIES
    FOLDER core2
    OUTPUT_NAME "Azure.AI.CognitiveServices.Core"
)

target_link_libraries(core2
    PRIVATE
        interfaces
        common
        session2
        ${SPEECHSDK_CORE_LIBRARY}
)

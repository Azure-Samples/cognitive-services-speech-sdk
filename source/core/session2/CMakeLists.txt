project(session2)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
)

set(HEADERS_PRIVATE
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/session2.h"
)

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/session2.cpp"
    "${SRC_DIR}/stdafx.cpp"
)

set_source_groups()

add_library(session2 STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(session2
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
    PUBLIC
        ${INC_DIR}
)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET audio PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties(session2 PROPERTIES
    FOLDER core
)

target_link_libraries(session2 PRIVATE interfaces common)

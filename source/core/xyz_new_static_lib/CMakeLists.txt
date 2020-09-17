project(xyz_new_static_lib)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
)

set(HEADERS_PRIVATE
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/new_class_1.h"
    "${SRC_DIR}/new_class_2.h"
)

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/new_class_1.cpp"
    "${SRC_DIR}/new_class_2.cpp"
    "${SRC_DIR}/stdafx.cpp"
)

set_source_groups()

add_library(xyz_new_static_lib STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(xyz_new_static_lib
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
    PUBLIC
        ${INC_DIR}
)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET xyz_new_static_lib PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties(xyz_new_static_lib PROPERTIES
    FOLDER templates
)

target_link_libraries(xyz_new_static_lib
    PRIVATE
        interfaces
        common
)

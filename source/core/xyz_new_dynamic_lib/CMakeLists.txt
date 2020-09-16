project(xyz_new_dynamic_lib)

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

add_library(xyz_new_dynamic_lib SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(xyz_new_dynamic_lib
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
    PUBLIC
        ${INC_DIR}
)

add_compile_definitions(USE_NON_CORE_ROOT_SITE)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET xyz_new_dynamic_lib PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties(xyz_new_dynamic_lib PROPERTIES
    FOLDER core
)

target_link_libraries(xyz_new_dynamic_lib
    PRIVATE
        interfaces
        common
        # xyz_new_static_lib
        ${SPEECHSDK_CORE_LIBRARY}
)

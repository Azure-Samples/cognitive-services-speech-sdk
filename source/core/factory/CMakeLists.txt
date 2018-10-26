project(factory)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})

include_directories("${COMMON}/include")
include_directories("${SR}")
include_directories("${USP}/include")

set(HEADERS
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    )

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/module_factory.cpp"
    "${SRC_DIR}/resource_manager.cpp"
    "${SRC_DIR}/stdafx.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

add_library(factory STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET factory PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (factory PROPERTIES
    FOLDER core
)

target_link_libraries(factory sr audio usp ${CMAKE_DL_LIBS})

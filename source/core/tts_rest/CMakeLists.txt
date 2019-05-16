project(tts_rest)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")

include_directories(${INC_DIR})
include_directories("${COMMON}/include")
include_directories("${USP}/include")
include_directories("${COMMON_GENERATED_INCLUDE}")

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/rest_tts_engine_adapter.h"
    "${SRC_DIR}/rest_tts_helper.h"
    "${SRC_DIR}/authenticator.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/rest_tts_engine_adapter.cpp"
    "${SRC_DIR}/authenticator.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(tts_rest STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET tts_rest PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (tts_rest PROPERTIES
    FOLDER core
)

target_link_libraries(tts_rest usp)

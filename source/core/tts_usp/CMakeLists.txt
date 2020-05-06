project(tts_usp)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")

include_directories(${INC_DIR})
include_directories(${SR})
include_directories(${TTS})
include_directories(${TTS_REST})
include_directories("${USP}/include")
include_directories("${COMMON}/include")
include_directories("${COMMON_GENERATED_INCLUDE}")

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/usp_tts_engine_adapter.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/usp_tts_engine_adapter.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(tts_usp STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET tts_usp PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (tts_usp PROPERTIES
    FOLDER core
)

target_link_libraries(tts_usp usp)

if (MSVC)
   set_target_properties(tts_usp PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)

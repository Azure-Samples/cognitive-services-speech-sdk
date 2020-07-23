project(tts_rest)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
    "${INC_DIR}/rest_tts_engine_adapter.h"
)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/rest_tts_engine_adapter.cpp"
    )

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(tts_rest STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(tts_rest
    PRIVATE
        ${SRC_DIR}
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        ${CARBON_INCLUDE}
        "${CARBON_EXTERNAL}/json"
        "${COMMON_GENERATED_INCLUDE}"
    PUBLIC
        ${INC_DIR}
)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET tts_rest PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (tts_rest PROPERTIES
    FOLDER core
)

target_link_libraries(tts_rest usp common)

if (MSVC)
   set_target_properties(tts_rest PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)

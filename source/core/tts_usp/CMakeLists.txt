project(tts_usp)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
    "${INC_DIR}/usp_tts_engine_adapter.h"
)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/usp_tts_engine_adapter.cpp"
    )

set_source_groups()

add_library(tts_usp STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(tts_usp
    PRIVATE
        ${SRC_DIR}
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        "${CARBON_EXTERNAL}/json"
        ${COMMON_GENERATED_INCLUDE}
    PUBLIC
        ${INC_DIR}
)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET tts_usp PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (tts_usp PROPERTIES
    FOLDER core/tts
)

target_link_libraries(tts_usp interfaces usp common)

if (MSVC)
   set_target_properties(tts_usp PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)

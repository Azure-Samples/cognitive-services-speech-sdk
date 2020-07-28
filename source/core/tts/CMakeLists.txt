project(tts)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")


set(HEADERS
    "${INC_DIR}/speech_synthesis_api_factory.h"
    )

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/synthesizer.h"
    "${SRC_DIR}/synthesis_result.h"
    "${SRC_DIR}/synthesis_event_args.h"
    "${SRC_DIR}/word_boundary_event_args.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/speech_synthesis_api_factory.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/synthesizer.cpp"
    "${SRC_DIR}/synthesis_result.cpp"
    "${SRC_DIR}/synthesis_event_args.cpp"
    "${SRC_DIR}/word_boundary_event_args.cpp"
    )

set_source_groups()

add_library(tts STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(tts
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
    set_property(TARGET tts PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (tts PROPERTIES
    FOLDER core/tts
)

target_link_libraries(tts PRIVATE interfaces common usp tts_rest tts_usp)

if (MSVC)
   set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)

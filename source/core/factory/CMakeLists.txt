project(factory)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
    "${SRC_DIR}/stdafx.h"
    "${INC_DIR}/create_module_object.h"
    "${INC_DIR}/resource_manager.h"
    )

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/resource_manager.cpp"
    "${SRC_DIR}/factory_site_helpers.cpp"
    "${SRC_DIR}/stdafx.cpp"
    )

set_source_groups()

add_library(factory STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(factory
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        "${CARBON_EXTERNAL}/json"
    PUBLIC
        ${INC_DIR}
)

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET factory PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (factory PROPERTIES
    FOLDER core
)

if(STATIC_EMBEDDEDSR_EXTENSION)
    list(APPEND additional_static_libs embedded_sr)
endif()

target_link_libraries(factory
                      PRIVATE interfaces
                      PRIVATE sr
                      PRIVATE tts
                      PRIVATE audio
                      PRIVATE usp
                      PRIVATE tts_rest
                      PRIVATE tts_usp
                      PRIVATE conversation_translation
                      PRIVATE common
                      PRIVATE speaker_recognition
                      PRIVATE data
                      PRIVATE session2
                      PRIVATE xyz_new_lib
                      PRIVATE ${CMAKE_DL_LIBS}
                      PRIVATE ${additional_static_libs})

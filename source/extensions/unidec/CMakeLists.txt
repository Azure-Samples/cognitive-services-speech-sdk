project(unidec)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})

include_directories(${INC_DIR})
include_directories("${CARBON_EXTERNAL}/json")
include_directories("${CARBON_EXTERNAL}/unidec/Richland.Speech.UnidecRuntime/native")

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/unidec_reco_engine_adapter.h"
    )

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/unidec_reco_engine_adapter.cpp"
    "${SRC_DIR}/stdafx.cpp"
    )

set_source_groups()

if(SPEECHSDK_TARGET_PLATFORM STREQUAL Linux-arm64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXPLATFORM")
    set(UNIDEC_LIBS "${CARBON_EXTERNAL}/unidec/Richland.Speech.UnidecRuntime/linux-aarch64-platforms/lib/libSpeechToText.so")
elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL Linux-x64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXPLATFORM")
    set(UNIDEC_LIBS "${CARBON_EXTERNAL}/unidec/Richland.Speech.UnidecRuntime/linux-amd64-platform/lib/libSpeechToText.so")
elseif((SPEECHSDK_TARGET_PLATFORM STREQUAL Windows-x64) OR
       (SPEECHSDK_TARGET_PLATFORM STREQUAL WindowsUwp-x64))
    set(UNIDEC_LIBS "${CARBON_EXTERNAL}/unidec/Richland.Speech.UnidecRuntime/native/UnidecRuntime.lib")
    list(APPEND UNIDEC_LIBS rpcrt4.lib)
endif()

add_library(unidec SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH "@loader_path")
elseif(UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH "$ORIGIN")
endif()

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET unidec PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

add_compile_definitions(USE_NON_CORE_ROOT_SITE)

set_target_properties (unidec PROPERTIES
    DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
    OUTPUT_NAME "Microsoft.CognitiveServices.Speech.extension.embedded.sr"
    FOLDER extensions
)

target_link_libraries(unidec PRIVATE interfaces ${UNIDEC_LIBS} common ${SPEECHSDK_CORE_LIBRARY})

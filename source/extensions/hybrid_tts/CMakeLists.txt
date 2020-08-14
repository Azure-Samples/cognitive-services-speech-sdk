project(hybrid_tts)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/local_tts_engine_adapter.h"
    "${SRC_DIR}/hybrid_tts_engine_adapter.h"
    "${SRC_DIR}/ms_tts_output.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/local_tts_engine_adapter.cpp"
    "${SRC_DIR}/hybrid_tts_engine_adapter.cpp"
    )

set_source_groups()

add_library(hybrid_tts SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(hybrid_tts
    PRIVATE
        ${SRC_DIR}
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        "${CARBON_EXTERNAL}/json"
        ${COMMON_GENERATED_INCLUDE}
        "${CARBON_EXTERNAL}/offline_tts/include"
    PUBLIC
        ${INC_DIR}
)

add_compile_definitions(USE_NON_CORE_ROOT_SITE)

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH "@loader_path")
elseif(UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH "$ORIGIN")
endif()

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET hybrid_tts PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

set_target_properties (hybrid_tts PROPERTIES
    OUTPUT_NAME "Microsoft.CognitiveServices.Speech.extension.embedded.tts"
    FOLDER extensions
)

if(SPEECHSDK_TARGET_PLATFORM STREQUAL "Windows-x64" OR SPEECHSDK_TARGET_PLATFORM STREQUAL "WindowsUwp-x64")
  set(OFFLINE_TTS_LIBS "${CARBON_EXTERNAL}/offline_tts/Windows/x64/libMSTTSEngine.lib")
elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL "Linux-arm64")
  set(OFFLINE_TTS_LIBS "${CARBON_EXTERNAL}/offline_tts/Linux/arm64/libMSTTSEngine.so")
elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL "Linux-arm32")
  set(OFFLINE_TTS_LIBS "${CARBON_EXTERNAL}/offline_tts/Linux/arm32/libMSTTSEngine.so")
elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL "Linux-x64")
  set(OFFLINE_TTS_LIBS "${CARBON_EXTERNAL}/offline_tts/Linux/x64/libMSTTSEngine.so")
elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL "Android-arm32")
  set(OFFLINE_TTS_LIBS "${CARBON_EXTERNAL}/offline_tts/Android/arm32/libMSTTSEngine.so")
elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL "Android-arm64")
  set(OFFLINE_TTS_LIBS "${CARBON_EXTERNAL}/offline_tts/Android/arm64/libMSTTSEngine.so")
# elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL "OSX-x64")
#   set(OFFLINE_TTS_LIBS "${CARBON_EXTERNAL}/offline_tts/osx/libMSTTSEngine.dylib")
endif()

target_link_libraries(hybrid_tts PRIVATE ${OFFLINE_TTS_LIBS} interfaces common audio ${SPEECHSDK_CORE_LIBRARY})
project(speechsdk_compression_silk)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/stdafx.h"
    "${SRC_DIR}/silk_codec_native.h"
    "${SRC_DIR}/silk_codec_impl.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/silk_codec_native.cpp"
    "${SRC_DIR}/silk_codec_impl.cpp"
    )

  set_source_groups()

# HINTS ${CARBON_EXTERNAL}/silk-codec/lib/${CMAKE_SYSTEM_NAME}/${PLATFORM_NAME}/${CMAKE_SYSTEM_PROCESSOR}

if (WIN32)
  set (SILKLIBHINT ${CARBON_EXTERNAL}/silk-codec/lib/Windows/${SPEECHSDK_ARCH})
else()
  if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    set (SILKLIBHINT ${CARBON_EXTERNAL}/silk-codec/lib/${CMAKE_SYSTEM_NAME}/${CMAKE_ANDROID_ARCH_ABI})
  else()
    set (SILKLIBHINT ${CARBON_EXTERNAL}/silk-codec/lib/${CMAKE_SYSTEM_NAME}/${SPEECHSDK_ARCH})
  endif()
endif()

message("Silk lib path hint: " ${SILKLIBHINT})

# find SILK library.
find_library(SILK_LIBRARIES
        NAMES silk
        HINTS ${SILKLIBHINT}
        NO_CMAKE_FIND_ROOT_PATH
    )

# SILK_LIBRARIES will be "SILK_LIBRARIES-NOTFOUND" if not found

if(WIN32)
  set(SPEECHSDK_ORIGINAL_FILENAME "Microsoft.CognitiveServices.Speech.extension.silk_codec.dll")
  configure_file(
    ${CARBON_ROOT}/ci/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc)

  list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc")
endif()

if(MSVC)
    # ignore lack of pdbs from external libs
    set(CMAKE_SHARED_LINKER_FLAGS "$(CMAKE_SHARED_LINKER_FLAGS) /IGNORE:4099")

    # ignore MSVCRT requested by the external libs, use the debug version
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:MSVCRT.lib")
endif()

if(SPEECHSDK_OS MATCHES "^IOS")
    add_library(speechsdk_compression_silk STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})
else()
    add_library(speechsdk_compression_silk SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

    set_target_properties(speechsdk_compression_silk PROPERTIES
        # DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
        OUTPUT_NAME "Microsoft.CognitiveServices.Speech.extension.silk_codec"
        FOLDER "extensions/audio-compression/Silk"
        )
endif()

# Set USE_NON_CORE_ROOT_SITE based on if we are building extensions or unit tests
add_compile_definitions(USE_NON_CORE_ROOT_SITE)

target_include_directories(speechsdk_compression_silk
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        "${COMMON}/include"
        "${CARBON_EXTERNAL}/json"
        "${CARBON_EXTERNAL}/silk-codec"
    )

target_link_libraries(speechsdk_compression_silk PRIVATE common)

if(SPEECHSDK_OS MATCHES "^IOS")
    target_link_libraries(speechsdk_compression_silk
        PRIVATE
        interfaces
        ${SILK_LIBRARIES}
        )
else()
    target_link_libraries(speechsdk_compression_silk
        PRIVATE
        interfaces
        ${SILK_LIBRARIES}
        )
endif()
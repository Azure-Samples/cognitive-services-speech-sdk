project(speechsdk_compression_silk)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")
include_directories("${CARBON_EXTERNAL}/silk-codec")
include_directories("${COMMON}/include")


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

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

# HINTS ${CARBON_EXTERNAL}/silk-codec/lib/${CMAKE_SYSTEM_NAME}/${PLATFORM_NAME}/${CMAKE_SYSTEM_PROCESSOR}
if (WIN32)
  set (SILKLIBHINT ${CARBON_EXTERNAL}/silk-codec/lib/Windows/${SPEECHSDK_ARCH})
else()
  set (SILKLIBHINT ${CARBON_EXTERNAL}/silk-codec/lib/${CMAKE_SYSTEM_NAME}/${SPEECHSDK_ARCH})
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
        ${CARBON_INCLUDE}
        "${COMMON}/include"
    )

if(SPEECHSDK_OS MATCHES "^IOS")
    target_link_libraries(speechsdk_compression_silk
        ${SILK_LIBRARIES}
        )
else()
    target_link_libraries(speechsdk_compression_silk
        ${SILK_LIBRARIES}
        )
endif()
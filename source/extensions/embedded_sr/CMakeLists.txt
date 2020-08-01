project(embedded_sr)

if(NOT (BUILD_UNIDEC OR BUILD_RNNT))
    message(FATAL_ERROR "Nothing to build.")
endif()

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(RNNT_SRC_DIR "${SRC_DIR}/rnnt")
set(UNIDEC_SRC_DIR "${SRC_DIR}/unidec")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})

include_directories("${CARBON_EXTERNAL}/json")
include_directories("${SRC_DIR}")
if(BUILD_RNNT)
    include_directories("${RNNT_SRC_DIR}")
endif()
if(BUILD_UNIDEC)
    include_directories("${UNIDEC_SRC_DIR}")
    include_directories("${CARBON_EXTERNAL}/unidec/Richland.Speech.UnidecRuntime/native")
endif()
include_directories("${CARBON_CORE}/sr")

if(BUILD_RNNT)
    set(RNNT_HEADERS_PRIVATE
        "${RNNT_SRC_DIR}/conditional_variable_buffer.h"
        "${RNNT_SRC_DIR}/rnnt.h"
        "${RNNT_SRC_DIR}/rnntmessages.h"
        "${RNNT_SRC_DIR}/rnnt_client.h"
        "${RNNT_SRC_DIR}/rnnt_dll.h"
        "${RNNT_SRC_DIR}/rnnt_reco_engine_adapter.h"
        "${RNNT_SRC_DIR}/rnnt_tokens.h"
    )

    set(RNNT_SOURCES
        "${RNNT_SRC_DIR}/rnnt_client.cpp"
        "${RNNT_SRC_DIR}/rnnt_reco_engine_adapter.cpp"
    )
endif()

if(BUILD_UNIDEC)
    set(UNIDEC_HEADERS_PRIVATE
        "${UNIDEC_SRC_DIR}/unidec_config.h"
        "${UNIDEC_SRC_DIR}/unidec_reco_engine_adapter.h"
        "${UNIDEC_SRC_DIR}/unidec_runtime_dll.h"
    )

    set(UNIDEC_SOURCES
        "${UNIDEC_SRC_DIR}/unidec_reco_engine_adapter.cpp"
    )
endif()

set(HEADERS)

set(HEADERS_PRIVATE
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/stdafx.h"
)
list(APPEND HEADERS_PRIVATE ${RNNT_HEADERS_PRIVATE})
list(APPEND HEADERS_PRIVATE ${UNIDEC_HEADERS_PRIVATE})

set(SOURCES
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/stdafx.cpp"
)
list(APPEND SOURCES ${RNNT_SOURCES})
list(APPEND SOURCES ${UNIDEC_SOURCES})

set_source_groups()

if(BUILD_RNNT)
    if(SPEECHSDK_OS STREQUAL "Android" OR SPEECHSDK_OS STREQUAL "Linux")
        if(SPEECHSDK_ARCH STREQUAL "arm32")
            set(MAS_PATH "${CARBON_EXTERNAL}/mas/${SPEECHSDK_OS}/arm/${CMAKE_BUILD_TYPE}")
        else()
            set(MAS_PATH "${CARBON_EXTERNAL}/mas/${SPEECHSDK_OS}/${SPEECHSDK_ARCH}/${CMAKE_BUILD_TYPE}")
        endif()
    elseif(SPEECHSDK_OS MATCHES "^Windows")
        if(SPEECHSDK_ARCH STREQUAL "x86")
            set(MAS_PATH "${CARBON_EXTERNAL}/mas/Windows/Win32/${CMAKE_BUILD_TYPE}")
        elseif(SPEECHSDK_ARCH STREQUAL "x64")
            set(MAS_PATH "${CARBON_EXTERNAL}/mas/Windows/x64/${CMAKE_BUILD_TYPE}")
        elseif(SPEECHSDK_ARCH STREQUAL "arm32")
            set(MAS_PATH "${CARBON_EXTERNAL}/mas/Windows/ARM/${CMAKE_BUILD_TYPE}")
        else()
            set(MAS_PATH "${CARBON_EXTERNAL}/mas/Windows/ARM64/${CMAKE_BUILD_TYPE}")
        endif()
    elseif(SPEECHSDK_OS MATCHES "^IOS")
        if(SPEECHSDK_OS STREQUAL "IOSSIMULATOR")
            if(SPEECHSDK_ARCH STREQUAL "x64")
                set(MAS_PATH "${CARBON_EXTERNAL}/mas/iOS/x64/${CMAKE_BUILD_TYPE}")
            else()
                # Should not reach here.
                message(FATAL_ERROR "${SPEECHSDK_ARCH} is not supported for ${SPEECHSDK_OS}.")
            endif()
        else()
            if(SPEECHSDK_ARCH STREQUAL "arm64" OR SPEECHSDK_ARCH STREQUAL "all")
                set(MAS_PATH "${CARBON_EXTERNAL}/mas/iOS/arm64/${CMAKE_BUILD_TYPE}")
            elseif(SPEECHSDK_ARCH STREQUAL "armv7")
                set(MAS_PATH "${CARBON_EXTERNAL}/mas/iOS/armv7/${CMAKE_BUILD_TYPE}")
            else()
                # Should not reach here.
                message(FATAL_ERROR "${SPEECHSDK_ARCH} is not supported for ${SPEECHSDK_OS}.")
            endif()
        endif()
    elseif(SPEECHSDK_OS STREQUAL "OSX")
        if(SPEECHSDK_ARCH STREQUAL "x64")
            set(MAS_PATH "${CARBON_EXTERNAL}/mas/OSX/x64/${CMAKE_BUILD_TYPE}")
        else()
            # Should not reach here.
            message(FATAL_ERROR "${SPEECHSDK_ARCH} is not supported for ${SPEECHSDK_OS}.")
        endif()
    endif()

    include_directories(${MAS_PATH})
endif()

if(BUILD_UNIDEC)
    if(SPEECHSDK_TARGET_PLATFORM STREQUAL Linux-arm64 OR SPEECHSDK_TARGET_PLATFORM STREQUAL Linux-x64)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXPLATFORM")
    elseif(SPEECHSDK_TARGET_PLATFORM STREQUAL Windows-x64 OR SPEECHSDK_TARGET_PLATFORM STREQUAL WindowsUwp-x64)
        set(UNIDEC_LIBS rpcrt4.lib)
    endif()
endif()

add_compile_definitions(USE_NON_CORE_ROOT_SITE)

if(STATIC_EMBEDDEDSR_EXTENSION)
    add_library(${PROJECT_NAME} STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})
else()
    add_library(${PROJECT_NAME} SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
        OUTPUT_NAME "Microsoft.CognitiveServices.Speech.extension.embedded.sr"
        FOLDER "extensions/embedded_sr"
    )
endif()

if(WIN32 AND SPECTRE_MITIGATION)
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_GLOBAL_SpectreMitigation "Spectre")
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH "@loader_path")
elseif(UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH "$ORIGIN")
endif()

if(STATIC_EMBEDDEDSR_EXTENSION)
    add_library(unimic_runtime_static STATIC IMPORTED)
    set_target_properties(unimic_runtime_static PROPERTIES IMPORTED_LOCATION ${MAS_PATH}/libunimic_runtime_static.a)
    target_link_libraries(${PROJECT_NAME} PRIVATE interfaces common sr logging unimic_runtime_static)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE interfaces ${UNIDEC_LIBS} common sr ${SPEECHSDK_CORE_LIBRARY})
endif()

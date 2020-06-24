project(${SPEECHSDK_CORE_LIBRARY})

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${FACTORY}/include")
include_directories("${COMMON}/include")
include_directories("${LOGGING}/include")

set(HEADERS)

set(HEADERS_PRIVATE "${SRC_DIR}/stdafx.h")

set(SOURCES
  "${SRC_DIR}/dll.cpp"
  "${SRC_DIR}/stdafx.cpp"
  )

if(WIN32)
  if (MSVC)
    set_source_files_properties("${SRC_DIR}/stdafx.cpp"
      PROPERTIES
      COMPILE_FLAGS "/Ycstdafx.h"
      )

    # For precompiled header.
    # Set
    # "Precompiled Header" to "Use (/Yu)"
    # "Precompiled Header File" to "stdafx.h"
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yustdafx.h /FIstdafx.h")

  endif()
  list(APPEND HEADERS_PRIVATE "${SRC_DIR}/windows/targetver.h")

  set(SPEECHSDK_ORIGINAL_FILENAME "${SPEECHSDK_CORE_LIBRARY}.dll")
  configure_file(
    ${CARBON_ROOT}/ci/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc)

    list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc")
endif()

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES} $<TARGET_OBJECTS:c_api>)

# Disable LNK4286 (caused by importing diagnostics_log_message)
if(MSVC)
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4286")
endif()

if(APPLE)
  add_library(${PROJECT_NAME}_static STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES} $<TARGET_OBJECTS:c_api>)
  set_target_properties(${PROJECT_NAME}
    PROPERTIES BUILD_RPATH "@loader_path")
elseif(UNIX)
  set_target_properties(${PROJECT_NAME}
    PROPERTIES BUILD_RPATH "$ORIGIN")
endif()

if(WIN32 AND SPECTRE_MITIGATION)
  set_property(TARGET ${PROJECT_NAME} PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
  if (${use_openssl})
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
  endif()
endif()

set_target_properties (${PROJECT_NAME} PROPERTIES
  FOLDER core)

# the following list are needed to make ld producing a .core.so that can be loaded by a python wheel
target_link_libraries(${PROJECT_NAME}
    PRIVATE common
    PRIVATE logging
    PRIVATE factory
    PRIVATE speaker_recognition
    PRIVATE usp
  )

if(ANDROID OR (${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
  if(${use_openssl})
    target_link_libraries(${PROJECT_NAME} PRIVATE z dl) # libz dl
  endif()
endif()

if(APPLE)
  FIND_LIBRARY(CORE_FOUNDATION CoreFoundation)
  if (NOT CORE_FOUNDATION)
    message(FATAL_ERROR "CoreFoundation not found")
  endif()
  target_link_libraries(${PROJECT_NAME}
    PRIVATE ${CORE_FOUNDATION}
    )
endif()

if (EXTENSION_CODEC_ENABLED AND STATIC_CODEC_EXTENSION)
  target_link_libraries(${PROJECT_NAME} PRIVATE codec)
endif()

if (EXTENSION_KWS_ENABLED AND STATIC_KWS_EXTENSION)
  target_link_libraries(${PROJECT_NAME} PRIVATE kws_factory)
endif()

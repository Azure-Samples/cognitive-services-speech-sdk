project(${SPEECHSDK_CONVERSATION_LIBRARY})

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")

include_directories(${INC_DIR})
include_directories("${COMMON}/include")
include_directories("${COMMON_GENERATED_INCLUDE}")
include_directories("${CONVERSATION_TRANSLATION}/include")

set(HEADERS
    "${SRC_DIR}/stdafx.h"
    )

set(SOURCES
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/dll.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    )

if(WIN32)
  list(APPEND HEADERS "${SRC_DIR}/windows/targetver.h")

  set(SPEECHSDK_ORIGINAL_FILENAME "${SPEECHSDK_CONVERSATION_LIBRARY}.dll")
  configure_file(
    ${CARBON_ROOT}/ci/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc)

    list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc")
endif()

source_group("Header Files (private)" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})

if(APPLE) 
  set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH "@loader_path")
elseif(UNIX)
  set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_RPATH "$ORIGIN")
endif()

if(WIN32 AND SPECTRE_MITIGATION)
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

if (UNIX)
    add_definitions(-Wno-error)
endif()

set_target_properties (${PROJECT_NAME} PROPERTIES
  DEFINE_SYMBOL "SPX_CONFIG_EXPORTAPIS"
  FOLDER core
)

add_compile_definitions(USE_NON_CORE_ROOT_SITE)

if(DEFINED STATIC_CONVERSATION_LIB)
    target_compile_definitions(${PROJECT_NAME} PUBLIC STATIC_CONVERSATION_LIB)
    target_link_libraries(${PROJECT_NAME}
                          PRIVATE common
                          ${SPEECHSDK_CORE_LIBRARY})
else()
    target_link_libraries(${PROJECT_NAME}
                          PRIVATE conversation_translation
                          ${SPEECHSDK_CORE_LIBRARY})
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
  if (${use_openssl})
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
  endif()
endif()

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

if (MSVC)
   set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)

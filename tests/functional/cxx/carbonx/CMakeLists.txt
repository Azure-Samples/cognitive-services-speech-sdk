project(carbon_test_console)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include") # for PAL utils


set(HEADERS)

set(HEADERS_PRIVATE 
    "${SRC_DIR}/carbon_test_console.h"
    "${SRC_DIR}/carbonx_pal.h"
    "${SRC_DIR}/stdafx.h"
)

set(SOURCES 
    "${SRC_DIR}/carbon_test_console.cpp"
    "${SRC_DIR}/channel9.cpp"
    "${SRC_DIR}/hello_world.cpp"
    "${SRC_DIR}/hello_world_c.cpp"
    "${SRC_DIR}/stdafx.cpp"
)

include_directories("${SRC_DIR}")

if(WIN32)
    include_directories("${SRC_DIR}/windows") # for "stdafx.h"
    list(APPEND HEADERS_PRIVATE
        "${SRC_DIR}/windows/targetver.h"
    )
    list(APPEND SOURCES
        "${SRC_DIR}/windows/carbonx_pal.cpp"
    )
else()
    list(APPEND SOURCES
        "${SRC_DIR}/linux/carbonx_pal.cpp"
    )
endif()

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

source_group("Speech C API headers" FILES ${SPEECH_C_API_HEADERS})
source_group("Speech CXX API headers" FILES ${SPEECH_CXX_API_HEADERS})

add_executable(carbonx 
    ${SPEECH_C_API_HEADERS} ${SPEECH_CXX_API_HEADERS}
    ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES}
    )
# common is needed for PAL etc
target_link_libraries(carbonx common ${SPEECHSDK_CORE_LIBRARY})

if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
if (${use_openssl})
        set_target_properties(carbonx PROPERTIES LINK_FLAGS "/ignore:4099")
endif()
endif()

set_target_properties (carbonx PROPERTIES
    FOLDER tools
    OUTPUT_NAME carbonx
)

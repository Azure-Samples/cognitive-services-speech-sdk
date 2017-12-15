project(carbon_test_console)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})

set(HEADERS)

set(HEADERS_PRIVATE 
    "${SRC_DIR}/carbon_test_console.h"
    "${SRC_DIR}/carbonx_pal.h"
)

set(SOURCES 
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/carbon_test_console.cpp"
    "${SRC_DIR}/hello_world.cpp"
    "${SRC_DIR}/hello_world_c.cpp"
)

if(WIN32)
    include_directories("${SRC_DIR}") # for "carbonx_pal.h"   
    include_directories("${SRC_DIR}/windows") # for "stdafx.h"   
    list(APPEND HEADERS_PRIVATE
        "${SRC_DIR}/windows/stdafx.h"
        "${SRC_DIR}/windows/targetver.h"
    )
    list(APPEND SOURCES
        "${SRC_DIR}/windows/carbonx_pal.cpp"
    )
endif()

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

source_group("Speech C API headers" FILES ${SPEECH_C_API_HEADERS})
source_group("Speech CXX API headers" FILES ${SPEECH_CXX_API_HEADERS})

add_executable(carbon_test_console 
    ${SPEECH_C_API_HEADERS} ${SPEECH_CXX_API_HEADERS}
    ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES}
    )
 
target_link_libraries(carbon_test_console carbon)

set_target_properties (carbon_test_console PROPERTIES
    FOLDER tools
    OUTPUT_NAME carbonx
)
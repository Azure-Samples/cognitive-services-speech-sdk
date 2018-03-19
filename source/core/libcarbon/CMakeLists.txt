project(libcarbon)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")
include_directories(${SHARED_UTIL_INC_FOLDER})

set(HEADERS)

set(HEADERS_PRIVATE "${SRC_DIR}/stdafx.h")

set(SOURCES 
    "${SRC_DIR}/stdafx.cpp"
    "${SRC_DIR}/dll.cpp"
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
endif()

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES} $<TARGET_OBJECTS:c_api>)

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "carbon"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE sr
    PRIVATE audio 
    PRIVATE usp
    PRIVATE common 
    PRIVATE factory)
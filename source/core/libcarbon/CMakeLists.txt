project(${SPEECHSDK_CORE_LIBRARY})

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")

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

endif()

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES} $<TARGET_OBJECTS:c_api>)

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

if(ANDROID)

target_link_libraries(${PROJECT_NAME}
    PRIVATE sr
    PRIVATE audio 
    PRIVATE usp
    PRIVATE common 
    PRIVATE factory
    PRIVATE z) # libz

else()

target_link_libraries(${PROJECT_NAME}
    PRIVATE sr
    PRIVATE audio 
    PRIVATE usp
    PRIVATE common 
    PRIVATE factory)

endif()

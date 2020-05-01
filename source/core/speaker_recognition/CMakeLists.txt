project(speaker_recognition)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories(${SRC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")
include_directories("${USP}/include")
include_directories("${PAL}/include")
include_directories("${SR}")
include_directories("${CARBON_EXTERNAL}/json")

file(GLOB HEADERS "${SRC_DIR}/*.h")
file(GLOB SOURCES "${SRC_DIR}/*.cpp")

# make this the last source file to avoid link warning LNK4221
#list(APPEND SOURCES "${SRC_DIR}/stdafx.cpp")

source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

add_library(speaker_recognition STATIC ${HEADERS} ${SOURCES})

if (WIN32 AND SPECTRE_MITIGATION)
set_property(TARGET speaker_recognition PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
# Need C++17 for WinRT API access via cppwinrt.
set_target_properties(speaker_recognition PROPERTIES CXX_STANDARD 17)
target_compile_definitions(speaker_recognition PUBLIC _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

set_target_properties(speaker_recognition PROPERTIES
    FOLDER core
)

target_link_libraries(${PROJECT_NAME}
                      PRIVATE logging
                      PRIVATE audio
                      PRIVATE common)

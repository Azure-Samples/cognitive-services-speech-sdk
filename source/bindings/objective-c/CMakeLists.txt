project(${SPEECHSDK_CORE_LIBRARY})

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(OBJC_PUBLIC_HEADER_DIR "${PROJECT_SOURCE_DIR}/public")
set(LIBCARBON_SRC_DIR "${PROJECT_SOURCE_DIR}/../../core/libcarbon")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${OBJC_PUBLIC_HEADER_DIR})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")

FILE(GLOB OBJC_PUBLIC_HEADERS "${OBJC_PUBLIC_HEADER_DIR}/*.h")
FILE(GLOB OBJC_PRIVATE_HEADERS "${SRC_DIR}/*.h")
FILE(GLOB OBJC_SOURCE "${SRC_DIR}/*.mm")

set(HEADERS ${OBJC_PUBLIC_HEADERS}) 
set(HEADERS_PRIVATE ${OBJC_PRIVATE_HEADERS}) 
set(SOURCES ${OBJC_SOURCE})

source_group("Header Files (public)" FILES ${HEADERS})
source_group("Header Files (private)" FILES ${HEADERS_PRIVATE})
source_group("Source Files" FILES ${SOURCES})

add_library(${PROJECT_NAME} SHARED ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES} $<TARGET_OBJECTS:c_api>)


target_link_libraries(${PROJECT_NAME}
    PRIVATE sr
    PRIVATE audio 
    PRIVATE usp
    PRIVATE common 
    PRIVATE factory)


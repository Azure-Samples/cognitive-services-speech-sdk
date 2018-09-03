project(${SPEECHSDK_CORE_LIBRARY})

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(OBJC_PUBLIC_HEADER_DIR "${PROJECT_SOURCE_DIR}/public")
set(LIBCARBON_SRC_DIR "${PROJECT_SOURCE_DIR}/../../core/libcarbon")

include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${OBJC_PUBLIC_HEADER_DIR})
include_directories(${CARBON_INCLUDE})
include_directories("${COMMON}/include")


set(HEADERS "${OBJC_PUBLIC_HEADER_DIR}/speech_factory.h"
            "${OBJC_PUBLIC_HEADER_DIR}/speech_recognizer.h"
            "${OBJC_PUBLIC_HEADER_DIR}/speech_recognition_result.h"
            "${OBJC_PUBLIC_HEADER_DIR}/speech_recognition_result_event_args.h"
)

set(HEADERS_PRIVATE "${SRC_DIR}/speech_recognizer_private.h"                  
                    "${SRC_DIR}/speech_recognition_result_private.h"                    
                    "${SRC_DIR}/speech_recognition_result_event_args_private.h"
                    "${LIBCARBON_SRC_DIR}/stdafx.h"
)

set(SOURCES 
    "${SRC_DIR}/speech_factory.mm"
    "${SRC_DIR}/speech_recognizer.mm"
    "${SRC_DIR}/speech_recognition_result.mm"
    "${SRC_DIR}/speech_recognition_result_event_args.mm"
    "${LIBCARBON_SRC_DIR}/dll.cpp"
    "${LIBCARBON_SRC_DIR}/stdafx.cpp"
)

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


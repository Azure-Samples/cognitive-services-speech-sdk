project(core_unit_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories("${CARBON_CORE}/include")
include_directories("${COMMON}/include")
include_directories("${USP}/include")
include_directories("${SR}")
include_directories("${AUDIO}")
include_directories("${AUDIO}/include")
include_directories("${CARBON_EXTERNAL}/Catch2")
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${TEST_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")

set (HEADERS_PRIVATE 
    "${SRC_DIR}/sequence_generator_audio_stream_reader.h"
    )

set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/single_to_many_stream_reader_adapter_test.cpp"
    "${SRC_DIR}/sequence_generator_audio_stream_reader.cpp"
    "${SRC_DIR}/read_write_ring_buffer_test_case.cpp"
    )

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${TEST_HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES} ${TEST_HEADERS})
target_link_libraries(${PROJECT_NAME} usp sr common audio conversation_translation ${TEST_LIBS})

if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
if (${use_openssl})
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
endif()
endif()

if (NOT WIN32)
    find_package (Threads)
    target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

set_target_properties (core_unit_tests PROPERTIES
    FOLDER tests
    OUTPUT_NAME core_unit_tests
)

if (EXTENSION_CODEC_ENABLED AND STATIC_CODEC_EXTENSION)
  target_link_libraries(${PROJECT_NAME} codec)
endif()

if(EXTENSION_KWS_ENABLED AND STATIC_KWS_EXTENSION)
  target_link_libraries(${PROJECT_NAME} kws_factory)
endif()

project(core_unit_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

set (HEADERS_PRIVATE
    "${SRC_DIR}/sequence_generator_audio_stream_reader.h"
    "${SRC_DIR}/unit_test_utils.h"
    "${SRC_DIR}/mocks.h"
    )

set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/single_to_many_stream_reader_adapter_test.cpp"
    "${SRC_DIR}/sequence_generator_audio_stream_reader.cpp"
    "${SRC_DIR}/read_write_ring_buffer_test_case.cpp"
    "${SRC_DIR}/string_utils_test.cpp"
    "${SRC_DIR}/audio_processor_write_to_audio_source_buffer_test.cpp"
    "${SRC_DIR}/audio_source_adapter_test.cpp"
    )

list(APPEND HEADERS_PRIVATE ${TEST_HEADERS})

set_source_groups()

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS_PRIVATE})

set(PRIVATE_AUDIO_HEADERS "${CARBON_CORE}/audio")
set(PRIVATE_DATA_HEADERS "${CARBON_CORE}/data")

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${TEST_INCLUDE}
        "${CARBON_CORE}/include"
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        "${CARBON_EXTERNAL}/json"
        "${CARBON_EXTERNAL}/Catch2"
        # This is a unit test project, so include internals
        ${PRIVATE_AUDIO_HEADERS}
        ${PRIVATE_DATA_HEADERS}
)

target_link_libraries(${PROJECT_NAME} usp data audio factory common)

if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
if (${use_openssl})
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099 /INCREMENTAL:NO")
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

add_compile_definitions(USE_NON_CORE_ROOT_SITE)

if (EXTENSION_CODEC_ENABLED AND STATIC_CODEC_EXTENSION)
  target_link_libraries(${PROJECT_NAME} codec)
endif()

if(EXTENSION_KWS_ENABLED AND STATIC_KWS_EXTENSION)
  target_link_libraries(${PROJECT_NAME} kws_factory)
endif()

if (EXTENSION_RNNT_ENABLED AND STATIC_EMBEDDEDSR_EXTENSION)
  target_link_libraries(${PROJECT_NAME} embedded_sr)
endif()

target_link_libraries(${PROJECT_NAME} ${SPEECHSDK_CORE_LIBRARY})

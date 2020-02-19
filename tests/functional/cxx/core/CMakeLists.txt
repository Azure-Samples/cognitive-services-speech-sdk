project(core_tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

include_directories("${CARBON_CORE}/include")
include_directories("${COMMON}/include")
include_directories("${USP}/include")
include_directories("${SR}")
include_directories("${TTS_REST}")
include_directories("${AUDIO}")
include_directories("${AUDIO}/include")
include_directories("${CARBON_EXTERNAL}/Catch2")
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories(${TEST_INCLUDE})
include_directories("${CARBON_EXTERNAL}/json")

set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/usp_test_case.cpp"
    "${SRC_DIR}/mic_test_case.cpp"
    "${SRC_DIR}/sr_test_case.cpp"
    "${SRC_DIR}/speech_context_json_test.cpp"
    )

if(EXTENSION_AUDIOCOMPRESSION_ENABLED)
  list(APPEND SOURCES
    "${SRC_DIR}/compression_codec_test.cpp"
  )
endif()

source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${TEST_HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES} ${TEST_HEADERS})

# define USE_NON_CORE_ROOT_SITE when building extensions or unit tests. This enable the extensions or unit test to use the extension root site provided by CSpxExtensionResourceManager.
# unit tests against modules other than common: they still need to be linked with factory since SpxGetCoreRootSite is defined in factory.
add_compile_definitions(USE_NON_CORE_ROOT_SITE)

target_link_libraries(${PROJECT_NAME} sr conversation_translation factory common audio usp ${TEST_LIBS})

if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
if (${use_openssl})
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
endif()
endif()

if (NOT WIN32)
    find_package (Threads)
    target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

set_target_properties (core_tests PROPERTIES
    FOLDER tests
    OUTPUT_NAME core_tests
)

if (EXTENSION_CODEC_ENABLED AND STATIC_CODEC_EXTENSION)
  target_link_libraries(${PROJECT_NAME} codec)
endif()

if(EXTENSION_KWS_ENABLED AND STATIC_KWS_EXTENSION)
  target_link_libraries(${PROJECT_NAME} kws_factory)
endif()
cmake_minimum_required(VERSION 3.20)
project(live_interpreter_cpp LANGUAGES CXX)

# Supply an extracted native SDK or an installed Microsoft.CognitiveServices.Speech
# NuGet package. This build never downloads an SDK.
set(SPEECHSDK_ROOT "$ENV{SPEECHSDK_ROOT}" CACHE PATH "Native Speech SDK or NuGet package root")
if(NOT SPEECHSDK_ROOT)
    if(DEFINED ENV{NUGET_PACKAGES})
        set(_nuget "$ENV{NUGET_PACKAGES}")
    elseif(WIN32)
        set(_nuget "$ENV{USERPROFILE}/.nuget/packages")
    else()
        set(_nuget "$ENV{HOME}/.nuget/packages")
    endif()
    set(SPEECHSDK_ROOT "${_nuget}/microsoft.cognitiveservices.speech/1.53.0")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$" OR CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
    set(_arch arm64)
else()
    set(_arch x64)
endif()
set(SPEECHSDK_ARCH "${_arch}" CACHE STRING "Native SDK architecture: x64 or arm64")
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8 OR NOT SPEECHSDK_ARCH MATCHES "^(x64|arm64)$")
    message(FATAL_ERROR "Use a 64-bit x64 or ARM64 toolchain and matching SPEECHSDK_ARCH.")
endif()

# Honor the current SDK root and architecture when reconfiguring an existing build.
foreach(_result IN ITEMS SPEECHSDK_CXX_INCLUDE SPEECHSDK_C_INCLUDE SPEECHSDK_LIBRARY SPEECHSDK_DLL)
    unset(${_result} CACHE)
endforeach()

find_path(SPEECHSDK_CXX_INCLUDE speechapi_cxx.h
    PATHS "${SPEECHSDK_ROOT}/include/cxx_api" "${SPEECHSDK_ROOT}/build/native/include/cxx_api"
    NO_DEFAULT_PATH REQUIRED)
find_path(SPEECHSDK_C_INCLUDE speechapi_c_common.h
    PATHS "${SPEECHSDK_ROOT}/include/c_api" "${SPEECHSDK_ROOT}/build/native/include/c_api"
    NO_DEFAULT_PATH REQUIRED)
if(WIN32)
    set(_platform win)
elseif(APPLE)
    set(_platform osx)
else()
    set(_platform linux)
endif()
set(_runtime "${SPEECHSDK_ROOT}/runtimes/${_platform}-${SPEECHSDK_ARCH}/native")
find_library(SPEECHSDK_LIBRARY NAMES Microsoft.CognitiveServices.Speech.core
    PATHS "${SPEECHSDK_ROOT}/lib/${SPEECHSDK_ARCH}"
          "${SPEECHSDK_ROOT}/build/native/${SPEECHSDK_ARCH}/Release"
          "${_runtime}"
    NO_DEFAULT_PATH REQUIRED)

find_package(Threads REQUIRED)
if(WIN32)
    find_file(SPEECHSDK_DLL Microsoft.CognitiveServices.Speech.core.dll
        PATHS "${_runtime}" "${SPEECHSDK_ROOT}/lib/${SPEECHSDK_ARCH}"
        NO_DEFAULT_PATH REQUIRED)
    get_filename_component(_dll_dir "${SPEECHSDK_DLL}" DIRECTORY)
    file(GLOB _runtime_dlls "${_dll_dir}/*.dll")
else()
    get_filename_component(_library_dir "${SPEECHSDK_LIBRARY}" DIRECTORY)
endif()

function(configure_speech_target target)
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_include_directories(${target} SYSTEM PRIVATE "${SPEECHSDK_CXX_INCLUDE}" "${SPEECHSDK_C_INCLUDE}")
    target_link_libraries(${target} PRIVATE "${SPEECHSDK_LIBRARY}" Threads::Threads)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /EHsc)
        target_compile_definitions(${target} PRIVATE NOMINMAX)
    endif()
    if(WIN32)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${_runtime_dlls} "$<TARGET_FILE_DIR:${target}>"
            COMMAND_EXPAND_LISTS)
    else()
        set_target_properties(${target} PROPERTIES BUILD_RPATH "${_library_dir}")
    endif()
endfunction()

add_executable(live_interpreter main.cpp)
configure_speech_target(live_interpreter)

include(CTest)
if(BUILD_TESTING)
    add_executable(helper_tests helper_tests.cpp)
    target_compile_features(helper_tests PRIVATE cxx_std_17)
    target_link_libraries(helper_tests PRIVATE Threads::Threads)
    add_test(NAME sample_helpers COMMAND helper_tests)
    set_tests_properties(sample_helpers PROPERTIES TIMEOUT 10)
    add_test(NAME completion_watchdog COMMAND helper_tests --watchdog)
    set_tests_properties(completion_watchdog PROPERTIES WILL_FAIL TRUE TIMEOUT 5)
    add_executable(config_tests config_tests.cpp)
    configure_speech_target(config_tests)
    add_test(NAME translation_config COMMAND config_tests)
    set_tests_properties(translation_config PROPERTIES TIMEOUT 10)
endif()

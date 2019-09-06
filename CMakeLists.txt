if(WIN32)
  # Version >= 3.10 is required for generating VS 2017 solutions and ensuring SWIG to create correct dll names.
  cmake_minimum_required(VERSION 3.10)
elseif(APPLE)
  cmake_minimum_required(VERSION 3.9)
else()
  cmake_minimum_required(VERSION 3.5)
endif()

# Only interpret ``if()`` arguments as variables or keywords when unquoted.
# cmake --help-policy CMP0054
cmake_policy(SET CMP0054 NEW)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(speechsdk_functions)

set(SPEECHSDK_BUILD_TYPE "local" CACHE STRING "SDK build type (local, dev, int, prod)")
get_local_version(_LOCAL_VERSION)
set(SPEECHSDK_VERSION "${_LOCAL_VERSION}" CACHE STRING "Version to use")
message(STATUS "Building Speech SDK version ${SPEECHSDK_VERSION}")

parse_version(
  BUILD_TYPE ${SPEECHSDK_BUILD_TYPE}
  VERSION ${SPEECHSDK_VERSION}
  PREFIX SPEECHSDK
  )

get_local_platform(_LOCAL_PLATFORM)
set(SPEECHSDK_TARGET_PLATFORM "${_LOCAL_PLATFORM}" CACHE STRING
  "SDK target platform ({Windows,WindowsUwp,Linux,OSX,Android,IOS,IOSSIMULATOR}-{x86,x64,arm32,arm64}; not all combinations are supported)")
message(STATUS "SPEECHSDK_TARGET_PLATFORM=${SPEECHSDK_TARGET_PLATFORM}")

parse_platform(TARGET_PLATFORM ${SPEECHSDK_TARGET_PLATFORM} PREFIX SPEECHSDK)

option(SPECTRE_MITIGATION "Build with Spectre mitigation" OFF)

# Make the compilation of KWS optional
option(EXTENSION_KWS_ENABLED "Build the KWS extension based on the VoiceActivation component" OFF)

# Make the compilation of gstreamer (for decoding audio format) optional
option(EXTENSION_CODEC_ENABLED "Build the CODEC extension based on the supported platform" OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

if(SPEECHSDK_OS STREQUAL "WindowsUwp")
  option(BUILD_PYTHON "Build Python SWIG module" OFF)
  option(BUILD_CSHARP "Build C# module" ON)
  option(BUILD_JAVA "Build Java/Android SWIG module" OFF)
  set(use_default_uuid ON CACHE BOOL "")
  add_compile_definitions("USING_DEFAULT_UUID")
  set(use_schannel OFF CACHE BOOL "")
  set(use_openssl ON CACHE BOOL "")
  if(SPEECHSDK_ARCH STREQUAL "x64")
    set(OPENSSL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/uwp_ssl/lib/x64" CACHE STRING "")
  elseif(SPEECHSDK_ARCH STREQUAL "x86")
    set(OPENSSL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/uwp_ssl/lib/Win32" CACHE STRING "")
  elseif(SPEECHSDK_ARCH STREQUAL "arm32")
    set(OPENSSL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/uwp_ssl/lib/arm" CACHE STRING "")
  elseif(SPEECHSDK_ARCH STREQUAL "arm64")
    set(OPENSSL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/uwp_ssl/lib/arm64" CACHE STRING "")
  else()
    # Should not reach based on earlier checks.
    message(FATAL_ERROR "internal error")
  endif()
  set(OPENSSL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/uwp_ssl/include" CACHE STRING "")
  set(OPENSSL_USE_STATIC_LIBS ON CACHE BOOL "")
  set(CMAKE_SYSTEM_NAME WindowsStore)
  set(CMAKE_SYSTEM_VERSION 10.0)
elseif(SPEECHSDK_OS MATCHES "^IOS")
  option(BUILD_PYTHON "Build Python SWIG module" OFF)
  option(BUILD_CSHARP "Build C# module" OFF)
  option(BUILD_JAVA "Build Java/Android SWIG module" OFF)
  # override setting from toolchain file to allow finding GStreamer
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
  set(use_builtin_httpapi ON CACHE BOOL "")
  set(use_openssl OFF CACHE BOOL "")
  set(ENABLE_VISIBILITY TRUE CACHE BOOL "Whether or not to hide symbols (-fvisibility=hidden)")
  set(ENABLE_ARC TRUE CACHE BOOL "Enable automatic reference counting (-fvisibility=hidden)")
  if(SPEECHSDK_OS STREQUAL "IOSSIMULATOR")
    set(ENABLE_BITCODE FALSE CACHE BOOL "Whether or not to enable bitcode")
    if(SPEECHSDK_ARCH STREQUAL "x64")
      set(PLATFORM SIMULATOR64)
    elseif(SPEECHSDK_ARCH STREQUAL "x86")
      set(PLATFORM SIMULATOR)
    else()
      # Should not reach based on earlier checks.
      message(FATAL_ERROR "internal error")
    endif()
  else()
    if(SPEECHSDK_ARCH STREQUAL "arm64")
      set(PLATFORM OS64)
    elseif(SPEECHSDK_ARCH STREQUAL "all")
      set(PLATFORM OS)
      set(ARCHS armv7 arm64)
    else()
      # Should not reach based on earlier checks.
      message(FATAL_ERROR "internal error")
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      set(ENABLE_BITCODE FALSE CACHE BOOL "Whether or not to enable bitcode")
      set(ENABLE_BITCODE_MARKER 1)
    else()
      set(ENABLE_BITCODE TRUE CACHE BOOL "Whether or not to enable bitcode")
    endif()
  endif()
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/external/ios-cmake/ios.toolchain.cmake")
  set(DEPLOYMENT_TARGET 9.2)
  if(EXTENSION_CODEC_ENABLED)
    add_compile_definitions("STATIC_CODEC_EXTENSION")
    set(STATIC_CODEC_EXTENSION TRUE)
  endif()
  set(MACOSX ON)
elseif(SPEECHSDK_OS STREQUAL "OSX")
  option(BUILD_PYTHON "Build Python SWIG module" OFF)
  option(BUILD_CSHARP "Build C# module" ON)
  option(BUILD_JAVA "Build Java/Android SWIG module" ON)
  set(use_builtin_httpapi ON CACHE BOOL "")
  set(use_openssl OFF CACHE BOOL "")
  set(MACOSX ON)
elseif(SPEECHSDK_OS STREQUAL "Android")
  option(BUILD_PYTHON "Build Python SWIG module" OFF)
  option(BUILD_CSHARP "Build C# module" OFF)
  option(BUILD_JAVA "Build Java/Android SWIG module" ON)
  set(use_default_uuid ON CACHE BOOL "")
  add_compile_definitions("ANDROID_UUID")
  set(CMAKE_SYSTEM_NAME Android)
  set(CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION clang)
  # Note: the API levels below are also matched in the Android OpenSSL build,
  # cf. ci/build-android-openssl.sh.
  if(SPEECHSDK_ARCH STREQUAL "arm32")
    set(CMAKE_ANDROID_ARCH_ABI "armeabi-v7a")
    set(CMAKE_SYSTEM_VERSION "19")
  elseif(SPEECHSDK_ARCH STREQUAL arm64)
    set(CMAKE_ANDROID_ARCH_ABI "arm64-v8a")
    set(CMAKE_SYSTEM_VERSION 21)
  elseif(SPEECHSDK_ARCH STREQUAL x86)
    set(CMAKE_ANDROID_ARCH_ABI "x86")
    set(CMAKE_SYSTEM_VERSION 19)
  elseif(SPEECHSDK_ARCH STREQUAL "x64")
    set(CMAKE_ANDROID_ARCH_ABI x86_64)
    set(CMAKE_SYSTEM_VERSION 21)
  else()
    # Should never reach this based on earlier checks.
    message(FATAL_ERROR "internal error")
  endif()
  set(CMAKE_ANDROID_STL_TYPE c++_static)
  set(OPENSSL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/android-openssl/${CMAKE_ANDROID_ARCH_ABI}/usr/local" CACHE STRING "")
  set(use_openssl ON CACHE BOOL "")
elseif(SPEECHSDK_OS STREQUAL "Windows")
  option(BUILD_PYTHON "Build Python SWIG module" OFF)
  option(BUILD_CSHARP "Build C# module" ON)
  option(BUILD_JAVA "Build Java/Android SWIG module" ON)
elseif(SPEECHSDK_OS STREQUAL "Linux")
  set(use_openssl ON CACHE BOOL "")
  set(use_openssl_dynamic ON CACHE BOOL "")
  option(BUILD_PYTHON "Build Python SWIG module" OFF)
  option(BUILD_JAVA "Build Java/Android SWIG module" ON)
  if(SPEECHSDK_ARCH STREQUAL "x64")
    option(BUILD_CSHARP "Build C# module" ON)
  else()
    option(BUILD_CSHARP "Build C# module" OFF)
  endif()
else()
  message(FATAL_ERROR "internal error")
endif()

message(STATUS "CMake version info: ${CMAKE_VERSION}")
message(STATUS "BUILD_PYTHON=${BUILD_PYTHON}")
message(STATUS "BUILD_CSHARP=${BUILD_CSHARP}")
message(STATUS "BUILD_JAVA=${BUILD_JAVA}")

set(SPEECHSDK_PYTHON_WHEEL_ARGS "" CACHE STRING "Extra arguments to be passed to the wheel command. Must be given as ;-separated list.")

# Configure CCache if available
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(carbon)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

if(use_openssl)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSPEECHSDK_USE_OPENSSL")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSPEECHSDK_USE_OPENSSL")
endif()

if(MSVC)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)

  # Force to always compile with W4
  # first remove all possible warning settings to avoid warning D9025
  string(REGEX REPLACE "/W[0-3]" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REGEX REPLACE "/W[0-3]" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
  # then add warning level 4 flags
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")
  # disable precompiled headers (for now) to allow build in IDE environment
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Y-")

  # The code below are changes for the version that is included in the Windows OS. In the OS we don't want to dynamically depend on
  # vcruntime*.dll so we create a version of the library that is statically linked agains the UCRT.
  if((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (SPEECHSDK_CRT STREQUAL "OS_UCRT"))
      # TODO: When we add support remove this from here and the csharp tests.
      set(DISABLE_KWS_TESTS ON)

      message("changing to hybrid static linked VCRuntime.")
      foreach(flag_var
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_DEBUG
      )
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
      endforeach()

      # Override linker to still dynamically link the ABI stable core c runtime DLLs, to reduce our DLL size.
      set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libucrt.lib /DEFAULTLIB:ucrt.lib")
      set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libucrtd.lib /DEFAULTLIB:ucrtd.lib")
      set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libucrt.lib /DEFAULTLIB:ucrt.lib")
      set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libucrtd.lib /DEFAULTLIB:ucrtd.lib")
      set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libucrt.lib /DEFAULTLIB:ucrt.lib")
      set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libucrtd.lib /DEFAULTLIB:ucrtd.lib")
  endif()

  # add control flow guard
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /guard:cf")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /guard:cf")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /guard:cf")

  # make sure we generate pdbs for release as well
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Zi")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

  # add flags for windows store app
  if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSPX_UWP")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /APPCONTAINER /DYNAMICBASE /NXCOMPAT")
  endif()

elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-long-long -Wpedantic")

  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC -fPIE

  # Include debug information in release builds
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g")
elseif(ANDROID)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-long-long -Wpedantic -Wno-gnu-zero-variadic-macro-arguments -Wno-attributes")

  # compilation flags required by security checks
  # stack protector adds checks to prevent buffer overflows on stack allocated arrays
  # _FORTIFY_SOURCE=2 replaces glibc functions that are otherwise unsafe with their safe replacements (e.g. strcpy vs. strncpy)
  # See https://linux.die.net/man/7/feature_test_macros
  # The stack protector setting enables protection against stack attacks using buffer overflows on stack allocated memory.
  # See http://lists.llvm.org/pipermail/cfe-dev/2017-April/053662.html
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2 -fstack-protector-strong")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=2 -fstack-protector-strong")

  # relro makes the relocation section read only after loading preventing attacks on the table
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -z nocopyreloc -z relro -z now") # -Wl,-z,relro
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -z nocopyreloc -z relro -z now") # -Wl,-z,relro

  set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC -fPIE

  # Include debug information in release builds
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # compilation flags required by security checks
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong -Wformat -Wformat-security -Werror")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-strong -Wformat -Wformat-security -Werror")

  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g")

  set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC -fPIE

  # visibility
  cmake_policy(SET CMP0063 NEW)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_C_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

  # option to only compile embedded bitcode markers
  if(DEFINED APPLE)
    if (DEFINED PLATFORM)
      if(ENABLE_BITCODE_MARKER)
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -fembed-bitcode-marker")
      endif()
    else()
      # enable ARC for Objective-C on macOS. On iOS, it is enabled by default.
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fobjc-arc")
    endif()
  endif()
endif()

set(CARBON_ROOT ${PROJECT_SOURCE_DIR})
set(CARBON_SOURCE "${CARBON_ROOT}/source")
set(CARBON_PUBLIC "${CARBON_SOURCE}/public")
set(CARBON_CORE "${CARBON_SOURCE}/core")
set(CARBON_BINDINGS "${CARBON_SOURCE}/bindings")
set(CARBON_EXTENSIONS "${CARBON_SOURCE}/extensions")
set(CARBON_EXTERNAL "${CARBON_ROOT}/external")

# include folders
set(CARBON_C_API "${CARBON_PUBLIC}/c_api")
set(CARBON_CXX_API "${CARBON_PUBLIC}/cxx_api")
set(CARBON_INCLUDE "${CARBON_CORE}/include")

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
  set(SPEECHSDK_CORE_LIBRARY "Microsoft.CognitiveServices.Speech.core")
elseif(APPLE)
  set(SPEECHSDK_CORE_LIBRARY "Microsoft.CognitiveServices.Speech.core")
  set(SPEECHSDK_CORE_FRAMEWORK "MicrosoftCognitiveServicesSpeech")
elseif((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (SPEECHSDK_CRT STREQUAL "OS_UCRT"))
  set(SPEECHSDK_CORE_LIBRARY "Microsoft.CognitiveServices.Speech.core.os")
else()
  set(SPEECHSDK_CORE_LIBRARY "Microsoft.CognitiveServices.Speech.core")
endif()

file(GLOB SPEECH_C_API_HEADERS "${CARBON_C_API}/*.h")
file(GLOB SPEECH_CXX_API_HEADERS "${CARBON_CXX_API}/*.h")

# individual components
set(COMMON "${CARBON_CORE}/common")
set(FACTORY "${CARBON_CORE}/factory")
set(SR "${CARBON_CORE}/sr")
set(TTS "${CARBON_CORE}/tts")
set(TTS_MOCK "${CARBON_CORE}/tts_mock")
set(TTS_REST "${CARBON_CORE}/tts_rest")
set(TTS_USP "${CARBON_CORE}/tts_usp")
set(LIBCARBON "${CARBON_CORE}/libcarbon")
set(C_API "${CARBON_CORE}/c_api")
set(AUDIO "${CARBON_CORE}/audio")
set(USP "${CARBON_CORE}/usp")
set(MOCK "${CARBON_CORE}/mock")

# Enable Unidec build with 'cmake -DEXTENSION_UNIDEC_ENABLED=true'
if(EXTENSION_UNIDEC_ENABLED AND
   ((SPEECHSDK_TARGET_PLATFORM STREQUAL Linux-arm64) OR
    (SPEECHSDK_TARGET_PLATFORM STREQUAL Linux-x64) OR
    (SPEECHSDK_TARGET_PLATFORM STREQUAL Windows-x64) OR
    (SPEECHSDK_TARGET_PLATFORM STREQUAL WindowsUwp-x64)))
  set(BUILD_UNIDEC ON)
  set(UNIDEC "${CARBON_EXTENSIONS}/unidec")
else()
  set(BUILD_UNIDEC OFF)
endif()

message(STATUS "BUILD_UNIDEC=${BUILD_UNIDEC}")

set(TESTS "${CARBON_ROOT}/tests")
set(JAVA_TESTS "${TESTS}/functional/java/src/test/java")

set(CARBONX "${TESTS}/functional/cxx/carbonx")

set(COMMON_GENERATED_INCLUDE ${CMAKE_BINARY_DIR}/common-generated-include)

add_subdirectory("${CARBON_EXTERNAL}")
# azure-c-shared resets this to ONLY, needs to be BOTH to find GStreamer for iOS
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)

include_directories(${AZURE_SHARED_UTIL_INCLUDE})

if(EXTENSION_CODEC_ENABLED)
  if(SPEECHSDK_OS STREQUAL "Android")
    set(GSTREAMER_PKG "${GST_ROOT}")
  endif()

  set(GSTREAMER_OS_LIST Android Linux IOS IOSSIMULATOR IOSSIMULATOR64)
  if(SPEECHSDK_OS IN_LIST GSTREAMER_OS_LIST)
    add_subdirectory(${CARBON_EXTENSIONS}/codec)
  endif()
endif()

add_subdirectory(${COMMON})
add_subdirectory(${FACTORY})
add_subdirectory(${AUDIO})
add_subdirectory(${USP})

add_subdirectory(${C_API})
add_subdirectory(${SR})
add_subdirectory(${TTS})
add_subdirectory(${TTS_MOCK})
add_subdirectory(${TTS_REST})
add_subdirectory(${TTS_USP})

add_subdirectory(${MOCK})

add_subdirectory(${LIBCARBON})

if(BUILD_UNIDEC)
  add_subdirectory(${UNIDEC})
endif()

if(EXTENSION_KWS_ENABLED)
  add_subdirectory("${CARBON_EXTENSIONS}/kws")
  add_definitions(-DSPEECHSDK_KWS_ENABLED)
endif()

add_subdirectory(${CARBON_BINDINGS})

add_subdirectory(${TESTS})
add_subdirectory(${CARBONX})

#Since windows might build debug or release version after running cmake, we copy to both just in case
file(COPY "${TESTS}/test.settings.json" DESTINATION "${CARBON_ROOT}/build/bin/Debug")
file(COPY "${TESTS}/input" DESTINATION "${CARBON_ROOT}/build/bin/Debug")
file(COPY "${TESTS}/test.settings.json" DESTINATION "${CARBON_ROOT}/build/bin/Release")
file(COPY "${TESTS}/input" DESTINATION "${CARBON_ROOT}/build/bin/Release")

# Dummy targets to make API header appear
# in the generated VS solution.
add_custom_target(c_headers
  SOURCES ${SPEECH_C_API_HEADERS})
set_target_properties(c_headers PROPERTIES FOLDER api)

add_custom_target(cxx_headers
  SOURCES ${SPEECH_CXX_API_HEADERS})
set_target_properties(cxx_headers PROPERTIES FOLDER api)


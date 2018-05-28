if(WIN32)
  # Version >= 3.7.x is required for generating VS 2017 solutions.
  cmake_minimum_required(VERSION 3.7)
else()
  cmake_minimum_required(VERSION 3.5)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)


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

# In Release mode, set Azure-c-shared xlogging level to the minimum.
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNO_LOGGING")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNO_LOGGING")

if(MSVC)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)

  # Force to always compile with W4
  # first remove all possible warning settings to avoid warning D9025
  string(REGEX REPLACE "/W[0-3]" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  # then add warning level 4 flags
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
  # disable precompiled headers (for now) to allow build in IDE environment
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Y-")

  # add control flow guard
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /guard:cf")

  # make sure we generate pdbs for release as well
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-long-long -Wpedantic -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
elseif(ANDROID)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-long-long -Wpedantic -fPIC -Wno-gnu-zero-variadic-macro-arguments")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

# Android build complains about 
# <built-in>: In function 'float abs(float)':
# <built-in>: warning: conflicts with previous declaration here [-Wattributes]
# Added -Wno-attributes to suppress it.
if(ANDROID)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-attributes")
endif()

set(CARBON_ROOT ${PROJECT_SOURCE_DIR})
set(CARBON_SOURCE "${CARBON_ROOT}/source")
set(CARBON_PUBLIC "${CARBON_SOURCE}/public")
set(CARBON_CORE "${CARBON_SOURCE}/core")
set(CARBON_BINDINGS "${CARBON_SOURCE}/bindings")
set(CARBON_EXTERNAL "${CARBON_ROOT}/external")

# include folders
set(CARBON_C_API "${CARBON_PUBLIC}/c_api")
set(CARBON_CXX_API "${CARBON_PUBLIC}/cxx_api")
set(CARBON_INCLUDE "${CARBON_CORE}/include")

set(SPEECHSDK_CORE_LIBRARY "Microsoft.CognitiveServices.Speech.core")

FILE(GLOB SPEECH_C_API_HEADERS "${CARBON_C_API}/*.h")
FILE(GLOB SPEECH_CXX_API_HEADERS "${CARBON_CXX_API}/*.h")

# individual components
set(COMMON "${CARBON_CORE}/common")
set(FACTORY "${CARBON_CORE}/factory")
set(SR "${CARBON_CORE}/sr")
set(LIBCARBON "${CARBON_CORE}/libcarbon")
set(C_API "${CARBON_CORE}/c_api")
set(AUDIO "${CARBON_CORE}/audio")
set(USP "${CARBON_CORE}/usp")
set(MOCK "${CARBON_CORE}/mock")
if(CMAKE_CL_64)
    set(UNIDEC "${CARBON_CORE}/unidec")
endif()

set(TESTS "${CARBON_ROOT}/tests")
set(JAVA_TESTS "${TESTS}/functional/java")

set(JAVA_SAMPLES "${CARBON_ROOT}/samples/java/quickstart")

set(CARBONX "${TESTS}/functional/cxx/carbonx")

add_subdirectory("${CARBON_EXTERNAL}")

include_directories(${AZURE_SHARED_UTIL_INCLUDE})

add_subdirectory(${COMMON})
add_subdirectory(${FACTORY})
add_subdirectory(${AUDIO})
add_subdirectory(${USP})


add_subdirectory(${C_API})
add_subdirectory(${SR})
add_subdirectory(${MOCK})
add_subdirectory(${LIBCARBON})

if(CMAKE_CL_64)
    add_subdirectory(${UNIDEC})
endif()

add_subdirectory(${CARBONX})

add_subdirectory(${CARBON_BINDINGS})

if(NOT ANDROID)
  add_subdirectory(${TESTS})
endif()

# @wolfgang: need JNI on linux
if(BUILD_JAVA)
  if(WIN32)
    add_subdirectory(${JAVA_SAMPLES})
    add_subdirectory(${JAVA_TESTS})
  endif()

  if(ANDROID)
    add_subdirectory(${JAVA_TESTS})
  endif()
endif()

# Dummy targets to make API header appear
# in the generated VS solution.
add_custom_target(c_headers
    SOURCES ${SPEECH_C_API_HEADERS})
set_target_properties(c_headers PROPERTIES FOLDER api)

add_custom_target(cxx_headers
    SOURCES ${SPEECH_CXX_API_HEADERS})
set_target_properties(cxx_headers PROPERTIES FOLDER api)

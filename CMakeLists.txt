cmake_minimum_required(VERSION 3.5)

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
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(carbon)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

if(MSVC)
  # Force to always compile with W4
  # first remove all possible warning settings to avoid warning D9025
  string(REGEX REPLACE "/W[0-3]" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  # then add warning level 4 flags
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
  # disable precompiled headers (for now) to allow build in IDE environment
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Y-")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-long-long -Wpedantic -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

set(CARBON_ROOT ${PROJECT_SOURCE_DIR})
set(CARBON_SOURCE "${CARBON_ROOT}/source")
set(CARBON_PUBLIC "${CARBON_SOURCE}/public")
set(CARBON_CORE "${CARBON_SOURCE}/core")
set(CARBON_EXTERNAL "${CARBON_ROOT}/external")

# include folders
set(CARBON_C_API "${CARBON_PUBLIC}/c_api")
set(CARBON_CXX_API "${CARBON_PUBLIC}/cxx_api")
set(CARBON_INCLUDE "${CARBON_CORE}/include")

set(SPEECH_C_API_HEADERS
    "${CARBON_C_API}/speechapi_c.h"
    "${CARBON_C_API}/speechapi_c_common.h"
    "${CARBON_C_API}/speechapi_c_recognizer.h"
    "${CARBON_C_API}/speechapi_c_recognizer_factory.h"
    "${CARBON_C_API}/speechapi_c_result.h"
    "${CARBON_C_API}/speechapi_c_session.h"
    "${CARBON_C_API}/spxdebug.h"
    "${CARBON_C_API}/spxerror.h"
    )

set(SPEECH_CXX_API_HEADERS
    "${CARBON_CXX_API}/speechapi_cxx.h"
    "${CARBON_CXX_API}/speechapi_cxx_common.h"
    "${CARBON_CXX_API}/speechapi_cxx_eventargs.h"
    "${CARBON_CXX_API}/speechapi_cxx_eventsignal.h"
    "${CARBON_CXX_API}/speechapi_cxx_parameter.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognition_async_recognizer.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognition_base_async_recognizer.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognition_eventargs.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognition_result.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognizer_parameter_collection.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognizer.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognizer_factory.h"
    "${CARBON_CXX_API}/speechapi_cxx_session_parameter_collection.h"
    "${CARBON_CXX_API}/speechapi_cxx_session.h"
    "${CARBON_CXX_API}/speechapi_cxx_session_eventargs.h"
    "${CARBON_CXX_API}/speechapi_cxx_speech_recognition_eventargs.h"
    "${CARBON_CXX_API}/speechapi_cxx_speech_recognition_result.h"
    "${CARBON_CXX_API}/speechapi_cxx_speech_recognizer.h"
    "${CARBON_CXX_API}/speechapi_cxx_translation_eventargs.h"
    "${CARBON_CXX_API}/speechapi_cxx_translation_result.h"
    "${CARBON_CXX_API}/speechapi_cxx_translation_recognizer.h"
    "${CARBON_CXX_API}/speechapi_cxx_todo.h"
    "${CARBON_CXX_API}/speechapi_cxx_todo_intent.h"
    "${CARBON_CXX_API}/speechapi_cxx_todo_recognition.h"
    )


# individual components
set(COMMON "${CARBON_CORE}/common")
set(FACTORY "${CARBON_CORE}/factory")
set(SR "${CARBON_CORE}/sr")
set(CARBON_LIB "${CARBON_CORE}/carbon_lib")
set(C_API "${CARBON_CORE}/c_api")
set(AUDIO "${CARBON_CORE}/audio")
set(USP "${CARBON_CORE}/usp")
if(CMAKE_CL_64)
    set(UNIDEC "${CARBON_CORE}/unidec")
endif()

set(TESTS "${CARBON_ROOT}/tests")

set(USP_CONSOLE "${CARBON_ROOT}/tools/uspclientconsole")
set(CARBONX "${CARBON_ROOT}/tools/carbonx")
set(CARBON_BINDINGS "${CARBON_ROOT}/bindings")
set(CARBON_CSHARP_CONSOLE "${CARBON_ROOT}/tools/carbon_csharp_console")

add_subdirectory("${CARBON_EXTERNAL}")

include_directories(${AZURE_SHARED_UTIL_INCLUDE})

add_subdirectory(${COMMON})
add_subdirectory(${FACTORY})
add_subdirectory(${AUDIO})
add_subdirectory(${USP})
add_subdirectory(${USP_CONSOLE})
add_subdirectory(${TESTS})
add_subdirectory(${C_API})
add_subdirectory(${SR})
add_subdirectory(${CARBON_LIB})
if(CMAKE_CL_64)
    add_subdirectory(${UNIDEC})
endif()
add_subdirectory(${CARBONX})

add_subdirectory(${CARBON_BINDINGS})
add_subdirectory(${CARBON_CSHARP_CONSOLE})

# Dummy targets to make API header appear
# in the generated VS solution.
add_custom_target(c_header
    SOURCES ${SPEECH_C_API_HEADERS})
set_target_properties(c_header PROPERTIES FOLDER api)

add_custom_target(cxx_headers
    SOURCES ${SPEECH_CXX_API_HEADERS})
set_target_properties(cxx_headers PROPERTIES FOLDER api)
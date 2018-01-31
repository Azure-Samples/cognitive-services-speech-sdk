cmake_minimum_required(VERSION 3.5)

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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-long-long -Wpedantic")
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
    "${CARBON_C_API}/spxdebug.h"
    "${CARBON_C_API}/spxerror.h"
    )

set(SPEECH_CXX_API_HEADERS
    "${CARBON_CXX_API}/speechapi_cxx.h"
    "${CARBON_CXX_API}/speechapi_cxx_common.h"
    "${CARBON_CXX_API}/speechapi_cxx_eventargs.h"
    "${CARBON_CXX_API}/speechapi_cxx_eventsignal.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognition_async_recognizer.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognition_base_async_recognizer.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognition_eventargs.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognition_result.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognizer.h"
    "${CARBON_CXX_API}/speechapi_cxx_recognizer_factory.h"
    "${CARBON_CXX_API}/speechapi_cxx_session_eventargs.h"
    "${CARBON_CXX_API}/speechapi_cxx_speech_recognition_eventargs.h"
    "${CARBON_CXX_API}/speechapi_cxx_speech_recognition_result.h"
    "${CARBON_CXX_API}/speechapi_cxx_speech_recognizer.h"
    "${CARBON_CXX_API}/speechapi_cxx_todo.h"
    "${CARBON_CXX_API}/speechapi_cxx_todo_intent.h"
    "${CARBON_CXX_API}/speechapi_cxx_todo_recognition.h"
    "${CARBON_CXX_API}/speechapi_cxx_todo_session.h"
    )


# individual components
set(COMMON "${CARBON_CORE}/common")
set(SR "${CARBON_CORE}/sr")
set(CARBON_DLL "${CARBON_CORE}/carbon_dll")
set(C_API "${CARBON_CORE}/c_api")
set(AUDIO "${CARBON_CORE}/audio")
set(USP "${CARBON_CORE}/usp")

set(TESTS "${CARBON_ROOT}/tests")

set(USP_CONSOLE "${CARBON_ROOT}/tools/uspclientconsole")
set(CARBONX "${CARBON_ROOT}/tools/carbonx")

add_subdirectory ("${CARBON_EXTERNAL}/azure-c-shared-utility" azure_c_shared_utility EXCLUDE_FROM_ALL)
set_target_properties (aziotsharedutil PROPERTIES FOLDER external_libs)
if (UNIX)
    set_target_properties (aziotsharedutil PROPERTIES COMPILE_FLAGS "-Wno-error=unused-variable")
endif()
set(AZURE_SHARED_LIB aziotsharedutil)
include_directories (AZURE_SHARED_UTIL_INCLUDE ${SHARED_UTIL_INC_FOLDER})

if(WIN32)
    list(APPEND AZURE_SHARED_LIB rpcrt4.lib)
endif()

add_subdirectory(${COMMON})
add_subdirectory(${AUDIO})
add_subdirectory(${USP})
add_subdirectory(${USP_CONSOLE})
add_subdirectory(${TESTS})
add_subdirectory(${C_API})
add_subdirectory(${SR})
add_subdirectory(${CARBON_DLL})
add_subdirectory(${CARBONX})

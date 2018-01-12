#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

if (SHARED_UTILS_NO_LOGGING)
    SET(LWS_WITH_NO_LOGS ON CACHE BOOL "disable libWebSockets logging option" FORCE)
endif()

set(LIBWEBSOCKETS_DIR "${CARBON_EXTERNAL}/libwebsockets")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR ANDROID)
    add_subdirectory(${LIBWEBSOCKETS_DIR} libwebsockets EXCLUDE_FROM_ALL)
    include_directories (${LIBWEBSOCKETS_INCLUDE_DIRS})
endif()

if (DEFINED AZURE_C_SHARED_UTILITY_BUILT)
    RETURN()
else()
    SET(AZURE_C_SHARED_UTILITY_BUILT ON)
endif()

project(azure_c_shared_utility)

option(run_valgrind "set run_valgrind to ON if tests are to be run under valgrind/helgrind/drd. Default is OFF" OFF)

#making a nice global variable to know if we are on linux or not.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
endif()

include (CTest)

#these are the include folders
#the following "set" statetement exports across the project a global variable called COMMON_INC_FOLDER that expands to whatever needs to included when using COMMON library
set(SHARED_UTIL_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using sharedLib lib" FORCE)
set(SHARED_UTIL_SRC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/src CACHE INTERNAL "this is what needs to be included when doing include sources" FORCE)
set(SHARED_UTIL_ADAPTER_FOLDER "${CMAKE_CURRENT_LIST_DIR}/adapters" CACHE INTERNAL "this is where the adapters live" FORCE)
include_directories(${SHARED_UTIL_INC_FOLDER})

#the following variables are project-wide and can be used with cmake-gui
option(skip_unittests "set skip_unittests to ON to skip unittests (default is OFF)[if possible, they are always build]" ON)
option(use_http "set use_http to ON if http is to be used, set to OFF to not use http" ON)
option(use_condition "set use_condition to ON if the condition module and its adapters should be enabled" ON)

if(WIN32)
    option(use_schannel "set use_schannel to ON if schannel is to be used, set to OFF to not use schannel" ON)
    option(use_openssl "set use_openssl to ON if openssl is to be used, set to OFF to not use openssl" OFF)
    option(use_wolfssl "set use_wolfssl to ON if wolfssl is to be used, set to OFF to not use wolfssl" OFF)
else()
    option(use_schannel "set use_schannel to ON if schannel is to be used, set to OFF to not use schannel" OFF)
    option(use_openssl "set use_openssl to ON if openssl is to be used, set to OFF to not use openssl" OFF)
    option(use_wolfssl "set use_wolfssl to ON if wolfssl is to be used, set to OFF to not use wolfssl" OFF)
endif()
option(use_socketio "set use_socketio to ON if socketio is to be included in the library, set to OFF if a different implementation will be provided" OFF)

option(compileOption_C "passes a string to the command line of the C compiler" OFF)
option(compileOption_CXX "passes a string to the command line of the C++ compiler" OFF)
option(use_audio "set the use_audio to ON if you would like to use the audio stack" ON)
option(azureiot_debug "set the azureiot_debug to ON if you would like to have verbosity in your logs" OFF)

# Start of variables used during install
set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/usr/include/azure_c_shared_utility" CACHE PATH "Include file directory")
set (LIB_INSTALL_DIR lib CACHE PATH "Library object file directory")

#Use solution folders. 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message(STATUS "use_openssl:    ${use_openssl}")
message(STATUS "use_wolfssl:    ${use_wolfssl}")
message(STATUS "use_socketio:   ${use_socketio}")
message(STATUS "use_http:       ${use_http}")
message(STATUS "azureiot_debug: ${azureiot_debug}")
message(STATUS "use_gballoc:    ${use_gballoc}")

enable_testing()
include(CheckSymbolExists)
function(detect_architecture symbol arch) 
    if (NOT DEFINED ARCHITECTURE OR ARCHITECTURE STREQUAL "")
        set(CMAKE_REQUIRED_QUIET 1)
        check_symbol_exists("${symbol}" "" ARCHITECTURE_${arch}) 
        unset(CMAKE_REQUIRED_QUIET) 
        # The output variable needs to be unique across invocations otherwise 
        # CMake's crazy scope rules will keep it defined 
        if (ARCHITECTURE_${arch}) 
            set(ARCHITECTURE "${arch}" PARENT_SCOPE) 
            set(ARCHITECTURE_${arch} 1 PARENT_SCOPE) 
            add_definitions(-DARCHITECTURE_${arch}=1) 
        endif()
    endif()
endfunction()
if (MSVC)
    detect_architecture("_M_AMD64" x86_64)
    detect_architecture("_M_IX86" x86)
    detect_architecture("_M_ARM" ARM)
else() 
    detect_architecture("__x86_64__" x86_64)
    detect_architecture("__i386__" x86)
    detect_architecture("__arm__" ARM)
endif() 
if (NOT DEFINED ARCHITECTURE OR ARCHITECTURE STREQUAL "") 
    set(ARCHITECTURE "GENERIC") 
endif() 
message(STATUS "Shared_Util architecture: ${ARCHITECTURE}")

#if any compiler has a command line switch called "OFF" then it will need special care
if(NOT "${compileOption_C}" STREQUAL "OFF")
    set(CMAKE_C_FLAGS "${compileOption_C} ${CMAKE_C_FLAGS}")
endif()

if(NOT "${compileOption_CXX}" STREQUAL "OFF")
    set(CMAKE_CXX_FLAGS "${compileOption_CXX} ${CMAKE_CXX_FLAGS}")
endif()

#this project uses several other projects that are build not by these CMakeFiles
#this project also targets several OSes

#this function takes care of three things:
#1. copying some shared libraries(.dll or .so) to the location of the output executable

macro(compileAsC99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")
      set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
    endif()
  else()
    set (CMAKE_C_STANDARD 99)
    set (CMAKE_CXX_STANDARD 11)
  endif()
endmacro(compileAsC99)

macro(compileAsC11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c11 ${CMAKE_C_FLAGS}")
      set (CMAKE_C_FLAGS "-D_POSIX_C_SOURCE=200112L ${CMAKE_C_FLAGS}")
      set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
    endif()
  else()
    set (CMAKE_C_STANDARD 11)
    set (CMAKE_CXX_STANDARD 11)
  endif()
  if(UNIX)
    set (_CFLAGS "-fPIC -Wall -Wno-unknown-pragmas -fvisibility=hidden")

    # openwrt raises warning about wrong path for poll.h
    string(FIND ${CMAKE_C_COMPILER} "openwrt" _FINDPOS)

    if ("${_FINDPOS}" STREQUAL "-1")
        set (_CFLAGS "${_CFLAGS} -Werror")
    endif()

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_CFLAGS}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_CFLAGS}")
  endif()

  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-visibility")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-visibility")
  endif()
endmacro(compileAsC11)

if (SHARED_UTILS_NO_LOGGING)
	set(CMAKE_C_FLAGS "-DNO_LOGGING ${CMAKE_C_FLAGS}")
endif()

if (azureiot_debug)
else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNO_ENUM_STRINGS")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_ENUM_STRINGS")
endif()

if(${use_gballoc})
    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC -DGB_MEASURE_MEMORY_FOR_THIS)
else()    
endif()

if (SHARED_UTILS_NO_LOGGING)
else()
	set(LOGGER_C_FILES 
		${sharedutil_include_directories}../../src/xlogging.c 
		${sharedutil_include_directories}../../src/consolelogger.c
		)
	set(LOGGER_SRC_FILES 
		./src/xlogging.c 
		./src/consolelogger.c
		)
endif()

function(add_files_to_install filesToBeInstalled)
    set(INSTALL_H_FILES ${INSTALL_H_FILES} ${filesToBeInstalled} CACHE INTERNAL "Files that will be installed on the system")
endfunction()

function(windows_unittests_add_dll whatIsBuilding)
    link_directories(${whatIsBuilding}_dll $ENV{VCInstallDir}UnitTest/lib)
    
    add_library(${whatIsBuilding}_dll SHARED 
        ${${whatIsBuilding}_cpp_files} 
        ${${whatIsBuilding}_h_files} 
        ${${whatIsBuilding}_c_files}
        ${LOGGER_C_FILES}
    )

    set_target_properties(${whatIsBuilding}_dll
               PROPERTIES
               FOLDER "UnitTests") 
    
    target_include_directories(${whatIsBuilding}_dll PUBLIC ${sharedutil_include_directories} $ENV{VCInstallDir}UnitTest/include)
    target_compile_definitions(${whatIsBuilding}_dll PUBLIC -DCPP_UNITTEST)
    target_link_libraries(${whatIsBuilding}_dll micromock_cpp_unittest umock_c ctest testrunnerswitcher ${ARGN})
endfunction()

function(windows_unittests_add_exe whatIsBuilding)
    add_executable(${whatIsBuilding}_exe
        ${${whatIsBuilding}_cpp_files} 
        ${${whatIsBuilding}_h_files} 
        ${${whatIsBuilding}_c_files}
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${LOGGER_C_FILES}
    )
    set_target_properties(${whatIsBuilding}_exe
               PROPERTIES
               FOLDER "UnitTests")
                
    target_compile_definitions(${whatIsBuilding}_exe PUBLIC -DUSE_CTEST)
    target_include_directories(${whatIsBuilding}_exe PUBLIC ${sharedutil_include_directories})
    target_link_libraries(${whatIsBuilding}_exe micromock_ctest umock_c ctest testrunnerswitcher ${ARGN})
    add_test(NAME ${whatIsBuilding} COMMAND ${whatIsBuilding}_exe)
endfunction()

#this function takes more than the 1 mandatory argument (whatIsBuilding)
#the parameters are separated by "known" separators
#for example, ADDITIONAL_LIBS starts a list of needed libraries
function(linux_unittests_add_exe whatIsBuilding)
    add_executable(${whatIsBuilding}_exe
        ${${whatIsBuilding}_cpp_files} 
        ${${whatIsBuilding}_h_files} 
        ${${whatIsBuilding}_c_files}
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${LOGGER_C_FILES}
    )
    set_target_properties(${whatIsBuilding}_exe
               PROPERTIES
               FOLDER "UnitTests")
                
    target_compile_definitions(${whatIsBuilding}_exe PUBLIC -DUSE_CTEST)
    target_include_directories(${whatIsBuilding}_exe PUBLIC ${sharedutil_include_directories})
    
    #this part detects 
        #       - the additional libraries that might be needed.
    #     additional libraries are started by ADDITIONAL_LIBS parameter and ended by any other known parameter (or end of variable arguments)
    #   - a valgring suppression file (VALGRIND_SUPRESSIONS_FILE) for memcheck
    #     the file name follows immediately after 
    
    set(PARSING_ADDITIONAL_LIBS OFF)
    set(PARSING_VALGRIND_SUPRESSIONS_FILE OFF)
        set(VALGRIND_SUPPRESSIONS_FILE_EXTRA_PARAMETER)
    foreach(f ${ARGN})
        if(${f} STREQUAL "ADDITIONAL_LIBS")
            SET(PARSING_ADDITIONAL_LIBS ON)
            SET(PARSING_VALGRIND_SUPRESSION_FILE OFF)
            #also unset all the other states
            
        elseif(${f} STREQUAL "VALGRIND_SUPPRESSIONS_FILE")
            SET(PARSING_ADDITIONAL_LIBS OFF)
            SET(PARSING_VALGRIND_SUPPRESSION_FILE ON)
            
        else()
        
            if(PARSING_ADDITIONAL_LIBS)
                target_link_libraries(${whatIsBuilding}_exe ${f})
            elseif(PARSING_VALGRIND_SUPPRESSION_FILE)
                set(VALGRIND_SUPPRESSIONS_FILE_EXTRA_PARAMETER "--suppressions=${f}")
            endif()
        endif()	
        
    endforeach()
    
    target_link_libraries(${whatIsBuilding}_exe micromock_ctest umock_c ctest)
    
    add_test(NAME ${whatIsBuilding} COMMAND $<TARGET_FILE:${whatIsBuilding}_exe>)
    
    if(${run_valgrind})
        find_program(VALGRIND_FOUND NAMES valgrind)
        if(${VALGRIND_FOUND} STREQUAL VALGRIND_FOUND-NOTFOUND)
            message(WARNING "run_valgrind was TRUE, but valgrind was not found - there will be no tests run under valgrind")
        else()
            add_test(NAME ${whatIsBuilding}_valgrind COMMAND valgrind                 --num-callers=100 --error-exitcode=1 --trace-children=yes --leak-check=full --track-origins=yes ${VALGRIND_SUPPRESSIONS_FILE_EXTRA_PARAMETER} $<TARGET_FILE:${whatIsBuilding}_exe>)
            add_test(NAME ${whatIsBuilding}_helgrind COMMAND valgrind --tool=helgrind --num-callers=100 --error-exitcode=1 $<TARGET_FILE:${whatIsBuilding}_exe>)
            add_test(NAME ${whatIsBuilding}_drd      COMMAND valgrind --tool=drd      --num-callers=100 --error-exitcode=1 $<TARGET_FILE:${whatIsBuilding}_exe>)
        endif()
    endif()
endfunction()

function(build_test_artifacts whatIsBuilding use_gballoc)
    
    #the first argument is what is building
    #the second argument is whether the tests should be build with gballoc #defines or not
    #the following arguments are a list of libraries to link with
    
    if(${use_gballoc})
        add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
    else()    
    endif()
    
    #setting #defines
    if(WIN32)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    else()
    endif()
    
    #setting includes
    set(sharedutil_include_directories ${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} ${SAL_INC_FOLDER} ${SHARED_UTIL_INC_FOLDER} ${SHARED_UTIL_SRC_FOLDER})
    if(WIN32)
    else()
        include_directories(${sharedutil_include_directories})
    endif()

    #setting output type
    if(WIN32)
        if(
            (("${whatIsBuilding}" MATCHES ".*unittests.*") AND ${skip_unittests})
        )
        else()
            windows_unittests_add_exe(${whatIsBuilding} ${ARGN})
            windows_unittests_add_dll(${whatIsBuilding} ${ARGN})
        endif()
    else()
        if(
            (("${whatIsBuilding}" MATCHES ".*unittests.*") AND ${skip_unittests})
        )
        else()
            linux_unittests_add_exe(${whatIsBuilding} ${ARGN})
        endif()
    endif()
endfunction(build_test_artifacts)


function(c_windows_unittests_add_dll whatIsBuilding folder)
    link_directories(${whatIsBuilding}_dll $ENV{VCInstallDir}UnitTest/lib)

    SET(VAR 1)
    foreach(file ${${whatIsBuilding}_test_files})
    add_library(${whatIsBuilding}_testsonly_lib STATIC 
            ${file}
    )
        set_source_files_properties(${file} PROPERTIES COMPILE_FLAGS -DCPPUNITTEST_SYMBOL=some_symbol_for_cppunittest_${VAR})
        MATH(EXPR VAR "${VAR}+1")
    endforeach()

    set_target_properties(${whatIsBuilding}_testsonly_lib
               PROPERTIES
               FOLDER ${folder} ) 

    target_include_directories(${whatIsBuilding}_testsonly_lib PUBLIC ${sharedutil_include_directories} $ENV{VCInstallDir}UnitTest/include)
    target_compile_definitions(${whatIsBuilding}_testsonly_lib PUBLIC -DCPP_UNITTEST)
    target_compile_options(${whatIsBuilding}_testsonly_lib PUBLIC /TP /EHsc)
  
    add_library(${whatIsBuilding}_dll SHARED 
        ${${whatIsBuilding}_cpp_files} 
        ${${whatIsBuilding}_h_files} 
        ${${whatIsBuilding}_c_files}
        ${LOGGER_C_FILES}
    )

    set_target_properties(${whatIsBuilding}_dll
               PROPERTIES
               FOLDER ${folder} ) 

    target_include_directories(${whatIsBuilding}_dll PUBLIC ${sharedutil_include_directories} $ENV{VCInstallDir}UnitTest/include)
    target_compile_definitions(${whatIsBuilding}_dll PUBLIC -DCPP_UNITTEST)
    target_link_libraries(${whatIsBuilding}_dll micromock_cpp_unittest umock_c ctest testrunnerswitcher ${whatIsBuilding}_testsonly_lib ${ARGN})
    SET(VAR 1)
    foreach(file ${${whatIsBuilding}_test_files})
        # for x64 the underscore is not needed
        if (ARCHITECTURE STREQUAL "x86_64")
            set_property(TARGET ${whatIsBuilding}_dll APPEND PROPERTY LINK_FLAGS /INCLUDE:"some_symbol_for_cppunittest_${VAR}")
        else()
            set_property(TARGET ${whatIsBuilding}_dll APPEND PROPERTY LINK_FLAGS /INCLUDE:"_some_symbol_for_cppunittest_${VAR}")
        endif()
        MATH(EXPR VAR "${VAR}+1")
    endforeach()
endfunction()

function(c_windows_unittests_add_exe whatIsBuilding folder)
    add_executable(${whatIsBuilding}_exe
        ${${whatIsBuilding}_test_files} 
        ${${whatIsBuilding}_cpp_files} 
        ${${whatIsBuilding}_h_files} 
        ${${whatIsBuilding}_c_files}
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${LOGGER_C_FILES}
    )
    set_target_properties(${whatIsBuilding}_exe
               PROPERTIES
               FOLDER ${folder})
                
    target_compile_definitions(${whatIsBuilding}_exe PUBLIC -DUSE_CTEST)
    target_include_directories(${whatIsBuilding}_exe PUBLIC ${sharedutil_include_directories})
    target_link_libraries(${whatIsBuilding}_exe micromock_ctest umock_c ctest testrunnerswitcher ${ARGN})
    add_test(NAME ${whatIsBuilding} COMMAND ${whatIsBuilding}_exe)
endfunction()

#this function takes more than the 1 mandatory argument (whatIsBuilding)
#the parameters are separated by "known" separators
#for example, ADDITIONAL_LIBS starts a list of needed libraries
function(c_linux_unittests_add_exe whatIsBuilding folder)

    add_executable(${whatIsBuilding}_exe
        ${${whatIsBuilding}_test_files} 
        ${${whatIsBuilding}_cpp_files} 
        ${${whatIsBuilding}_h_files} 
        ${${whatIsBuilding}_c_files}
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${LOGGER_C_FILES}
    )
    set_target_properties(${whatIsBuilding}_exe
               PROPERTIES
               FOLDER ${folder})

    target_compile_definitions(${whatIsBuilding}_exe PUBLIC -DUSE_CTEST)
    target_include_directories(${whatIsBuilding}_exe PUBLIC ${sharedutil_include_directories})
    
    #this part detects 
        #       - the additional libraries that might be needed.
    #     additional libraries are started by ADDITIONAL_LIBS parameter and ended by any other known parameter (or end of variable arguments)
    #   - a valgring suppression file (VALGRIND_SUPRESSIONS_FILE) for memcheck
    #     the file name follows immediately after 
    
    set(PARSING_ADDITIONAL_LIBS OFF)
    set(PARSING_VALGRIND_SUPRESSIONS_FILE OFF)
        set(VALGRIND_SUPPRESSIONS_FILE_EXTRA_PARAMETER)
    foreach(f ${ARGN})
        if(${f} STREQUAL "ADDITIONAL_LIBS")
            SET(PARSING_ADDITIONAL_LIBS ON)
            SET(PARSING_VALGRIND_SUPRESSION_FILE OFF)
            #also unset all the other states
            continue()
                elseif(${f} STREQUAL "VALGRIND_SUPPRESSIONS_FILE")
            SET(PARSING_ADDITIONAL_LIBS OFF)
                        SET(PARSING_VALGRIND_SUPPRESSION_FILE ON)
            continue()
        endif()
        
        if(PARSING_ADDITIONAL_LIBS)
            target_link_libraries(${whatIsBuilding}_exe ${f})
                elseif(PARSING_VALGRIND_SUPPRESSION_FILE)
                        set(VALGRIND_SUPPRESSIONS_FILE_EXTRA_PARAMETER "--suppressions=${f}")
        endif()	
        
    endforeach()
    
    target_link_libraries(${whatIsBuilding}_exe micromock_ctest umock_c ctest)
    
    add_test(NAME ${whatIsBuilding} COMMAND $<TARGET_FILE:${whatIsBuilding}_exe>)
    
    if(${run_valgrind})
        find_program(VALGRIND_FOUND NAMES valgrind)
        if(${VALGRIND_FOUND} STREQUAL VALGRIND_FOUND-NOTFOUND)
            message(WARNING "run_valgrind was TRUE, but valgrind was not found - there will be no tests run under valgrind")
        else()
            add_test(NAME ${whatIsBuilding}_valgrind COMMAND valgrind                 --num-callers=100 --error-exitcode=1 --trace-children=yes --leak-check=full --track-origins=yes ${VALGRIND_SUPPRESSIONS_FILE_EXTRA_PARAMETER} $<TARGET_FILE:${whatIsBuilding}_exe>)
            add_test(NAME ${whatIsBuilding}_helgrind COMMAND valgrind --tool=helgrind --num-callers=100 --error-exitcode=1 $<TARGET_FILE:${whatIsBuilding}_exe>)
            add_test(NAME ${whatIsBuilding}_drd      COMMAND valgrind --tool=drd      --num-callers=100 --error-exitcode=1 $<TARGET_FILE:${whatIsBuilding}_exe>)
        endif()
    endif()
endfunction()

function(build_c_test_artifacts whatIsBuilding use_gballoc folder)
    
    #the first argument is what is building
    #the second argument is whether the tests should be build with gballoc #defines or not
    #the following arguments are a list of libraries to link with
    
    if(${use_gballoc})
        add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
    else()    
    endif()
    
    #setting #defines
    if(WIN32)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    else()
    endif()
    
    #setting includes
    set(sharedutil_include_directories ${MICROMOCK_INC_FOLDER} ${UMOCK_C_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} ${SAL_INC_FOLDER} ${SHARED_UTIL_INC_FOLDER} ${SHARED_UTIL_SRC_FOLDER})
    if(WIN32)
    else()
        include_directories(${sharedutil_include_directories})
    endif()

    #setting output type
    if(WIN32)
        if(
            (("${whatIsBuilding}" MATCHES ".*unittests.*") AND ${skip_unittests})
        )
        else()
            c_windows_unittests_add_dll(${whatIsBuilding} ${folder} ${ARGN})
            c_windows_unittests_add_exe(${whatIsBuilding} ${folder} ${ARGN})
        endif()
    else()
        if(
            (("${whatIsBuilding}" MATCHES ".*unittests.*") AND ${skip_unittests})
        )
        else()
            c_linux_unittests_add_exe(${whatIsBuilding} ${folder} ${ARGN})
        endif()
    endif()
endfunction()

function(set_platform_files)
    if(WIN32)
        if(${use_condition})
            set(CONDITION_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/condition_win32.c PARENT_SCOPE)
        endif()
        set(THREAD_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/threadapi_win32.c PARENT_SCOPE)
        set(LOCK_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/lock_win32.c PARENT_SCOPE)
        if (WINCE)
            set(HTTP_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/httpapi_wince.cpp PARENT_SCOPE)
        else()
            set(HTTP_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/httpapi_winhttp.c PARENT_SCOPE)
        endif()
        set(PLATFORM_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/platform_win32.c PARENT_SCOPE)
        if (${use_socketio})
        set(SOCKETIO_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/socketio_win32.c PARENT_SCOPE)
        endif()
        set(TICKCOUTER_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/tickcounter_win32.c PARENT_SCOPE)
        set(UNIQUEID_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/uniqueid_win32.c PARENT_SCOPE)
        if(${use_audio})
            set(AUDIO_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/audio_sys_win32.cpp PARENT_SCOPE)
        endif()
    else()
        if(${use_condition})
            set(CONDITION_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/condition_pthreads.c PARENT_SCOPE)
        endif()
        set(LOCK_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/lock_pthreads.c PARENT_SCOPE)
        if (${use_openssl} OR ${use_wolfssl})
	        set(PLATFORM_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/platform_linux.c PARENT_SCOPE)
		else()
	        set(PLATFORM_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/platform_stub.c PARENT_SCOPE)
        endif()
        if (${use_socketio})
        set(SOCKETIO_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/socketio_berkeley.c PARENT_SCOPE)
        endif()
        set(THREAD_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/threadapi_pthreads.c PARENT_SCOPE)
        set(WSIO_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/wsio_libwebsockets.c PARENT_SCOPE)
        set(TICKCOUTER_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/tickcounter_linux.c PARENT_SCOPE)
        set(HTTP_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/httpapi_curl.c PARENT_SCOPE)
        set(UNIQUEID_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/uniqueid_stub.c PARENT_SCOPE)
        if (NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
            if(${use_audio})
                set(AUDIO_C_FILE ${CMAKE_CURRENT_LIST_DIR}/adapters/audio_sys_linux.c PARENT_SCOPE)
            endif()
        endif()
    endif()
endfunction(set_platform_files)

#Setup the platform files
set_platform_files()


include_directories(${UMOCK_C_INC_FOLDER})

if(${use_condition})
    compileAsC11()
else()
    compileAsC99() 
endif()

#these are the C source files
set(source_c_files
./src/base64.c
./src/buffer.c
./src/crt_abstractions.c
./src/doublylinkedlist.c
./src/gballoc.c
./src/gb_stdio.c
./src/gb_time.c
./src/hmac.c
./src/hmacsha256.c
./src/xio.c
./src/list.c
./src/map.c
./src/queue.c
./src/ring_buffer.c
./src/sastoken.c
./src/sha1.c
./src/sha224.c
./src/sha384-512.c
./src/strings.c
./src/string_tokenizer.c
./src/urlencode.c
./src/usha.c
./src/vector.c
./adapters/agenttime.c
${LOGGER_SRC_FILES}
${CONDITION_C_FILE}
${LOCK_C_FILE}
${PLATFORM_C_FILE}
${SOCKETIO_C_FILE}
${TICKCOUTER_C_FILE}
${THREAD_C_FILE}
${UNIQUEID_C_FILE}
${AUDIO_C_FILE}
${WSIO_FILE}
)

if(${use_http})
    set(source_c_files ${source_c_files}
        ./src/httpapiex.c
        ./src/httpapiexsas.c
        ./src/httpheaders.c
        ${HTTP_C_FILE}
    )
endif()

if(${use_schannel})
    set(source_c_files ${source_c_files}
        ./src/tlsio_schannel.c
    )
endif()
if(${use_wolfssl})
    set(source_c_files ${source_c_files}
        ./src/tlsio_wolfssl.c
    )
endif()
if(${use_openssl})
    set(source_c_files ${source_c_files}
        ./src/tlsio_openssl.c
    )
endif()

#these are the C headers
set(source_h_files
./inc/azure_c_shared_utility/agenttime.h
./inc/azure_c_shared_utility/base64.h
./inc/azure_c_shared_utility/buffer_.h
./inc/azure_c_shared_utility/crt_abstractions.h
./inc/azure_c_shared_utility/constmap.h
./inc/azure_c_shared_utility/condition.h
./inc/azure_c_shared_utility/consolelogger.h
./inc/azure_c_shared_utility/doublylinkedlist.h
./inc/azure_c_shared_utility/gballoc.h
./inc/azure_c_shared_utility/gb_stdio.h
./inc/azure_c_shared_utility/gb_time.h
./inc/azure_c_shared_utility/hmac.h
./inc/azure_c_shared_utility/hmacsha256.h
./inc/azure_c_shared_utility/list.h
./inc/azure_c_shared_utility/lock.h
./inc/azure_c_shared_utility/macro_utils.h
./inc/azure_c_shared_utility/map.h
./inc/azure_c_shared_utility/platform.h
./inc/azure_c_shared_utility/queue.h
./inc/azure_c_shared_utility/refcount.h
./inc/azure_c_shared_utility/ring_buffer.h
./inc/azure_c_shared_utility/sastoken.h
./inc/azure_c_shared_utility/sha-private.h
./inc/azure_c_shared_utility/sha.h
./inc/azure_c_shared_utility/socketio.h
./inc/azure_c_shared_utility/stdint_ce6.h
./inc/azure_c_shared_utility/strings.h
./inc/azure_c_shared_utility/string_tokenizer.h
./inc/azure_c_shared_utility/tickcounter.h
./inc/azure_c_shared_utility/threadapi.h
./inc/azure_c_shared_utility/xio.h
./inc/azure_c_shared_utility/uhttp.h
./inc/azure_c_shared_utility/umock_c_prod.h
./inc/azure_c_shared_utility/uniqueid.h
./inc/azure_c_shared_utility/urlencode.h
./inc/azure_c_shared_utility/vector.h
./inc/azure_c_shared_utility/xlogging.h
./inc/azure_c_shared_utility/constbuffer.h
./inc/azure_c_shared_utility/tlsio.h
)

if(${use_audio})
    set(source_h_files ${source_h_files}
        ./inc/azure_c_shared_utility/audio_sys.h
    )
endif()

if(${use_http})
    set(source_h_files ${source_h_files}
        ./inc/azure_c_shared_utility/httpapi.h
        ./inc/azure_c_shared_utility/httpapiex.h
        ./inc/azure_c_shared_utility/httpapiexsas.h
        ./inc/azure_c_shared_utility/httpheaders.h
    )
endif()

if(${use_schannel})
    set(source_h_files ${source_h_files}
        ./inc/azure_c_shared_utility/tlsio_schannel.h
    )
endif()
if(${use_wolfssl})
    set(source_h_files ${source_h_files}
        ./inc/azure_c_shared_utility/tlsio_wolfssl.h
    )
if(WIN32)
include_directories($ENV{WolfSSLDir})
endif()
endif()
if(${use_openssl})
    set(source_h_files ${source_h_files}
        ./inc/azure_c_shared_utility/tlsio_openssl.h
    )
endif()

# needed by use_openssl and LWS.
if (NOT $ENV{OPENSSL_ROOT_DIR} STREQUAL "")
    set(OPENSSL_ROOT_DIR $ENV{OPENSSL_ROOT_DIR})
elseif(NOT $ENV{OpenSSLDir} STREQUAL "")
    set(OPENSSL_ROOT_DIR $ENV{OpenSSLDir})
endif()

if (NOT ${OPENSSL_ROOT_DIR} STREQUAL "")
    include_directories(${OPENSSL_ROOT_DIR}/include)
elseif (DEFINED OPENSSL_INCLUDE_DIRS)
    include_directories(${OPENSSL_INCLUDE_DIRS})
endif()

if(WIN32)
else()
    add_files_to_install("${source_h_files}")
endif()

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(AZURE_C_SHARED_UTILITY_BUILT "77772C70-53C7-4CB0" PARENT_SCOPE)
endif()

IF(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

#this is the product (a library)
add_library(aziotsharedutil ${source_c_files} ${source_h_files})

if(${use_http})
    if(WIN32)
        if (WINCE)
            target_link_libraries(aziotsharedutil crypt32.lib ws2.lib)
        else()
            target_link_libraries(aziotsharedutil winhttp.lib)
        endif()
    elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
        target_link_libraries(aziotsharedutil curl)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
        target_link_libraries(aziotsharedutil ${CURL_LIBS})
        include_directories(${CURL_INCLUDE_DIRS})
    endif()
endif()

if(${use_schannel})
    if(WIN32)
        target_link_libraries(aziotsharedutil ws2_32 secur32)
    endif()
endif()

if(${use_openssl})
    if (WIN32)
    elseif (NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
        target_link_libraries(aziotsharedutil ssl crypto)
    else()
        target_link_libraries(aziotsharedutil ${OPENSSL_LIBS})
    endif()
endif()

if(${use_wolfssl})
    target_link_libraries(aziotsharedutil wolfssl)
endif()

if(WIN32)
elseif (NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(aziotsharedutil pthread)
endif()

if(${use_audio})
    if (UNIX AND NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
        MESSAGE(STATUS "adding libasound")
        target_link_libraries(aziotsharedutil asound)
    endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR ANDROID)
    target_link_libraries(aziotsharedutil ${LIBWEBSOCKETS_LIBRARIES})
endif()

if (NOT ${skip_unittests})
    add_subdirectory(testtools)
    add_subdirectory(tests)
endif()

if(WIN32)
else()
    install (TARGETS aziotsharedutil DESTINATION lib)
    install (FILES ${source_h_files} DESTINATION ${INCLUDE_INSTALL_DIR})
endif (WIN32)


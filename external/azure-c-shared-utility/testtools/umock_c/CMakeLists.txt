#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for umock_c
cmake_minimum_required(VERSION 2.8.11)

set(umock_c_c_files
./src/umock_c.c
./src/umock_c_negative_tests.c
./src/umockalloc.c
./src/umockcall.c
./src/umockcallrecorder.c
./src/umocktypename.c
./src/umocktypes.c
./src/umocktypes_bool.c
./src/umocktypes_c.c
./src/umocktypes_stdint.c
./src/umocktypes_charptr.c
)

set(umock_c_h_files
./inc/umock_c.h
./inc/umock_c_internal.h
./inc/umock_c_negative_tests.h
./inc/umockalloc.h
./inc/umockcall.h
./inc/umockcallrecorder.h
./inc/umocktypename.h
./inc/umocktypes.h
./inc/umocktypes_bool.h
./inc/umocktypes_c.h
./inc/umocktypes_stdint.h
./inc/umocktypes_charptr.h
)

IF(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

add_library(umock_c ${umock_c_c_files} ${umock_c_h_files})

set_target_properties(umock_c
               PROPERTIES
               FOLDER "test_tools") 

#these are the include folders
#the following "set" statetement exports across the project a global variable called UMOCK_C_INC_FOLDER that expands to whatever needs to included when using umock_c library
set(UMOCK_C_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using umock_c lib" FORCE)

include_directories(${UMOCK_C_INC_FOLDER} ${SHARED_UTIL_INC_FOLDER})

function(umockc_windows_unittests_add_dll whatIsBuilding)
    link_directories(${whatIsBuilding}_dll $ENV{VCInstallDir}UnitTest/lib)
    
    add_library(${whatIsBuilding}_dll SHARED 
        ${${whatIsBuilding}_test_files} 
        ${${whatIsBuilding}_h_files} 
        ${${whatIsBuilding}_c_files}
    )

    set_target_properties(${whatIsBuilding}_dll
               PROPERTIES
               FOLDER "umockc_tests") 
    
    target_include_directories(${whatIsBuilding}_dll PUBLIC ${sharedutil_include_directories} $ENV{VCInstallDir}UnitTest/include)
    target_compile_definitions(${whatIsBuilding}_dll PUBLIC -DCPP_UNITTEST)
    SET_SOURCE_FILES_PROPERTIES( ${${whatIsBuilding}_test_files} PROPERTIES LANGUAGE CXX )
    target_link_libraries(${whatIsBuilding}_dll ctest testrunnerswitcher ${ARGN})
endfunction()

function(umockc_windows_unittests_add_lib whatIsBuilding)
    link_directories(${whatIsBuilding}_lib $ENV{VCInstallDir}UnitTest/lib)
    
    add_library(${whatIsBuilding}_lib STATIC 
        ${${whatIsBuilding}_test_files} 
        ${${whatIsBuilding}_h_files} 
        ${${whatIsBuilding}_c_files}
    )
    
    target_include_directories(${whatIsBuilding}_lib PUBLIC ${sharedutil_include_directories})
    target_compile_definitions(${whatIsBuilding}_lib PUBLIC -DUSE_CTEST)
    target_link_libraries(${whatIsBuilding}_lib ctest testrunnerswitcher ${ARGN})
endfunction()

function(umockc_windows_unittests_add_exe whatIsBuilding)
    add_executable(${whatIsBuilding}_exe
        ${${whatIsBuilding}_test_files} 
        ${${whatIsBuilding}_h_files} 
        ${${whatIsBuilding}_c_files}
        ${CMAKE_CURRENT_LIST_DIR}/main.c
    )
    set_target_properties(${whatIsBuilding}_exe
               PROPERTIES
               FOLDER "umockc_tests")
                
    target_compile_definitions(${whatIsBuilding}_exe PUBLIC -DUSE_CTEST)
    target_include_directories(${whatIsBuilding}_exe PUBLIC ${sharedutil_include_directories})
    target_link_libraries(${whatIsBuilding}_exe ctest testrunnerswitcher ${ARGN})
    add_test(NAME ${whatIsBuilding} COMMAND ${whatIsBuilding}_exe)
endfunction()

function(umockc_build_test_artifacts whatIsBuilding use_gballoc)
    
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
            umockc_windows_unittests_add_exe(${whatIsBuilding} ${ARGN})
            umockc_windows_unittests_add_dll(${whatIsBuilding} ${ARGN})
        endif()
    else()
        if(
            (("${whatIsBuilding}" MATCHES ".*unittests.*") AND ${skip_unittests})
        )
        else()
            umockc_windows_unittests_add_exe(${whatIsBuilding} ${ARGN})
        endif()
    endif()
endfunction()

if (NOT ${skip_unittests})
    add_subdirectory(tests)
endif()

# Setup SWIG
find_package(SWIG REQUIRED)
include(UseSWIG)

set(SWIG_INTERFACE "${CARBON_BINDINGS}/carbon.i")
set(SWIG_WRAPPERS "${CARBON_BINDINGS}/wrappers.h")

add_definitions(-DSWIG)

if(MSVC)
     # Force to always compile with W4
     set(CMAKE_CXX_FLAGS "/W3 /EHsc")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
     set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter -fPIC")
endif()

# Add subdirectories for each language if desired
option(BUILD_PYTHON "Build Python SWIG module" ON)
if(BUILD_PYTHON)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        if(NOT ANDROID)
            add_subdirectory(python)
        endif()
    endif()   
endif()

option(BUILD_CSHARP "Build C# SWIG module" ON)
if(BUILD_CSHARP AND
   NOT ANDROID AND
   NOT APPLE AND
   (WIN32 OR ${CMAKE_SIZEOF_VOID_P} EQUAL 8))
       add_subdirectory(csharp)
endif()

option(BUILD_JAVA "Build Java/Android SWIG module" ON)
if(BUILD_JAVA)
    add_subdirectory(java)
endif()

# Dummy target to make this CMakeLists file appear 
# in the generated VS solution.
add_custom_target(swig 
    SOURCES ${SWIG_INTERFACE} ${SWIG_WRAPPERS})
set_target_properties(swig PROPERTIES FOLDER bindings)

# Setup SWIG
find_package(SWIG REQUIRED)
include(UseSWIG)

set(SWIG_INTERFACE "${PROJECT_SOURCE_DIR}/bindings/carbon.i")
set(SWIG_WRAPPERS "${PROJECT_SOURCE_DIR}/bindings/wrappers.h")

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
        add_subdirectory(python)
    endif()   
endif()

if(WIN32)
    option(BUILD_CSHARP "Build C# SWIG module" ON)
    if(BUILD_CSHARP)
        add_subdirectory(csharp)
    endif()
endif()

if(WIN32 OR ANDROID)
    option(BUILD_JAVA "Build Java/Android SWIG module" ON)
    if(BUILD_JAVA)
        add_subdirectory(java)
    endif()
endif()

# Dummy target to make this CMakeLists file appear 
# in the generated VS solution.
add_custom_target(swig 
    SOURCES ${SWIG_INTERFACE} ${SWIG_WRAPPERS})
set_target_properties(swig PROPERTIES FOLDER bindings)

# Setup SWIG
find_package(SWIG REQUIRED)
include(UseSWIG)

set(SWIG_INTERFACE "${PROJECT_SOURCE_DIR}/bindings/carbon.i")
set(SWIG_WRAPPERS "${PROJECT_SOURCE_DIR}/bindings/wrappers.h")

# Add subdirectories for each language if desired
option(BUILD_PYTHON "Build Python SWIG module" ON)
if(BUILD_PYTHON)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        add_subdirectory(python)
    endif()   
endif()

# Dummy target to make this CMakeLists file appear 
# in the generated VS solution.
add_custom_target(swig)
set_target_properties(swig PROPERTIES FOLDER bindings)

if(WIN32)
option(BUILD_CSHARP "Build C# SWIG module" ON)
if(BUILD_CSHARP)
    add_subdirectory(csharp)
endif()
endif()

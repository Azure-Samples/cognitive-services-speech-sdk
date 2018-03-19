project(carbon_py)

find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)

message(STATUS "Using Python interpreter: " ${PYTHON_EXECUTABLE})
message(STATUS "Found Python libs at: ${PYTHON_LIBRARIES}")
message(STATUS "Python include path: ${PYTHON_INCLUDE_PATH}")

include_directories(${PYTHON_INCLUDE_PATH})

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

# Since we're linking  against the release version of python.lib, disable iterator debugging.
if(MSVC)
    # Don't try to link pythonXY_d.lib, use release version of python.lib instead.
     add_definitions(-DSWIG_PYTHON_INTERPRETER_NO_DEBUG)

     add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
     # Compile with W3, too many warning in SWIG-generated code with /W4
     set(CMAKE_CXX_FLAGS "/W3")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
     set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter -fPIC")
endif()


# ${SWIG_INTERFACE} should resolve to {repo/root}/bindings/carbon.i
set(CMAKE_SWIG_FLAGS "")

set_source_files_properties("${SRC_DIR}/carbon_py.i" PROPERTIES
    CPLUSPLUS ON
    #SWIG_FLAGS "-E"
    #SWIG_FLAGS "-includeall"
    )

#if(MSVC)
#  set_source_files_properties(../carbon.i PROPERTIES 
#    SWIG_FLAGS "-D_WIN32"
#    )
#endif()

# TODO: is this really needed?
# On windows the _WIN64 is autodefined by the compiler cl.exe when using the Win64 toolchain
if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
   set_source_files_properties(${SWIG_INTERFACE} PROPERTIES SWIG_FLAGS "-D_WIN64")
endif()

include_directories(${INC_DIR})
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})
include_directories("${SRC_DIR}/..")

set(SOURCES 
    "${SRC_DIR}/carbon_py.i"
    "${SRC_DIR}/test.py.in"
    "${SRC_DIR}/conftest.py"
)

set(SWIG_MODULE_${PROJECT_NAME}_EXTRA_DEPS ${SWIG_INTERFACE} ${SWIG_WRAPPERS})

if("${CMAKE_VERSION}" VERSION_GREATER 3.8)
    swig_add_library(${PROJECT_NAME} LANGUAGE python SOURCES ${SOURCES})
else()
    swig_add_module(${PROJECT_NAME} python ${SOURCES})
endif()

swig_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES} libcarbon)

# Copied from https://www.vlam3d.com/2016/09/14/swig-monologues-part-1-getting-started-with-cmake/?lang=en,
# TODO: check to make sure this is really needed.
if(MSVC)
   set_target_properties(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} PROPERTIES COMPILE_FLAGS "/bigobj")
endif()

#target_include_directories(${SWIG_MODULE_carbon_py_REAL_NAME} PRIVATE 
#    ${PYTHON_INCLUDE_DIRS})

#target_link_libraries(${SWIG_MODULE_carbon_py_REAL_NAME} PRIVATE
    #${PROJECT_NAME})

set_target_properties(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} PROPERTIES
    FOLDER bindings/python
)

if(UNIX)
   set(TEST_PY_UNIX 1)
else()
    set(TEST_PY_UNIX 0)
endif()
set(FROM ${CMAKE_CURRENT_SOURCE_DIR})
set(TO ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${FROM}/test.py.in ${TO}/test.py)
configure_file(${FROM}/conftest.py ${TO}/conftest.py COPYONLY)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SWIG_INTERFACE})

source_group("Source Files" FILES ${SOURCES})

# see  https://github.com/Mizux/cmake-swig
if(UNIX)
# if(APPLE)
#    set_property(TARGET ${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} APPEND PROPERTY
#        LINK_FLAGS "-flat_namespace -undefined suppress")
# endif()
endif()

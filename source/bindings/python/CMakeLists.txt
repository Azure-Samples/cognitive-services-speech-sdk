project(speech_py_impl)

find_package(PythonInterp 3.5 REQUIRED)
find_package(PythonLibs 3.5 REQUIRED)
set(Python3_EXECUTABLE ${PYTHON_EXECUTABLE})
set(Python3_LIBRARIES ${PYTHON_LIBRARIES})
set(Python3_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS})

if(DEFINED SKBUILD)
  find_package(PythonExtensions REQUIRED)
endif()

message(STATUS "Using Python interpreter: " ${Python3_EXECUTABLE})
message(STATUS "Found Python libs at: " ${Python3_LIBRARIES})
message(STATUS "Python include path: " ${Python3_INCLUDE_DIRS})

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

set(PYTHON_PACKAGE_NAME azure/cognitiveservices/speech)

# Since we're linking  against the release version of python.lib, disable iterator debugging.
if(MSVC)
    # Don't try to link pythonXY_d.lib, use release version of python.lib instead.
     add_definitions(-DSWIG_PYTHON_INTERPRETER_NO_DEBUG)
     add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
endif()

set(CMAKE_SWIG_FLAGS
   "${CMAKE_SWIG_FLAGS}"
   -w401,314,325
   )

set(swig_include_dirs
    ${CARBON_C_API}
    ${CARBON_CXX_API}
    ${SRC_DIR}/..
    ${SRC_DIR}/swig-patch
    ${SRC_DIR}/swig-patch/python
    )

set_source_files_properties("${SRC_DIR}/src/speech_py_impl.i" PROPERTIES
    CPLUSPLUS ON
    SWIG_FLAGS "-E"
    SWIG_FLAGS "-py3"
    INCLUDE_DIRECTORIES "${swig_include_dirs}"
    )

# TODO: is this really needed?
# On windows the _WIN64 is autodefined by the compiler cl.exe when using the Win64 toolchain
if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
   set_source_files_properties(${SWIG_INTERFACE} PROPERTIES SWIG_FLAGS "-D_WIN64")
endif()

set(SOURCES
    "${SRC_DIR}/src/speech_py_impl.i"
)

# ${SWIG_INTERFACE} and ${SWIG_WRAPPERS} point to common interface / header files
set(SWIG_MODULE_${PROJECT_NAME}_EXTRA_DEPS ${SWIG_INTERFACE} ${SWIG_WRAPPERS})

# UseSwig supports INCLUDE_DIRECTORIES and OUTPUT_DIR properties on source files only since CMake 3.12
include_directories("${swig_include_dirs}")
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_PACKAGE_NAME})

if("${CMAKE_VERSION}" VERSION_GREATER 3.8)
    swig_add_library(${PROJECT_NAME}
        LANGUAGE python
        OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_PACKAGE_NAME} # only supported on cmake > 3.11
        SOURCES ${SOURCES}
        $<TARGET_OBJECTS:c_api>
    )
else()
    swig_add_module(${PROJECT_NAME} python ${SOURCES})
endif()

target_include_directories(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME}
    PRIVATE ${CARBON_C_API}
    PRIVATE ${CARBON_CXX_API}
    PRIVATE ${Python3_INCLUDE_DIRS}
    PRIVATE "${SRC_DIR}/.."
    )

if(MSVC)
    target_link_libraries(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME}
            PRIVATE sr
            PRIVATE audio
            PRIVATE usp
            PRIVATE common
            PRIVATE factory
            PRIVATE ${PYTHON_LIBRARIES}
        )
else()
    target_link_libraries(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME}
            PRIVATE sr
            PRIVATE audio
            PRIVATE usp
            PRIVATE common
            PRIVATE factory
        )
    set_target_properties(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME}
        PROPERTIES INSTALL_RPATH "$ORIGIN")
endif()

if(APPLE)
    set_target_properties(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()

# Copied from https://www.vlam3d.com/2016/09/14/swig-monologues-part-1-getting-started-with-cmake/?lang=en,
# TODO: check to make sure this is really needed.
# also, SWIG generates an unused variable.
# Also, ensure we build with debug information (TODO global setting does not apply?).
if(MSVC)
   set_target_properties(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} PROPERTIES
     COMPILE_FLAGS "/bigobj /wd4101"
     LINK_FLAGS "/DEBUG /OPT:REF /OPT:ICF")
endif()

if(DEFINED SKBUILD)
  python_extension_module(${PROJECT_NAME})
  set(module_dir "${PYTHON_RELATIVE_SITE_PACKAGES_DIR}/${PYTHON_PACKAGE_NAME}")

  install(TARGETS ${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} DESTINATION ${module_dir})

  # strip swig-generated type hints. This only works on bash and fails silently otherwise, e.g. when the
  # build is run from powershell etc.
  install(CODE "execute_process(COMMAND bash stripscript.sh ${CMAKE_SWIG_OUTDIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})")

  install(FILES ${CMAKE_SWIG_OUTDIR}/speech_py_impl.py DESTINATION ${module_dir} )

  if(EXTENSION_KWS_ENABLED)
    # TODO needs cmake 3.13 for out-of-dir targets:
    # install(TARGETS kws_factory DESTINATION ${module_dir})
    add_dependencies(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} kws_factory)
    install(FILES "$<TARGET_FILE:kws_factory>" DESTINATION ${module_dir})
  endif()
endif()

set_target_properties(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} PROPERTIES
    FOLDER bindings/python
    )

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY
    CMAKE_CONFIGURE_DEPENDS ${SWIG_INTERFACE})

source_group("Source Files" FILES ${SOURCES})


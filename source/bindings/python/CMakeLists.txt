project(speech_py_impl)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(python)

message(STATUS "Found Python interpreter: ${PYTHON_EXECUTABLE} (found version \"${PYTHON_VERSION_STRING}\")")
message(STATUS "Found Python libs at: ${PYTHON_LIBRARIES} (found version \"${PYTHONLIBS_VERSION_STRING}\")")
message(STATUS "Python include path: ${PYTHON_INCLUDE_DIRS}")

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

set(PYTHON_PACKAGE_NAME azure-cognitiveservices-speech)
string(REPLACE - / PYTHON_PACKAGE_REL_PATH ${PYTHON_PACKAGE_NAME})
string(REPLACE - . PYTHON_DOTTED_NAME ${PYTHON_PACKAGE_NAME})

# Since we're linking  against the release version of python.lib, disable iterator debugging.
if(MSVC)
    # Don't try to link pythonXY_d.lib, use release version of python.lib instead.
     add_definitions(-DSWIG_PYTHON_INTERPRETER_NO_DEBUG)
     add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
else()
     # Prevent warning for deprecated tp_print with SWIG 3.x and Python 3.8
     add_definitions(-Wno-deprecated-declarations)
endif()

set(CMAKE_SWIG_FLAGS
   "${CMAKE_SWIG_FLAGS}"
   -w401,314,325
   )

set(swig_include_dirs
    ${CARBON_C_API}
    ${CARBON_CXX_API}
    ${SRC_DIR}/..
    ${PYTHON_INCLUDE_DIRS}
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

set(SWIG_SOURCES
    "${SRC_DIR}/src/speech_py_impl.i"
)

#######################
## Python Packaging  ##
#######################
set(MODULE_ROOT "${CMAKE_CURRENT_BINARY_DIR}/../../..")
set(SPEECH_PY_OUTDIR ${MODULE_ROOT}/${PYTHON_PACKAGE_REL_PATH})
set(CMAKE_SWIG_OUTDIR "${SPEECH_PY_OUTDIR}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SPEECH_PY_OUTDIR})
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${SPEECH_PY_OUTDIR})
endforeach()

# ${SWIG_INTERFACE} and ${SWIG_WRAPPERS} point to common interface / header files
set(SWIG_MODULE_${PROJECT_NAME}_EXTRA_DEPS ${SWIG_INTERFACE} ${SWIG_WRAPPERS})

# UseSwig supports INCLUDE_DIRECTORIES and OUTPUT_DIR properties on source files only since CMake 3.12
include_directories("${swig_include_dirs}")

if(WIN32)
  set(SPEECHSDK_ORIGINAL_FILENAME ${PROJECT_NAME})
  configure_file(
    ${CARBON_ROOT}/ci/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc)

  set(SOURCES_VERSION "${CMAKE_CURRENT_BINARY_DIR}/windows/version.rc")
else()
  set(SOURCES_VERSION "")
endif()

if("${CMAKE_VERSION}" VERSION_GREATER 3.8)
    swig_add_library(${PROJECT_NAME}
        LANGUAGE python
        OUTPUT_DIR ${SPEECH_PY_OUTDIR} # only supported on cmake > 3.11
        SOURCES ${SWIG_SOURCES} ${SOURCES_VERSION}
    )
else()
  swig_add_module(${PROJECT_NAME} python ${SWIG_SOURCES} ${SOURCES_VERSION})
endif()

target_include_directories(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME}
    PRIVATE ${CARBON_C_API}
    PRIVATE ${CARBON_CXX_API}
    PRIVATE ${PYTHON_INCLUDE_DIRS}
    PRIVATE "${SRC_DIR}/.."
    )

if(MSVC)
    target_link_libraries(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME}
        PRIVATE ${SPEECHSDK_CORE_LIBRARY}
        PRIVATE ${PYTHON_LIBRARIES}
        )
else()
    target_link_libraries(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME}
        PRIVATE ${SPEECHSDK_CORE_LIBRARY}
        )
endif()

if(APPLE)
  set_target_properties(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  set_target_properties(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME}
    PROPERTIES BUILD_RPATH "@loader_path")
elseif(UNIX)
  set_target_properties(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME}
    PROPERTIES BUILD_RPATH "$ORIGIN")
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

# copy the python source files to the staging area for the Python wheel
foreach(INPUT_FILE IN ITEMS REDIST.txt ThirdPartyNotices.md license.md version.txt)
  configure_file(../../../${INPUT_FILE} ${SPEECH_PY_OUTDIR}/../../../${INPUT_FILE} COPYONLY)
endforeach()

foreach(INPUT_FILE IN ITEMS setup.cfg setup.py.in)
  configure_file(${INPUT_FILE} ${SPEECH_PY_OUTDIR}/../../../${INPUT_FILE} COPYONLY)
endforeach()

set(PYTHON_SOURCES
  __init__.py
  speech.py
  audio.py
  translation.py
  intent.py
  languageconfig.py
  )

foreach(INPUT_FILE IN ITEMS ${PYTHON_SOURCES})
  configure_file(src/${INPUT_FILE} ${SPEECH_PY_OUTDIR}/${INPUT_FILE} COPYONLY)
endforeach()

# set up the core library in the correct place
add_custom_command(
  TARGET ${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  "$<TARGET_FILE:${SPEECHSDK_CORE_LIBRARY}>"
  ${SPEECH_PY_OUTDIR}
  )

if(EXTENSION_KWS_ENABLED)
  # TODO needs cmake 3.13 for out-of-dir targets:
  # install(TARGETS kws_factory DESTINATION ${module_dir})
  add_dependencies(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} kws_factory)
  # set up the kws library in the correct place
  add_custom_command(
    TARGET ${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:kws_factory>"
    ${SPEECH_PY_OUTDIR}
    )
endif()

set(SETUP_PY "${MODULE_ROOT}/setup.py")

add_custom_command(OUTPUT ${SETUP_PY}
  COMMAND ${CMAKE_COMMAND}
    -DSETUP_PY=${SETUP_PY}
    -DPYTHON_DOTTED_NAME=${PYTHON_DOTTED_NAME}
    -DPYTHON_PACKAGE_NAME=${PYTHON_PACKAGE_NAME}
    -DPYTHON_PACKAGE_REL_PATH=${PYTHON_PACKAGE_REL_PATH}
    -DPYTHON_LIBS="$<TARGET_FILE_NAME:${SPEECHSDK_CORE_LIBRARY}>","$<TARGET_FILE_NAME:_speech_py_impl>",$<$<BOOL:${EXTENSION_KWS_ENABLED}>:"$<TARGET_FILE_NAME:$<IF:$<BOOL:${EXTENSION_KWS_ENABLED}>,kws_factory,_speech_py_impl>>">
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/write_setup_py.cmake
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/write_setup_py.cmake
    ${SETUP_PY}.in
  VERBATIM
  )

search_python_module(wheel 0.33.6)
search_python_module(strip_hints 0.1.4)

# strip swig-generated type hints, because they confuse sphinx.
add_custom_command(
  TARGET ${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} POST_BUILD
  COMMAND ${PYTHON_EXECUTABLE} -c \"import strip_hints\; strip_hints.strip_hints_main.process_command_line() \" speech_py_impl.py --to-empty > speech_py_impl_stripped.py
  COMMAND ${CMAKE_COMMAND} -E rename speech_py_impl_stripped.py speech_py_impl.py
  WORKING_DIRECTORY ${SPEECH_PY_OUTDIR}
  )

add_custom_target(bdist ALL
  DEPENDS ${SETUP_PY}
  COMMAND ${CMAKE_COMMAND} -E remove_directory dist
  COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel ${SPEECHSDK_PYTHON_WHEEL_ARGS}
  WORKING_DIRECTORY ${MODULE_ROOT}
  )

set_target_properties(${SWIG_MODULE_${PROJECT_NAME}_REAL_NAME} PROPERTIES
    FOLDER bindings/python
    )

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY
    CMAKE_CONFIGURE_DEPENDS ${SWIG_INTERFACE})

source_group("Source Files" FILES "${SOURCES}")


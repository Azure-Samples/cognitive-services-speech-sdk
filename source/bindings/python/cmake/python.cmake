# Find Python Interpreter
# set(Python_ADDITIONAL_VERSIONS "3.7;3.6;3.5" CACHE STRING "Python to use for binding")
find_package(PythonInterp 3.5 REQUIRED)

# Force PythonLibs to find the same version than the python interpreter (or nothing).
set(Python_ADDITIONAL_VERSIONS "${PYTHON_VERSION_STRING}")
enable_language(CXX) # PythonLibs require enable_language(CXX)
find_package(PythonLibs 3.5 REQUIRED)

# Find if python module MODULE_NAME is available, if not install it to the Python user install
# directory.
function(search_python_module MODULE_NAME INSTALL_MODULE_VERSION)
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import ${MODULE_NAME}; print(${MODULE_NAME}.__version__)"
    RESULT_VARIABLE _RESULT
    OUTPUT_VARIABLE MODULE_VERSION
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  if(${_RESULT} STREQUAL "0")
    message(STATUS "Found python module: ${MODULE_NAME} (found version \"${MODULE_VERSION}\")")
  else()
    message(WARNING "Can't find python module \"${MODULE_NAME}\", install it using pip...")
    if(INSTALL_MODULE_VERSION)
      message(STATUS "installing version: ${INSTALL_MODULE_VERSION}")
      execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -m pip install --user ${MODULE_NAME}==${INSTALL_MODULE_VERSION}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    else()
      execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -m pip install --user ${MODULE_NAME}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()
  endif()
endfunction()


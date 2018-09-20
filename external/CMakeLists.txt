set(AZURE_SHARED_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/azure-c-shared-utility")
if(NOT EXISTS ${AZURE_SHARED_LIB_DIR} OR NOT EXISTS ${AZURE_SHARED_LIB_DIR}/CMakeLists.txt)
    # Note: we do not need recursive Git submodule init as of now.
    execute_process(COMMAND git submodule update --init)
endif()

# Override AZURE_SHARED_LIB build options
set(use_builtin_httpapi ON CACHE BOOL "")
set(skip_samples ON CACHE BOOL "")
set(use_cppunittest OFF CACHE BOOL "")

add_subdirectory(${AZURE_SHARED_LIB_DIR} azure_c_shared_utility EXCLUDE_FROM_ALL)

set(AZURE_SHARED_LIB aziotsharedutil)
set_target_properties(${AZURE_SHARED_LIB} PROPERTIES FOLDER external_libs)

if (UNIX)
    set_target_properties(${AZURE_SHARED_LIB} PROPERTIES COMPILE_FLAGS 
    "-Wno-error=unused-variable -Wno-error=implicit-function-declaration")
endif()


set(AZURE_SHARED_UTIL_INCLUDE ${SHARED_UTIL_INC_FOLDER} PARENT_SCOPE)
set(AZURE_SHARED_LIB ${AZURE_SHARED_LIB} PARENT_SCOPE)

if(WIN32 OR ANDROID)
    add_subdirectory(swig EXCLUDE_FROM_ALL)
endif()

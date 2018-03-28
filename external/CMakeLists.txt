set(AZURE_SHARED_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/azure-c-shared-utility")
if(NOT EXISTS ${AZURE_SHARED_LIB_DIR} OR NOT EXISTS ${AZURE_SHARED_LIB_DIR}/CMakeLists.txt)
    # Note: we do not need recursive Git submodule init as of now.
    execute_process(COMMAND git submodule update --init)
endif()

if(APPLE)
    # apple-based tlsio (tlsio_appleios.c) seem to be buggy
    # (it spouts a ton of "Cannot accept bytes" to stderr)
    set(use_openssl ON CACHE BOOL "Using openssl-based TLS")
endif()
add_subdirectory(${AZURE_SHARED_LIB_DIR} azure_c_shared_utility EXCLUDE_FROM_ALL)

set(AZURE_SHARED_LIB aziotsharedutil)
set_target_properties(${AZURE_SHARED_LIB} PROPERTIES FOLDER external_libs)

if (UNIX)
    set_target_properties(${AZURE_SHARED_LIB} PROPERTIES COMPILE_FLAGS 
    "-Wno-error=unused-variable -Wno-error=int-conversion -Wno-error=implicit-function-declaration")
    if (APPLE)
        # Apple deprecated OpenSSL:
        # https://stackoverflow.com/questions/7406946/why-is-apple-deprecating-openssl-in-macos-10-7-lion
        set_target_properties (${AZURE_SHARED_LIB} PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations")
    endif()
endif()


set(AZURE_SHARED_UTIL_INCLUDE ${SHARED_UTIL_INC_FOLDER} PARENT_SCOPE)
set(AZURE_SHARED_LIB ${AZURE_SHARED_LIB} PARENT_SCOPE)

if(WIN32)
    add_subdirectory(swig EXCLUDE_FROM_ALL)
endif()
set(LIBWEBSOCKETS_DIR "${CMAKE_CURRENT_LIST_DIR}/libwebsockets")
if(NOT EXISTS ${LIBWEBSOCKETS_DIR} OR NOT EXISTS ${LIBWEBSOCKETS_DIR}/CMakeLists.txt)
    execute_process(COMMAND git submodule update --init)
endif()

if(UNIX)
    add_subdirectory(${LIBWEBSOCKETS_DIR} libwebsockets EXCLUDE_FROM_ALL)
    include_directories (${LIBWEBSOCKETS_INCLUDE_DIRS})
endif()

add_subdirectory("azure-c-shared-utility" azure_c_shared_utility EXCLUDE_FROM_ALL)

set(AZURE_SHARED_LIB aziotsharedutil)
set_target_properties(${AZURE_SHARED_LIB} PROPERTIES FOLDER external_libs)

if (UNIX)
    set_target_properties(${AZURE_SHARED_LIB} PROPERTIES COMPILE_FLAGS "-Wno-error=unused-variable")
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
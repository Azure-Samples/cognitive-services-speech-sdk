project(swig-exe)

file(COPY swig.exe DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if("${CMAKE_VERSION}" VERSION_GREATER 3.12)
    set(SWIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/Lib CACHE INTERNAL "")
else()
    set(SWIG_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
endif()

set(SWIG_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/swig.exe" CACHE INTERNAL "")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_CURRENT_LIST_DIR}/swig-lib.zip WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Note: we build Android binaries from Windows.
# To Android build on Linux hosts, this would need to be adapted.
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_CURRENT_LIST_DIR}/swig-lib-fortify.zip WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

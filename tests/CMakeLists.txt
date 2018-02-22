project(tests)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(SCRIPTS_DIR "${SRC_DIR}/scripts/")

file(GLOB SCRIPTS ${SCRIPTS_DIR}/*.sh)

foreach(script_file ${SCRIPTS})
    file(COPY ${script_file}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})    
endforeach()

add_subdirectory("${SRC_DIR}/core/audio")
add_subdirectory("${SRC_DIR}/public/cxx_api")
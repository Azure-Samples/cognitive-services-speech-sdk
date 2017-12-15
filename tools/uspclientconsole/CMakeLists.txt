project(uspclientconsole)

include_directories ("${USP}/include")

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(SOURCES "${SRC_DIR}/uspclientconsole.c")
source_group("Source Files" FILES ${SOURCES})

add_executable(uspclientconsole ${SOURCES})
target_link_libraries(uspclientconsole usp)

set_target_properties (uspclientconsole PROPERTIES
    FOLDER tools
)
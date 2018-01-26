project(uspclientconsole)

include_directories ("${USP}/include")

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(SOURCES "${SRC_DIR}/uspclientconsole.c")
source_group("Source Files" FILES ${SOURCES})

add_executable(uspclientconsole ${SOURCES})
target_link_libraries(uspclientconsole usp)

# TODO
#[[
if(UNIX)
  set_target_properties (usp PROPERTIES
      COMPILE_FLAGS "-Wall -Werror -Wextra -Wno-long-long" # TODO:  -Wpedantic
  )
endif()
]]

set_target_properties (uspclientconsole PROPERTIES
    FOLDER tools
)
project(uspclientconsole)

include_directories("${USP}/include")
include_directories("${COMMON}/include")
include_directories("${AUDIO}/include")

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(SOURCES "${SRC_DIR}/uspclientconsole.cpp")
source_group("Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} usp audio)


if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
if (${use_openssl})
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
endif()
endif()


# TODO
#[[
if(UNIX)
  set_target_properties(usp PROPERTIES
      COMPILE_FLAGS "-Wall -Werror -Wextra -Wno-long-long" # TODO:  -Wpedantic
  )
endif()
]]

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER tools
)
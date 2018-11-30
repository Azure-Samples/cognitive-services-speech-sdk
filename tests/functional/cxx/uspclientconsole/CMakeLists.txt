project(uspclientconsole)

include_directories("${USP}/include")
include_directories("${COMMON}/include")
include_directories("${AUDIO}/include")
include_directories("${CARBON_CORE}/include")
include_directories("${SR}")
include_directories(${CARBON_C_API})
include_directories(${CARBON_CXX_API})

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(SOURCES "${SRC_DIR}/uspclientconsole.cpp")
source_group("Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} usp audio sr)


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

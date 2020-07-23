project(uspclientconsole)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(SOURCES "${SRC_DIR}/uspclientconsole.cpp")
source_group("Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${CARBON_EXTERNAL}/json"
    ${PRIVATE_AUDIO_HEADERS}
    ${CARBON_C_API}
    ${CARBON_CXX_API}
    "${CARBON_CORE}/include"
)

target_link_libraries(${PROJECT_NAME} audio sr usp ${SPEECHSDK_CORE_LIBRARY})

if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
if (${use_openssl})
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099 /INCREMENTAL:NO")
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



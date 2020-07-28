project(interfaces)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
    "${INC_DIR}/ispxinterfaces.h"
    "${INC_DIR}/interfaces/types.h"
)

add_library(interfaces INTERFACE)

target_sources(interfaces INTERFACE ${HEADERS})

target_include_directories(interfaces
    INTERFACE
        ${INC_DIR}
)

add_custom_target(ispxinterfaces
    SOURCES ${HEADERS}
)

set_target_properties(ispxinterfaces PROPERTIES FOLDER core/shared)
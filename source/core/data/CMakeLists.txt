project(data)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
)

set(HEADERS_PRIVATE
    "${SRC_DIR}/create_module_object.h"
    "${SRC_DIR}/blocking_read_write_buffer.h"
    "${SRC_DIR}/buffer_data.h"
    "${SRC_DIR}/buffer_properties.h"
    "${SRC_DIR}/read_write_ring_buffer.h"
)

set(SOURCES
    "${SRC_DIR}/buffer_data.cpp"
    "${SRC_DIR}/buffer_properties.cpp"
    "${SRC_DIR}/create_module_object.cpp"
    "${SRC_DIR}/read_write_ring_buffer.cpp"
)

set_source_groups()

add_library(data STATIC ${HEADERS} ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(data
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        "${CARBON_EXTERNAL}/json"
    PUBLIC
        ${INC_DIR}
)

set_target_properties (data PROPERTIES
    FOLDER core/input
)

target_link_libraries(data PRIVATE interfaces common)

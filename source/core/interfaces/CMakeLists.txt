project(interfaces)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/include")

set(HEADERS
    "${INC_DIR}/ispxinterfaces.h"
    "${INC_DIR}/interfaces/aggregates.h"
    "${INC_DIR}/interfaces/audio.h"
    "${INC_DIR}/interfaces/base.h"
    "${INC_DIR}/interfaces/containers.h"
    "${INC_DIR}/interfaces/conversation.h"
    "${INC_DIR}/interfaces/conversation_translation.h"
    "${INC_DIR}/interfaces/data.h"
    "${INC_DIR}/interfaces/event_args.h"
    "${INC_DIR}/interfaces/keyword.h"
    "${INC_DIR}/interfaces/notify_me.h"
    "${INC_DIR}/interfaces/recognizers.h"
    "${INC_DIR}/interfaces/results.h"
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
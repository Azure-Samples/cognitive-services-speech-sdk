project(speaker_recognition)

set(SRC_DIR "${PROJECT_SOURCE_DIR}")

set(HEADERS_PRIVATE
    ${SRC_DIR}/create_module_object.h
    ${SRC_DIR}/http_audio_stream_session.h
    ${SRC_DIR}/http_client.h
    ${SRC_DIR}/http_recog_engine_adapter.h
    ${SRC_DIR}/speaker_identification_model.h
    ${SRC_DIR}/speaker_verification_model.h
    ${SRC_DIR}/stdafx.h
    ${SRC_DIR}/voice_profile.h
    ${SRC_DIR}/voice_profile_client.h
 )

 set(SOURCES
    ${SRC_DIR}/create_module_object.cpp
    ${SRC_DIR}/http_audio_stream_session.cpp
    ${SRC_DIR}/http_client.cpp
    ${SRC_DIR}/http_recog_engine_adapter.cpp
    ${SRC_DIR}/speaker_identification_model.cpp
    ${SRC_DIR}/speaker_verification_model.cpp
    ${SRC_DIR}/stdafx.cpp
    ${SRC_DIR}/voice_profile.cpp
    ${SRC_DIR}/voice_profile_client.cpp
 )


# make this the last source file to avoid link warning LNK4221
#list(APPEND SOURCES "${SRC_DIR}/stdafx.cpp")

set_source_groups()

add_library(speaker_recognition STATIC ${HEADERS_PRIVATE} ${SOURCES})

target_include_directories(speaker_recognition
    PRIVATE
        ${CARBON_C_API}
        ${CARBON_CXX_API}
        ${SRC_DIR}
        ${CARBON_EXTERNAL}/json
)

if (WIN32 AND SPECTRE_MITIGATION)
set_property(TARGET speaker_recognition PROPERTY VS_GLOBAL_SpectreMitigation "Spectre")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
# Need C++17 for WinRT API access via cppwinrt.
set_target_properties(speaker_recognition PROPERTIES CXX_STANDARD 17)
target_compile_definitions(speaker_recognition PUBLIC _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

set_target_properties(speaker_recognition PROPERTIES
    FOLDER core
)

target_link_libraries(${PROJECT_NAME} PRIVATE interfaces logging audio common pal usp)

if (MSVC)
   set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)

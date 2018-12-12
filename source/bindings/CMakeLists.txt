# Setup SWIG

if(BUILD_PYTHON OR BUILD_CSHARP OR BUILD_JAVA)

  find_package(SWIG REQUIRED)
  include(UseSWIG)

  set(SWIG_INTERFACE "${CARBON_BINDINGS}/carbon.i")
  set(SWIG_WRAPPERS "${CARBON_BINDINGS}/wrappers.h")

  add_definitions(-DSWIG)

  if(MSVC)
    # Force to always compile with W3 (we are currently on W4)
    string(REGEX REPLACE "/W4" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc")
  elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()

  # Add subdirectories for each language if desired

  if(BUILD_PYTHON)
    if(NOT ANDROID)
      add_subdirectory(python)
    endif()
  endif()


  if(BUILD_CSHARP AND
      NOT ANDROID AND
      (WIN32 OR ${CMAKE_SIZEOF_VOID_P} EQUAL 8)
      )
    add_subdirectory(csharp)
  endif()


  if(BUILD_JAVA)
    add_subdirectory(java)
  endif()

  # Dummy target to make this CMakeLists file appear
  # in the generated VS solution.
  add_custom_target(swig
    SOURCES ${SWIG_INTERFACE} ${SWIG_WRAPPERS})
  set_target_properties(swig PROPERTIES FOLDER bindings)

endif()

if(DEFINED APPLE AND NOT DEFINED SKBUILD)
  add_subdirectory(objective-c)
endif()


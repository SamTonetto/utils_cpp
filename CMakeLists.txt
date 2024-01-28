cmake_minimum_required(VERSION 3.25)

project(utils_cpp VERSION 1.0)

enable_testing()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE OFF) # Enable verbose makefile for debugging
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Create a compile_commands.json for an LSP to use.


if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColorReset "${Esc}[m")
  set(ColorBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()


# All warnings, all errors, and warnings as errors.
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()



# ------ Documentation ------
find_package(Doxygen)

option(BUILD_DOCS "Build docs" FALSE)


if(BUILD_DOCS)
  if(DOXYGEN_FOUND)
    message(STATUS "${PROJECT_NAME} -- ${Green}SUCCESS: Doxygen found${ColorReset}")


    # Copy README.md to use as Doxygen mainpage
    configure_file("${PROJECT_SOURCE_DIR}/README.md" "${CMAKE_CURRENT_BINARY_DIR}/README.md" COPYONLY)
    # Copy doc awesome stylesheet
    configure_file("${PROJECT_SOURCE_DIR}/docs/doxygen-awesome-css/doxygen-awesome.css"
      "${CMAKE_CURRENT_BINARY_DIR}/doxygen-awesome.css" COPYONLY)

    # Set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    message(STATUS "${PROJECT_NAME} -- DOXYGEN_IN = ${DOXYGEN_IN}")

    # Configure Doxyfile
    configure_file(${DOXYGEN_IN} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)

    add_custom_target(utils_docs ALL # all means documentation will be built when running "make"
      COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)
  else(DOXYGEN_FOUND)
    message(STATUS "${PROJECT_NAME} -- ${Red}FAILURE: Doxygen not found. Doxygen must be installed to generate documentation${ColorReset}")
  endif(DOXYGEN_FOUND)
endif(BUILD_DOCS)
# ------ End Documentation ------


# ================ Build Type ================
# Normally you would use
# `cmake .. -DCMAKE_BUILD_TYPE "release"` (or debug) to build in either mode.
# (note: CMake is case-insensitive)
# That is too much to type, so let's set an alias:
# `cmake .. -Db=r` (or d).
set(b "r" CACHE STRING "Build type: 'r' for Release, 'd' for Debug")

# --- Map user input to CMake build types
if(${b} STREQUAL "r")
  message(STATUS "${PROJECT_NAME} -- Build type set to ${ColorBold}${Green}Release${ColorReset}")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type set to Release" FORCE)
elseif(${b} STREQUAL "d")
  message(STATUS "${PROJECT_NAME} -- Build type set to ${ColorBold}${Yellow}Debug${ColorReset}")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type set to Debug" FORCE)
else()
  message(FATAL_ERROR ${PROJECT_NAME} -- "Invalid build type specified: ${b}. Use 'r' for Release, 'd' for Debug.")
endif()
# ================ End Build Type ================

# ================ Build ${PROJECT_NAME} ================
find_package(Boost REQUIRED COMPONENTS graph)
if (Boost_FOUND)
  message(STATUS "graphlib_cpp -- ${Green}SUCCESS: Boost found${ColorReset}")
  message(STATUS "graphlib_cpp -- Boost libraries: ${Boost_LIBRARIES}")
  message(STATUS "graphlib_cpp -- Boost include path: ${Boost_INCLUDE_DIRS}")
else()
  message(STATUS "${Red}graphlib_cpp -- FAILURE: Boost not found${ColorReset}")
  message(FATAL_ERROR "graphlib_cpp -- Boost not found")
endif()


# Define library as header-only, but link to boost graph.
add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>
)
target_include_directories(${PROJECT_NAME} PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external>)

target_link_libraries(${PROJECT_NAME} INTERFACE Boost::graph)
# ================ End Build ${PROJECT_NAME} ================


option(UTILS_CPP_BUILD_DEMOS "Build demos" FALSE)
option(UTILS_CPP_BUILD_TESTS "Build tests" FALSE)

if(UTILS_CPP_BUILD_DEMOS)
  message(STATUS "${PROJECT_NAME} -- ${Blue}Descending into demos${ColorReset}")
  add_subdirectory(demos)
  message(STATUS "${PROJECT_NAME} -- ${Blue}Finished with demos${ColorReset}")
endif()
if(UTILS_CPP_BUILD_TESTS)
  message(STATUS "${PROJECT_NAME} -- ${Blue}Descending into tests${ColorReset}")
  add_subdirectory(tests)
  message(STATUS "${PROJECT_NAME} -- ${Blue}Finished with tests${ColorReset}")
endif()

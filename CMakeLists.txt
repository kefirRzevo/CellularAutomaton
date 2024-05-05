cmake_minimum_required(VERSION 3.16)

project(automaton)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++)

find_package(Boost 1.40 REQUIRED COMPONENTS program_options dynamic_bitset)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(COMPILER_SETTINGS -g -O0 -Wall -Wextra -Wpedantic)  
else ()
    set(COMPILER_SETTINGS -O2 -Wall -Wextra -Wpedantic)
endif ()
add_compile_options(${COMPILER_SETTINGS})

set(PROJECT_SOURCES Automaton.cpp)
add_executable(
  ${PROJECT_NAME}
  ${PROJECT_SOURCES}
)

include_directories(include ${Boost_INCLUDE_DIR})

target_include_directories(
  ${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_DYNAMIC_BITSET_LIBRARY}
    sfml-graphics sfml-window sfml-system
)

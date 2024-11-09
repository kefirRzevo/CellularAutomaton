cmake_minimum_required(VERSION 3.16)

project(automaton)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

find_package(Boost 1.73 REQUIRED COMPONENTS program_options)
include(FetchContent)
FetchContent_Declare(SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.x
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(SFML)

add_executable (${PROJECT_NAME} Automaton.cc)
target_include_directories (${PROJECT_NAME} PRIVATE includes)
target_include_directories (${PROJECT_NAME} PRIVATE include ${Boost_INCLUDE_DIR})
target_link_libraries (${PROJECT_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} sfml-graphics sfml-window sfml-system)

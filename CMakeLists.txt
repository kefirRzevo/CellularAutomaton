cmake_minimum_required(VERSION 3.16)

project(automaton)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_COMPILER /usr/bin/g++)

find_package(Boost 1.85 REQUIRED COMPONENTS program_options container)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(COMPILER_SETTINGS -g -O0 -Wall -Wextra -Wpedantic)
else ()
    set(COMPILER_SETTINGS -O2 -Wall -Wextra -Wpedantic)
endif ()
add_compile_options(${COMPILER_SETTINGS})

# Makes FetchContent functionality available to us
include(FetchContent)

# Fetching the SFML repository into our project
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)

# Make the repository content available to us
FetchContent_MakeAvailable(SFML)

add_executable (${PROJECT_NAME} Automaton.cc)
target_include_directories (${PROJECT_NAME} PRIVATE includes)
target_include_directories (${PROJECT_NAME} PRIVATE include ${Boost_INCLUDE_DIR})
target_link_libraries (${PROJECT_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY} sfml-graphics sfml-window sfml-system)

#g++ -std=c++17 -g -lsfml-graphics -lsfml-window -lsfml-system -lboost_program_options Automaton.cc

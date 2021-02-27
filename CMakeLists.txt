cmake_minimum_required(VERSION 3.10)
project(example)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(MAJOR_VERSION 1)
set(MINOR_VERSION 0)

find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%H:%ad
        OUTPUT_VARIABLE COMMIT_INFO
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE BRANCH_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
message(STATUS "Git version is ${BRANCH_NAME}:${COMMIT_INFO}")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/version.h.ini
  ${CMAKE_CURRENT_SOURCE_DIR}/version.h
  @ONLY
)

add_executable(${CMAKE_PROJECT_NAME} example.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} -lmysqlclient)
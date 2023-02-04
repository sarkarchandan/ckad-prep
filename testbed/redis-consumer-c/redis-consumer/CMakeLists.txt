cmake_minimum_required(VERSION 3.0...3.22)

if(${CMAKE_VERSION} VERSION_LESS 3.22)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(redis-consumer LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
    hiredis
    GIT_REPOSITORY https://github.com/redis/hiredis
    GIT_TAG v1.1.0
)

FetchContent_GetProperties(hiredis)

if(NOT hiredis_POPULATED)
    FetchContent_Populate(hiredis)
    add_subdirectory(${hiredis_SOURCE_DIR} ${hiredis_BINARY_DIR})
endif()

find_package(Boost 1.74 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set(SOURCES
    src/rqueue.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC hiredis)
target_include_directories(${PROJECT_NAME} PRIVATE include)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

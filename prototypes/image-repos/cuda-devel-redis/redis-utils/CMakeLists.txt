cmake_minimum_required(VERSION 3.0...3.22)

if(${CMAKE_VERSION} VERSION_LESS 3.22)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(redis-prototype LANGUAGES C CXX)

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

set(CONSUMER_SRC
    src/rqueue.cpp
    src/consumer.cpp
)

set(PRODUCER_SRC
    src/producer.c
)

add_executable(redis-producer ${PRODUCER_SRC})
target_link_libraries(redis-producer PRIVATE hiredis)
set_property(TARGET redis-producer PROPERTY C_STANDARD 11)

add_executable(redis-consumer ${CONSUMER_SRC})
target_link_libraries(redis-consumer PRIVATE hiredis)
target_include_directories(redis-consumer PRIVATE include)
set_property(TARGET redis-consumer PROPERTY C_STANDARD 11)
target_compile_features(redis-consumer PRIVATE cxx_std_17)

cmake_minimum_required(VERSION 3.0...3.22)

if(${CMAKE_VERSION} VERSION_LESS 3.22)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(redis-cpp LANGUAGES C CXX)

find_package(Boost 1.74 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set(SOURCES
    src/publisher.cpp
    src/subscriber.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# <---------- set c++ standard ------------->
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)
target_include_directories(${PROJECT_NAME} PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(${PROJECT_NAME} ${HIREDIS_LIB})

# <------------ add redis-plus-plus dependency -------------->
find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(${PROJECT_NAME} PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(${PROJECT_NAME} ${REDIS_PLUS_PLUS_LIB})

target_include_directories(${PROJECT_NAME} PRIVATE include)


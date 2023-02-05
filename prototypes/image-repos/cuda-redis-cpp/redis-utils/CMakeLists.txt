cmake_minimum_required(VERSION 3.0...3.22)

if(${CMAKE_VERSION} VERSION_LESS 3.22)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(redis-cpp-base LANGUAGES C CXX)

find_package(Boost 1.74 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set(PUB_SRC
    src/publisher.cpp
    src/pub_daemon.cpp
)

set(SUB_SRC
    src/subscriber.cpp
    src/sub_daemon.cpp
)

add_executable(pub_daemon ${PUB_SRC})
add_executable(sub_daemon ${SUB_SRC})

# <---------- set c++ standard ------------->
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)
target_include_directories(pub_daemon PUBLIC ${HIREDIS_HEADER})
target_include_directories(sub_daemon PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(pub_daemon ${HIREDIS_LIB})
target_link_libraries(sub_daemon ${HIREDIS_LIB})

# <------------ add redis-plus-plus dependency -------------->
find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(pub_daemon PUBLIC ${REDIS_PLUS_PLUS_HEADER})
target_include_directories(sub_daemon PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(pub_daemon ${REDIS_PLUS_PLUS_LIB})
target_link_libraries(sub_daemon ${REDIS_PLUS_PLUS_LIB})

target_include_directories(pub_daemon PRIVATE include)
target_include_directories(sub_daemon PRIVATE include)


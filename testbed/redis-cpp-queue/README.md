# Preparation Steps for Container

1. Use base ubuntu 22.04.
2. Install build-essential, git, cmake.
2. Install hiredis: `apt-get install libhiredis-dev`.
3. Clone: https://github.com/sewenew/redis-plus-plus.git.
4. Build with cmake: `make`, `make install` with all defaults.
5. Use following stub for cmake:

```cmake
# <---------- set c++ standard ------------->
# NOTE: you must build redis-plus-plus and your application code with the same standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)
target_include_directories(target PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(target ${HIREDIS_LIB})

# <------------ add redis-plus-plus dependency -------------->
# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(target PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(target ${REDIS_PLUS_PLUS_LIB})
```
# Testbed

In this section we play around with conceptual prototypes in simple steps.

## Docker Compose - Redis Producer and Consumer

Starting Point: [Fine Parallel Processing with Work Queue](https://kubernetes.io/docs/tasks/job/fine-parallel-processing-work-queue/)

- Redis Master as simple service
- Redis Producer as simple service
    1. With Python [redis-py](https://github.com/redis/redis-py)
    2. With C [hiredis](https://github.com/redis/hiredis)
    3. With C++ [redis-plus-plus](https://github.com/sewenew/redis-plus-plus)
- Redis Consumer as simple service
    1. With C [hiredis](https://github.com/redis/hiredis)
    2. With C++ [redis-plus-plus](https://github.com/sewenew/redis-plus-plus)

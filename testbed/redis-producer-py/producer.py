#! /usr/bin/python3

import redis
import time

if __name__ == "__main__":
    r: redis.Redis = redis.Redis(host="redis", port=6379, db=0)
    for idx in range(1, 100):
        value: str = f"bar-{idx}"
        print(f"Publishing {value}")
        r.rpush("foo", value);
        time.sleep(1)

import time
import redis

class RedisPublisher:

    _q: redis.Redis
    #  Main queue
    _q_name: str  

    def __init__(self, 
        name: str, 
        host: str = "redis", 
        port: int = 6379, 
        db: int=0) -> None:
        self._q = redis.Redis(host=host, port=port, db=db)
        self._q_name = name
    
    def publish(self, count: int) -> None:
        for idx in range(1, count):
            print(f"Publishing: WorkItem-{idx}")
            self._q.rpush(self._q_name, f"WorkItem-{idx}")
            time.sleep(1)


if __name__ == "__main__":
    pass
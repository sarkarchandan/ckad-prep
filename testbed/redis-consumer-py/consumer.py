#! /usr/bin/python3

import redis
import uuid
import hashlib
import time


class RQueue:
    """Encapsulates Redis object to monitor the incoming items"""

    _q: redis.Redis
    # Unique identity for the worker
    _session: str  
    #  Main queue
    _q_name: str  
    # Processing queue where items are moved when client picks the same
    _processing_q_name: str  
    _lease_key_prefix: str

    def __init__(self, 
        name: str, 
        host: str = "redis", 
        port: int = 6379, 
        db: int=0) -> None:
        self._q = redis.Redis(host=host, port=port, db=db)
        self._session = str(uuid.uuid4())
        self._q_name = name
        self._processing_q_name = self._q_name + ":processing"
        self._lease_key_prefix = self._q_name + ":leased_by_session:"

    def session_id(self) -> str:
        """Returns the if for the session"""
        return self._session

    def _main_queue_size(self) -> int:
        """Returns the size of the main queue"""
        return self._q.llen(name=self._q_name)

    def _processing_queue_size(self) -> int:
        """Returns the size of the processing queue"""
        return self._q.llen(name=self._processing_q_name)

    def empty(self) -> bool:
        """Returns whether both main and processing queue is empty.

        NOTE: This does not guarantee that there is no ongoing work by the
        worker.
        """
        return self._main_queue_size() == 0 \
            and self._processing_queue_size() == 0

    def _item_key(self, item: bytes) -> str:
        """Returns a string that uniquely identifies an item from the queue
        
        Args:
            item: Received item from the queue as bytes
        """
        return hashlib.sha224(item).hexdigest()

    def _lease_exists(self, item: bytes) -> int:
        """Returns True if a lease on the item exists
        
        Args:
            item: Received item from the queue as bytes
        """
        return self._q.exists(self._lease_key_prefix + self._item_key(item))

    def lease(self, 
        lease_dur: int = 60, 
        blocking: bool = True, 
        timeout: int = None) -> bytes:
        """Marks the start of working on an item from the main work queue
        
        NOTE: Leases the items for a specified duration. After that duration
        the current worker can be assumed as non-functional and other workers
        can pick the same item.

        Args:
            leas_dur: Integer duration for leasing an item, default 60 seconds
            blocking: Boolean flag to block the queue until an item is available
            timeout: Timeout for blocking the queue, default None means that we
            block until an item is available

        Returns: 
            Item from the queue in bytes
        """
        item: bytes
        if blocking:
            item = self._q.brpoplpush(
                src=self._q_name, dst=self._processing_q_name, timeout=timeout)
        else:
            item = self._q.brpoplpush(
                src=self._q_name, dst=self._processing_q_name)
        if item:
            item_key: str = self._item_key(item=item)
            self._q.setex(
                name=self._lease_key_prefix + item_key, 
                time=lease_dur, 
                value=self._session)
        return item
    
    def complete(self, value: str) -> None:
        """Marks the completion of working on provided value
        
        NOTE: Upon expiry of lease on the item by current worker it is not
        guaranteed that the item is processed by current worker or some other
        worker picked up the same.
        """
        self._q.lrem(name=self._processing_q_name, count=0, value=value)
        item_key: str = self._item_key(item=value)
        self._q.delete(self._lease_key_prefix + item_key)


if __name__ == "__main__":
    q: RQueue = RQueue(name="foo")
    print(f"Worker with sessionID: { q.session_id()}")
    print(f"Initial queue state: empty: {str(q.empty())}")
    while not q.empty():
        item = q.lease(lease_dur=5, blocking=True, timeout=2) 
        if item is not None:
            item_str: str = item.decode("utf-8")
            print(f"Working on: {item_str}")
            # Here we would do some actual work instead of sleeping like 
            # executing a CUDA kernel
            time.sleep(2)
            q.complete(value=item)
        else:
            print("Waiting for work")
    print("Queue empty, exiting")

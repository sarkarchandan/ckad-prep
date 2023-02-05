#! /usr/bin/python3

import time
import argparse
from worker import RedisSubscriber
from publisher import RedisPublisher


if __name__ == "__main__":
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        prog="Daemon", description="Redis Pub Sub daemon process")
    parser.add_argument(
        "host", help="Redis server host", default="redis", type=str)
    parser.add_argument(
        "port", help="Redis server port", default=6379, type=int)
    parser.add_argument(
        "queue", help="Redis queue", default="foo", type=str)
    parser.add_argument(
        "--action",
        help="Daemon actions",
        choices=["pub", "sub"], 
        type=str, 
        required=True)
    args: argparse.Namespace = parser.parse_args()
    if args.action == "pub":
        rdp: RedisPublisher = RedisPublisher(
            name=args.queue, host=args.host, port=args.port)
        rdp.publish(20)
    elif args.action == "sub":
        rds: RedisSubscriber = RedisSubscriber(
            name=args.queue, host=args.host, port=args.port)
        print(f"Worker with sessionID: { rds.session_id()}")
        print(f"Initial queue state: empty: {str(rds.empty())}")
        while not rds.empty():
            item = rds.lease(lease_dur=5, blocking=True, timeout=2) 
            if item is not None:
                item_str: str = item.decode("utf-8")
                print(f"Working on: {item_str}")
                # Here we would do some actual work instead of sleeping like 
                # executing a CUDA kernel
                time.sleep(2)
                rds.complete(value=item)
            else:
                print("Waiting for work")
        print("Queue empty, exiting")
    else:
        raise RuntimeError("Invalid action")





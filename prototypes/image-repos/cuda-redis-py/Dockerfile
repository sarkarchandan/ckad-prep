FROM nvidia/cuda:12.0.0-devel-ubuntu22.04

RUN apt-get update \
    && apt-get install -y build-essential python3 python3-pip \
    && pip install --no-cache-dir redis[hiredis]

COPY ./publisher.py /publisher.py
COPY ./worker.py /worker.py
COPY ./daemon.py /daemon.py

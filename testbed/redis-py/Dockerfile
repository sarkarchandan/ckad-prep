FROM ubuntu:22.04

RUN apt-get update \
    && apt-get install -y build-essential python3 python3-pip \
    && pip install --no-cache-dir redis[hiredis]
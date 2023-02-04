FROM nvidia/cuda:12.0.0-devel-ubuntu22.04

RUN apt-get update \
    && apt-get install -y build-essential cmake git libboost-all-dev

COPY redis-utils /redis-utils
WORKDIR /redis-utils/

RUN mkdir build \
    && cd build \
    && cmake .. \
    && make

WORKDIR /redis-utils/build
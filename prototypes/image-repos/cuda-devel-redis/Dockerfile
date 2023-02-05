FROM nvidia/cuda:12.0.0-devel-ubuntu22.04

RUN apt-get update \
    && apt-get install -y build-essential cmake git libboost-all-dev libhiredis-dev \
    && git clone https://github.com/sewenew/redis-plus-plus.git redis-cpp

WORKDIR /redis-cpp/build

RUN cmake .. \
    && make \
    && make install

COPY redis-utils /redis-utils

WORKDIR /redis-utils/build

RUN cmake .. \
    && make

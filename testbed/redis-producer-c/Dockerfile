FROM ubuntu:22.04

RUN apt-get update \
    && apt-get install -y build-essential cmake git

COPY redis-producer /redis-producer
WORKDIR /redis-producer/

RUN mkdir build \
    && cd build \
    && cmake .. \
    && make

WORKDIR /redis-producer/build

CMD [ "./redis-producer" ]

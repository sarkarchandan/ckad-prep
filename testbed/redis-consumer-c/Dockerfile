FROM ubuntu:22.04

RUN apt-get update \
    && apt-get install -y build-essential cmake git libboost-all-dev

COPY redis-consumer /redis-consumer
WORKDIR /redis-consumer/

RUN mkdir build \
    && cd build \
    && cmake .. \
    && make

WORKDIR /redis-consumer/build

CMD [ "./redis-consumer" ]
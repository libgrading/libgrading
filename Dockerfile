ARG base

# Construct builder image with throwaway elements like cmake and libdistance
FROM $base:latest AS builder

RUN add-apt-repository ppa:professor-jon/grading-software
RUN apt-get update
RUN apt-get install -y cmake libdistance-dev

RUN mkdir /libgrading
COPY . /libgrading/

# Build libgrading
RUN mkdir /libgrading/build && \
    cd /libgrading/build && \
    cmake .. && \
    make install


# Build the actual autograder base image
FROM $base:latest

COPY --from=builder /libgrading/build/include/libgrading.h /usr/local/include/
COPY --from=builder /libgrading/build/src/libgrading.so* /usr/local/lib/

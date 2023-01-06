ARG base

# Construct builder image with throwaway elements like cmake and libdistance
FROM $base:latest AS builder

RUN apt-get update
RUN apt-get install -y cmake

RUN mkdir /libdistance /libgrading
RUN git clone https://github.com/paralax/libdistance /libdistance
COPY . /libgrading/

# Build libdistance
RUN make -C /libdistance && \
    cp /libdistance/distance.h /usr/include && \
    cp /libdistance/libdistance.a /usr/lib

# Build libgrading
RUN mkdir /libgrading/build && \
    cd /libgrading/build && \
    cmake .. && \
    make install


# Build the actual autograder base image
FROM $base:latest

COPY --from=builder /libgrading/build/include/libgrading.h /usr/local/include/
COPY --from=builder /libgrading/build/src/libgrading.so* /usr/local/lib/

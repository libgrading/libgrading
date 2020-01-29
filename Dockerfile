FROM gradescope/auto-builds:latest

COPY include/libgrading.h /usr/local/include/
COPY build/src/libgrading.* /usr/local/lib/

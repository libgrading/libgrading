FROM gradescope/auto-builds:latest

RUN apt-get update && \
    apt-get install -y clang && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    rm -rf /tmp/* /var/tmp/* /root/.cache

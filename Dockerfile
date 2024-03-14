FROM ubuntu:latest
RUN apt-get update && \
    apt-get install software-properties-common python3 git python3-pip cmake \
    gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential \
    libstdc++-arm-none-eabi-newlib clang -y
RUN git clone https://github.com/raspberrypi/pico-sdk.git && \
    cd pico-sdk && \
    git submodule update --init
ENV PICO_SDK_PATH=/pico-sdk
RUN git clone https://github.com/tinygrad/tinygrad.git && \
    cd tinygrad && \
    python3 -m pip install -e .


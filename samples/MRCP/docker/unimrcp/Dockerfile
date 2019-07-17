FROM ubuntu:bionic

LABEL maintainer="yulin.li@microsoft.com"

ARG version=1.6.0

RUN apt-get update && apt-get install -y \
    wget \
    build-essential \
    sudo \
    pkg-config \
    automake \
    libtool \
    libtool-bin \
    && rm -rf /var/lib/apt/lists/*

RUN wget -O unimrcp-deps.tar.gz http://www.unimrcp.org/project/component-view/unimrcp-deps-1-6-0-tar-gz/download && \
    tar zxvf unimrcp-deps.tar.gz && rm unimrcp-deps.tar.gz &&\
    cd unimrcp-deps-$version && \
    ./build-dep-libs.sh -s && cd / &&\
    rm -rf /unimrcp-deps-$version

RUN wget -O unimrcp.tar.gz http://www.unimrcp.org/project/component-view/unimrcp-1-6-0-tar-gz/download && \
    tar zxvf unimrcp.tar.gz && rm unimrcp.tar.gz && \
    cd unimrcp-$version && ./bootstrap && ./configure && make && \
    ldconfig && make install && make distclean

# sip port
EXPOSE 8060
# mrcp port
EXPOSE 1544
# rtp ports
EXPOSE 5000-6000
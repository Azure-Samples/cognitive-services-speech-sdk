# Swift Dockerfile
FROM ubuntu:16.04

ARG SWIFT_VERSION=4.1.2

RUN apt-get update && apt-get install -y \
    wget \
    clang \
    libicu-dev \
    autotools-dev \
    build-essential \
    python-dev \
    libxml2 \
    libcurl3

RUN mkdir /usr/local/swift-$SWIFT_VERSION && \
   wget --no-verbose -O - https://swift.org/builds/swift-$SWIFT_VERSION-release/ubuntu1604/swift-$SWIFT_VERSION-RELEASE/swift-$SWIFT_VERSION-RELEASE-ubuntu16.04.tar.gz| tar -xzf - -C /usr/local/swift-$SWIFT_VERSION --strip-components=1

ENV PATH $PATH:/usr/local/swift-$SWIFT_VERSION/usr/bin


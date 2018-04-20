
FROM i386/ubuntu:16.04

# TODO minimize
RUN apt-get update && \
    apt-get --yes install \
        build-essential \
        cmake \
        pkg-config \
        uuid-dev \
        zlib1g-dev \
        libssl-dev \
        libcurl4-openssl-dev \
        alsa \
        libasound2-dev \
        swig \
        python \
        python-dev \
        default-jre-headless \
        default-jdk-headless \
        curl

# Override at docker build time, as appropriate:
ARG BUILD_UID=0
ARG BUILD_USER=root

# N.B. Root already present, do not override.
RUN [ "$BUILD_UID" = 0 ] || adduser --gecos 'Speech SDK User' --disabled-password $BUILD_USER --uid $BUILD_UID
USER $BUILD_USER


FROM ubuntu:16.04

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
        python \
        python-dev \
        default-jdk \
        default-jre-headless \
        curl \
        libpcre++-dev

ARG SWIG_VERSION=3.0.12
RUN curl -L http://prdownloads.sourceforge.net/swig/swig-$SWIG_VERSION.tar.gz | tar xvfz - && \
    cd swig-$SWIG_VERSION && \
    ./configure && \
    make -j $(nproc) && \
    make install && \
    cd .. && \
    rm -rf swig-$SWIG_VERSION

# Override at docker build time, as appropriate:
ARG BUILD_UID=0
ARG BUILD_USER=root

# N.B. Root already present, do not override.
RUN [ "$BUILD_UID" = 0 ] || adduser --gecos 'Speech SDK User' --disabled-password $BUILD_USER --uid $BUILD_UID
USER $BUILD_USER

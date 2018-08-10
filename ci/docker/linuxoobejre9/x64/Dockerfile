
FROM ubuntu:16.04

RUN apt-get update && \
    apt-get --yes install \
      build-essential \
      libssl1.0.0 \
      libcurl3 \
      libasound2 \
      wget \
      # test dependencies
      pulseaudio \
      pulseaudio-utils

RUN apt-get update && \
    apt-get --yes install \
      openjdk-9-jre-headless

# Override at docker build time, as appropriate:
ARG BUILD_UID=0
ARG BUILD_USER=root

# N.B. Root already present, do not override.
RUN [ "$BUILD_UID" = 0 ] || adduser --gecos 'Speech SDK User' --disabled-password $BUILD_USER --uid $BUILD_UID
USER $BUILD_USER

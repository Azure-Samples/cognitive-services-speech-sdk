
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

# For testing
RUN apt-get install --yes expect

RUN apt-get update && \
    apt-get install --yes apt-transport-https && \
    wget -q https://packages.microsoft.com/config/ubuntu/16.04/packages-microsoft-prod.deb && \
    dpkg -i packages-microsoft-prod.deb && \
    rm packages-microsoft-prod.deb && \
    apt-get update && \
    apt-get install --yes dotnet-sdk-2.0.0

# Override at docker build time, as appropriate:
ARG BUILD_UID=0
ARG BUILD_USER=root

# N.B. Root already present, do not override.
RUN [ "$BUILD_UID" = 0 ] || adduser --gecos 'Speech SDK User' --disabled-password $BUILD_USER --uid $BUILD_UID
USER $BUILD_USER

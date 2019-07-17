FROM unimrcp:latest

LABEL maintainer="yulin.li@microsoft.com"

RUN apt-get update && apt-get install -y \
    libpoco-dev \
    rapidjson-dev \
    libssl1.0.0 \
    libasound2 \
    && rm -rf /var/lib/apt/lists/*

# Download and extract Microsoft Cognitive Service Speech SDK

ENV SPEECHSDK_ROOT="/usr/local/speechsdk"

RUN mkdir -p "$SPEECHSDK_ROOT" && \
    wget -O SpeechSDK-Linux.tar.gz https://aka.ms/csspeech/linuxbinary && \
    tar --strip 1 -xzf SpeechSDK-Linux.tar.gz -C "$SPEECHSDK_ROOT" && rm SpeechSDK-Linux.tar.gz

ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$SPEECHSDK_ROOT/lib/x64"

# copy ms-plugins

COPY ms-synth /unimrcp-1.6.0/plugins/ms-synth
COPY ms-recog /unimrcp-1.6.0/plugins/ms-recog
COPY ms-common /unimrcp-1.6.0/plugins/ms-common

# copy makefile for ms plugins

COPY configure.ac /unimrcp-1.6.0/configure.ac
COPY Makefile.am /unimrcp-1.6.0/plugins/Makefile.am

# re-build unimrcp

RUN cd /unimrcp-1.6.0 && \
    ./bootstrap && ./configure && make && make install

# rm source code
RUN rm -rf /unimrcp-1.6.0

# run unimrcpserver

WORKDIR /usr/local/unimrcp/bin
CMD ["./unimrcpserver"]
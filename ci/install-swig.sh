#!/usr/bin/env bash
# Run as root.

SWIG_VERSION=3.0.12

curl -L https://github.com/swig/swig/archive/rel-$SWIG_VERSION.tar.gz | tar xfz - && \
cd swig-rel-$SWIG_VERSION && \
./autogen.sh && \
./configure && \
make -j $(nproc) && \
make install && \
cd .. && \
rm -rf swig-$SWIG_VERSION

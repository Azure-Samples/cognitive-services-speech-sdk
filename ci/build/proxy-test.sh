#! /usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

ARTIFACTS_DIR=$1
set -e -x -u
set -o pipefail

proxylog=tinyproxy/tinyproxy.log
mkdir -p tinyproxy
# create log files now, otherwise permissions will be wrong when created inside docker
touch $proxylog
touch tinyproxy/tinyproxy.pid
chmod -R 777 tinyproxy
cp --verbose ci/build/tinyproxy.conf tinyproxy/

cleanup() {
    local exit_code=$?
    tail $proxylog || true
    docker stop tinyproxy || true
    rm -rf tinyproxy
    printf "Exiting with exit code %s\n" $exit_code
    exit $exit_code
}

trap cleanup EXIT

docker run --rm --name tinyproxy -d -v $(pwd)/tinyproxy/:/tinyproxy -p 8888:8888 ubuntu bash -c "set -x && \
    apt update -y && \
    apt install -y tinyproxy && \
    tinyproxy -c /tinyproxy/tinyproxy.conf && tail -f /dev/null"

# wait for proxy to start
sleep 10

architecture=x64
bindir=buildfromdrop$architecture/bin
mkdir -p ${bindir}
cp --verbose "$ARTIFACTS_DIR/Linux/Linux-$architecture/Release/"{public/lib,private/{bin,lib}}/* ${bindir}
chmod 755 ${bindir}/*

# run carbon
carbonx=${bindir}/carbonx
carbonlog=carbonx.log

for img in $DOCKER_IMAGES; do
    # Run carbonx with proxy. The iptables calls block direct HTTP and HTTPS
    # connections, to make sure that alle connections are proxied.
    docker run --volume $(pwd):/carbon --interactive \
        --cap-add=NET_ADMIN \
        --link=tinyproxy:tinyproxy \
        --rm ${img} \
            bash - <<SCRIPT 2>&1 | tee $carbonlog
            set -x -e
            apt update -y
            apt install iptables -y
            iptables -A OUTPUT -p tcp --dport 80 -j REJECT
            iptables -A OUTPUT -p tcp --dport 443 -j REJECT
            LD_LIBRARY_PATH=/carbon/${bindir} /carbon/${carbonx} \
                --speech --input:/carbon/${SPEECHSDK_INPUTDIR}/audio/whatstheweatherlike.wav \
                --subscription:${SPEECHSDK_SPEECH_KEY} --region:${SPEECHSDK_SPEECH_REGION} --single \
                --proxy-host:tinyproxy --proxy-port:8888
SCRIPT
done

# check proxy was actually used
grep "stt.speech.microsoft.com:443" $proxylog

# check STT result
grep -i "what's the weather like?" $carbonlog


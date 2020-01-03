#! /usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

set -e -u
set -o pipefail

MAC_ARTEFACTS_DIR=$1
JAVA_SDK=$2

# settings for mitmproxy
PROXY_HOST=localhost
PROXY_PORT=8080

cleanup() {
    local exit_code=$?
    tail $proxylog || true
    [[ -z $PROXY_PID ]] || kill $PROXY_PID
    printf "Exiting with exit code %s\n" $exit_code
    exit $exit_code
}

pip3 install mitmproxy

test_carbonx() {
    local proxylog=proxy.log
    local proxyout=proxy.out

    trap cleanup EXIT

    mitmdump -w $proxylog &> $proxyout &
    PROXY_PID=$!

    # wait for proxy to start
    sleep 5

    # run carbon
    carbonx=${MAC_ARTEFACTS_DIR}/${BUILDPLATFORM}/${BUILDCONFIGURATION}/private/bin/carbonx
    chmod 755 ${MAC_ARTEFACTS_DIR}/${BUILDPLATFORM}/${BUILDCONFIGURATION}/private/bin/*
    carbonlog=carbonx.log

    LD_LIBRARY_PATH=${MAC_ARTEFACTS_DIR}/${BUILDPLATFORM}/${BUILDCONFIGURATION}/public/lib \
        $carbonx --speech --input:tests/input/audio/whatstheweatherlike.wav \
        --subscription:${SPEECHSDK_SPEECH_KEY} --region:${SPEECHSDK_SPEECH_REGION} --single \
        --proxy-host:$PROXY_HOST --proxy-port:$PROXY_PORT --passthrough-certfile ~/.mitmproxy/mitmproxy-ca-cert.pem \
        2>&1 | tee $carbonlog

    # check proxy was actually used
    grep "stt.speech.microsoft.com:443" $proxyout

    # check STT result
    grep -i "what's the weather like?" $carbonlog
    cleanup
}


test_java_quickstart() {
    local proxylog=proxy.log
    local proxyout=proxy.out

    trap cleanup EXIT

    mitmdump -w $proxylog &> $proxyout &
    PROXY_PID=$!

    # wait for proxy to start
    sleep 5

    # prepare quickstart for proxy
    sed -i '' '/SpeechConfig/a\
                config.setProxy("'${PROXY_HOST}'", '${PROXY_PORT}', "", "");\
                config.setProperty(PropertyId.Speech_LogFilename, "java-quickstart.log");\
                String contents = new String(Files.readAllBytes(Paths.get("'${HOME}'/.mitmproxy/mitmproxy-ca-cert.pem")));\
                config.setProperty("OPENSSL_SINGLE_TRUSTED_CERT", contents);\
                config.setProperty("OPENSSL_SINGLE_TRUSTED_CERT_CRL_CHECK", "false");' \
        public_samples/quickstart/java/jre/from-microphone/src/speechsdk/quickstart/Main.java

    # run recognition from file in quickstart
    sed -i '' '/import com\.microsoft\.cognitiveservices\.speech\.\*;/a\
        import java.nio.file.*;\
        import com.microsoft.cognitiveservices.speech.audio.*;' \
        public_samples/quickstart/java/jre/from-microphone/src/speechsdk/quickstart/Main.java
    sed -i '' '/SpeechRecognizer/s/.*/\
                AudioConfig audioInput = AudioConfig.fromWavFileInput("tests\/input\/audio\/whatstheweatherlike.wav");\
                SpeechRecognizer reco = new SpeechRecognizer(config, audioInput);\
        /' public_samples/quickstart/java/jre/from-microphone/src/speechsdk/quickstart/Main.java

    # build quickstart
    ci/run-maven.sh public_samples/quickstart/java/jre/from-microphone ${MAVENROOT}

    # run modified quickstart
    java -cp public_samples/quickstart/java/jre/from-microphone/target/quickstart-eclipse-1.0.0-SNAPSHOT.jar:${JAVA_SDK} speechsdk.quickstart.Main | tee quickstart.log

    cat java-quickstart.log
    # check proxy was actually used
    grep "stt.speech.microsoft.com:443" $proxyout

    # check STT result
    grep -i "what's the weather like?" quickstart.log
    cleanup
}

FAILED=0
test_carbonx || FAILED=1
test_java_quickstart || FAILED=1

exit $FAILED


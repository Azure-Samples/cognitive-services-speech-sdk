#!/bin/bash

cd "$(dirname "$0")"

echo ======building the docker image of mrcp======

docker build --no-cache -t unimrcp:latest -f ./unimrcp/Dockerfile ..

echo ======building docker image of ms plugins======

docker build --no-cache -t unimrcp_ms:latest -f ./ms-plugins/Dockerfile ..
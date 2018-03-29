#!/bin/bash

set -e

BINARY_DIR=$1
CARBONX=$BINARY_DIR/carbonx
AUDIO_FILE=whatstheweatherlike.wav

echo -e "\n Testing carbon: \n"

ARGS="--speech --subscription:$2 --input $AUDIO_FILE"

echo "Launching: "
(set -x; $CARBONX $ARGS)
echo -e "\n\n\n"

echo "Launching: "
(set -x; $CARBONX $ARGS  --single)
echo -e "\n\n\n"

echo "Launching: "
(set -x; $CARBONX $ARGS --continuous 10)
echo -e "\n\n\n"

echo -e "\n Done\n"

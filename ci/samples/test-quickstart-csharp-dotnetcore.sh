#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -u -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 image-tag nuget-source"
IMAGE_TAG="${1?$USAGE}"
NUGET_SOURCE="${2?$USAGE}"
[[ -d $NUGET_SOURCE ]]

CSPROJ=public_samples/quickstart/csharp-dotnetcore/helloworld/helloworld.csproj
ASSEMBLY=public_samples/quickstart/csharp-dotnetcore/helloworld/bin/Debug/netcoreapp2.0/helloworld.dll

exec "$SCRIPT_DIR/test-dotnetcore.sh" "$IMAGE_TAG" "$NUGET_SOURCE" "$CSPROJ" \
  ./ci/samples/quickstart-e2e.expect tests/input/audio/whatstheweatherlike.wav "What's the weather like?" \
    dotnet "$ASSEMBLY"

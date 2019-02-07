#!/bin/bash
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

CSPROJ=public_samples/samples/csharp/dotnetcore/console/samples/samples.csproj
ASSEMBLY=public_samples/samples/csharp/dotnetcore/console/samples/bin/Debug/netcoreapp2.0/samples.dll

exec "$SCRIPT_DIR/test-dotnetcore.sh" "$IMAGE_TAG" "$NUGET_SOURCE" "$CSPROJ" \
  ./ci/samples/test-samples-csharp-dotnetcore-console.expect \
    tests/input \
    dotnet "$ASSEMBLY"

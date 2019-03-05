#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
set -u -x -e -o pipefail
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/../.."

USAGE="Usage: $0 image-tag nuget-source csproj cmd-relative-to-root..."
IMAGE_TAG="${1?$USAGE}"
NUGET_SOURCE="${2?$USAGE}"
CSPROJ="${3?$USAGE}"
shift 3

[[ -d $NUGET_SOURCE ]]
NUGET_SOURCE_REL=$(realpath --relative-to "$SOURCE_ROOT" "$NUGET_SOURCE")
! [[ $NUGET_SOURCE_REL = ../* ]]

[[ -f $CSPROJ ]]
CSPROJ_REL=$(realpath --relative-to "$SOURCE_ROOT" "$CSPROJ")
! [[ $CSPROJ_REL = ../* ]]

TESTCMD_ESCAPED="$(printf "%q " "$@")"

CSPROJ_REL_ESCAPED=$(printf "%q" "$CSPROJ_REL")
NUGET_SOURCE_REL_ESCAPED=$(printf "%q" "$NUGET_SOURCE_REL")

# Note: all steps in one container instance since otherwise we'd have to
# persist the NuGet package cache.
docker run --rm --interactive \
  --volume "$(readlink -f "$SOURCE_ROOT"):/test" \
  --workdir /test \
  --env DOTNET_CLI_TELEMETRY_OPTOUT=1 \
  --env DOTNET_SKIP_FIRST_TIME_EXPERIENCE=1 \
  "$IMAGE_TAG" \
  bash - <<SCRIPT
set -u -x -e -o pipefail
dotnet restore --no-cache --source $NUGET_SOURCE_REL_ESCAPED $CSPROJ_REL_ESCAPED
dotnet build $CSPROJ_REL_ESCAPED
./ci/samples/wrap-pulseaudio.sh $TESTCMD_ESCAPED
SCRIPT

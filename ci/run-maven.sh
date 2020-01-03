#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

USAGE="Usage: $0 samples-dir [<path-to-maven-repository>]"

SAMPLES_DIR="${1?$USAGE}"

set -e -o pipefail

if [[ -n $2 ]]; then
  PACKAGE_PATH="$2"
  if [[ $OS = "Windows_NT" ]]; then
    PACKAGE_PATH="$(cygpath -aw "$2")"
  fi
else
  PACKAGE_PATH=
fi

callFind=find
[[ $(type -t gfind) != file ]] || callFind=gfind

# We were using process substitution before for find/readarray, which started failing.
# Possibly related to https://github.com/git-for-windows/git/issues/2291, although
# we could not correlate with the software running on the hosted agents.
PROJECTS_FILE=$(mktemp proj-XXXXXX)
on_exit() {
  local exit_code=$?
  rm -f "$PROJECTS_FILE"
  printf "Exiting with exit code %s\n" $exit_code
  exit $exit_code
}
trap on_exit EXIT

${callFind} "$SAMPLES_DIR" -name pom.xml -printf '%h\n' > "$PROJECTS_FILE"

readarray -t PROJECTS < "$PROJECTS_FILE"

for dir in "${PROJECTS[@]}"; do
  echo $dir
  pushd "$dir"
  [[ -e pom.xml ]]

  # Patch in local repo if package path specified

  [[ -n $PACKAGE_PATH ]] && {
    perl -i -lpe 'BEGIN { ($s, $r) = splice @ARGV, 0, 2; $s = qr/\Q$s/ } s/$s/$r/g' \
      https://csspeechstorage.blob.core.windows.net/maven/ \
      "file:///${PACKAGE_PATH//\\/\/}/" \
      pom.xml
  }

  # Cygwin / Git bash perl can't in-place edit w/o backup
  rm -f pom.xml.bak

  mvn package

  popd
done

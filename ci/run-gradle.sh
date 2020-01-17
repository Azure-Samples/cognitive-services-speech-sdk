#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

USAGE="Usage: $0 [<path-to-maven-repository>]"

if [[ -n $1 ]]; then
  PACKAGE_PATH="$(cygpath -aw "$1")"
else
  PACKAGE_PATH=
fi

echo $PACKAGE_PATH

set -e -o pipefail

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

find . -name gradlew.bat -printf '%h\n' > "$PROJECTS_FILE"

readarray -t PROJECTS < "$PROJECTS_FILE"

for dir in "${PROJECTS[@]}"; do
  echo $dir
  pushd "$dir"
  [[ -e gradlew.bat ]]
  [[ -e build.gradle ]]

  # Patch in local repo if package path specified

  [[ -n $PACKAGE_PATH ]] && {

  cat >> build.gradle <<MAVEN
allprojects {
    repositories {
        maven {
            url '${PACKAGE_PATH//\\/\\\\}'
        }
    }
}
MAVEN
    }

    ./gradlew assemble

  popd
done

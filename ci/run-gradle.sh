#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

USAGE="Usage: $0 samples-dir [<path-to-maven-repository>]"

SAMPLES_DIR="${1?$USAGE}"

if [[ -n $1 ]]; then
  PACKAGE_PATH="$(cygpath -aw "$2")"
else
  PACKAGE_PATH=
fi

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

. "$SCRIPT_DIR/functions.sh" || exit 1

set -u -e -o pipefail

readarray -t PROJECTS < <(find "$SAMPLES_DIR" -name gradlew.bat -printf '%h\n')

# Allowing for a few number of (global) retries to accomodate for downloads
# failures (filling up the package cache).
MAX_RETRIES=2
RETRY=0

ERRORS=0
for dir in "${PROJECTS[@]}"; do
  pushd "$dir" > /dev/null
  [[ -e gradlew.bat ]] ||
    exitWithError 'Error: cannot find gradlew.bat in directory %s\n' "$dir"

  [[ -e build.gradle ]] ||
    exitWithError 'Error: cannot find build.gradle in directory %s\n' "$dir"

  echo === Trying to build directory $dir.

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

    while ((++RETRY <= MAX_RETRIES)); do
      if ./gradlew assemble; then
        popd 1> /dev/null
        continue 2
      else
        message="'gradlew assemble' failed for directory '$dir', global retry $RETRY/$MAX_RETRIES."
        vsts_logissue warning "$message"
        echo Warning: $message
        sleep 10
      fi
    done

    ./gradlew assemble

    [[ $? == 0 ]] || {
      message="run-gradle.sh: 'gradlew assemble' failed for directory '$dir', no more global retries."
      vsts_logissue error "'gradlew assemble' failed for directory '$dir', no more global retries."
      echo Error: $message
      ((++ERRORS))
    }
  popd 1> /dev/null
done

[[ $ERRORS == 0 ]] || {
  echo Error: not all builds successful.
  exit 1
}

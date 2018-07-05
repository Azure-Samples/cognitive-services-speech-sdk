#!/bin/bash

USAGE="Usage: $0 <path-to-maven-repository>"
PACKAGE_PATH="${1?$USAGE}"
PACKAGE_PATH="$(cygpath -aw "$PACKAGE_PATH")"
echo $PACKAGE_PATH
#SLN_PATH="${2?$USAGE}"

set -e -x -o pipefail

readarray -t PROJECTS < <(find . -name gradlew.bat -printf '%h\n')

for dir in "${PROJECTS[@]}"; do
  echo $dir
  pushd "$dir"
  [[ -e gradlew.bat ]]
  [[ -e build.gradle ]]

  # Patch in local repo
  cat >> build.gradle <<MAVEN
allprojects {
    repositories {
        maven {
            url '${PACKAGE_PATH//\\/\\\\}'
        }
    }
}
MAVEN

    ./gradlew assemble

  popd
done

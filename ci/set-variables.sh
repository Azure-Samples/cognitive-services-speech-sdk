#!/bin/bash
# Set variables based on source commit and requested options.
# This is only run in VSTS currently.
#
# Inputs:
# * version.txt in the repository directory
# * Output overrides named OVERRIDE_* coming from the build definition (cf. below)
# * Various VSTS build variables
#   * $BUILD_BUILDID
#   * $BUILD_REASON
#   * $BUILD_SOURCEBRANCH
#   * $SYSTEM_COLLECTIONID
#   * $SYSTEM_DEFINITIONID
#   * $SYSTEM_PULLREQUEST_PULLREQUESTID
#
# Outputs:
# * SPEECHSDK_MAIN_BUILD - equal to "true" if running from our main build
#   definition (4833), "false" otherwise. TODO no need to export?
# * SPEECHSDK_BUILD_TYPE - can be "dev", "int", "prod", which (roughly) correspond to
#   dev-box / PR / feature-branch, nightly, release-branch builds. "int" and "prod"
#   can only come from the main build definition (i.e., no clone, and no draft).
# * SPEECHSDK_SEMVER2 - semver 2 version without build meta data (commit ID)
#   The version number is determined from the contents of the version.txt
#   in the repository root, as well as the build type. The build types
#   dev and int correspond to alpha and beta prerelease version (the associated is the
#   VSTS build number). For prod builds, version.txt must fully specify the
#   version (without build meta data). For releases, it is expected that a
#   release branch is created and version.txt is edited on the path to the
#   release.
#   For example: release/0.3.0 is created, version.txt contents are changed to
#   "0.3.0-rc1", and further along to "0.3.0-rc2", and finally to "0.3.0" from which the
#   release can be created.
# * SPEECHSDK_VERSION_CODE - (Android) version code, an integer that linearly
#   orders versions for production builds. (Currently used in the Android Library, not sure
#   if it's strictly required there)
# * SPEECHSDK_SEMVER2NOMETA - same as above, without build meta data
#   We are currently using this (and not SPEECHSDK_SEMVER2) for NuGet packages,
#   since VSTS package management does not support build meta information.
# * SPEECHSDK_SIGN - equal to "true" if should sign, "false" otherwise.
# * SPEECHSDK_SDL_ALL - equal to "true" if SDL build should also run long-running tasks.
#   Note: Needs SPEECHSDK_SPECTRE_MITIGATION set to "true" to pass in binskim.
# * SPEECHSDK_SPECTRE_MITIGATION - equal to "true" if should build with Spectre mitigations, "false" otherwise.
#   Note: on Windows, requires VS component install Microsoft.VisualStudio.Component.VC.Runtimes.x86.x64.Spectre.
# * SPEECHSDK_NUGET_VSTS_PUSH - equal to "true" if a push to (one of) our internal
#   VSTS packagement feeds should be made.
# * SPEECHSDK_VSTS_FEED - VSTS feed to push to (if a push is done)
#     Defaults to our CarbonPre VSTS feed for 'dev' builds, Carbon otherwise.
# * SPEECHSDK_BUILD_AGENT_PLATFORM - can be "Windows-x64", "OSX-x64", "Linux-x64"
# * SPEECHSDK_BUILD_PHASES - space-separated and space-enclosed list of build phases to run
#     Default: " WindowsBuild WindowsUwpBuild NuGet NuGetLinuxTest LinuxBuild LinuxDockerBuild LinuxDrop OsxBuild IosBuild AndroidBuild AndroidPackage Doxygen JavaJrePackage JavaJrePackageLinuxTest JsBuild WindowsSdlBuild LinuxPythonBuild LinuxPythonOobeTest WindowsPythonBuild OsxPythonBuild BuildPythonDocs "
#     For int (nightly) builds, "TsaUpload WindowsSDLFortifyJava WackTest CheckSignatures" are added to the default phases.
#     For prod (release) builds, "WackTest CheckSignatures" are added to the default phases.
#     Check phase condition in build.yml for valid phase names.
# * SPEECHSDK_RUN_TESTS - whether to run tests. Can be 'true' (default) or 'false'.
#
# Override mechanism:
# Any of the outputs can be overridden. To override output X, specify a
# variable OVERRIDE_X with the desired override value. Note this isn't exposed
# at the build definition level, but it could be (in important enough cases),
# without source change. Overrides only happen before outputting and do not
# change the flow of the script.
#

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/.."

# Get some helpers
. "$SCRIPT_DIR/functions.sh"

set -x -e -o pipefail

# Determine build agent platform
case $(uname -a) in
  Linux*x86_64\ GNU/Linux)
    SPEECHSDK_BUILD_AGENT_PLATFORM=Linux-x64
    ;;
  MINGW64_NT-*x86_64\ Msys|MSYS_NT-10.0-WOW*i686\ Msys)
    # Note: second pattern matches the architecture of the UnityBuild build agent.
    SPEECHSDK_BUILD_AGENT_PLATFORM=Windows-x64
    ;;
  Darwin*\ x86_64)
    SPEECHSDK_BUILD_AGENT_PLATFORM=OSX-x64
    ;;
  *)
    echo Unexpected build agent platform:
    uname -a
    exit 1
    ;;
esac

VERSION="$(cat "$SCRIPT_DIR/../version.txt")"

# Must be a major.minor.patch version
echo VERSION=$VERSION

# Determine the build type, ID, and commit

SPEECHSDK_BUILD_TYPE=dev

IN_VSTS=$([[ -n $SYSTEM_DEFINITIONID && -n $SYSTEM_COLLECTIONID ]] && echo true || echo false)

if $IN_VSTS; then
  # We're running in VSTS

  # Note: build template expressions we are using elsewhere (e.g.,
  #   linux-docker-build.yml) duplicate the logic of determining build types. Both
  #   definitions should be kept in sync; build template expression cannot
  #   reuse the logic here.

  MAIN_BUILD_DEFS=,19422243-19b9-4d85-9ca6-bc961861d287/4833,

  SPEECHSDK_MAIN_BUILD=$([[ $MAIN_BUILD_DEFS == *,$SYSTEM_COLLECTIONID/$SYSTEM_DEFINITIONID,* ]] && echo true || echo false)

  if $SPEECHSDK_MAIN_BUILD; then
    # Non-draft build definition

    if [[ $BUILD_SOURCEBRANCH == refs/heads/release/* ]]; then
      SPEECHSDK_BUILD_TYPE=prod
    elif [[ $BUILD_SOURCEBRANCH == refs/heads/master && ( $BUILD_REASON == Schedule || $BUILD_REASON == Manual ) ]]; then
      SPEECHSDK_BUILD_TYPE=int
    fi
  fi
  if [[ $BUILD_REASON == PullRequest ]]; then
    _BUILD_COMMIT=pr$SYSTEM_PULLREQUEST_PULLREQUESTID
  else
    _BUILD_COMMIT=
  fi
  _BUILD_COMMIT+=$(echo $BUILD_SOURCEVERSION | cut -c 1-8)
  _BUILD_ID=$BUILD_BUILDID
else
  # Dev box
  _BUILD_ID=$(date -u +%Y%m%d%H%M%S)
  _BUILD_COMMIT=$(git rev-parse --short HEAD)
  SPEECHSDK_MAIN_BUILD=false
fi

if [[ $SPEECHSDK_BUILD_TYPE != prod && ! $VERSION =~ ^([0-9]+\.){2}[0-9]+$ ]]; then
   echo Invalid version, should be MAJOR.MINOR.PATCH: $VERSION
   exit 1
elif [[ ! $VERSION =~ ^([0-9]+\.){2}[0-9]+(-(alpha|beta|rc)\.[0-9]+)?$ ]]; then
   echo Invalid version, should be MAJOR.MINOR.PATCH with optional alpha/beta/rc pre-release: $VERSION
  exit 1
fi

# Must succeed with checks above:
[[ $VERSION =~ ^([0-9]+)\.([0-9]+)\.([0-9]+)(-(alpha|beta|rc)\.([0-9]+))?$ ]]

if [[ $SPEECHSDK_BUILD_TYPE == prod ]]; then
  # Note: The greatest value Google Play allows for versionCode is 2100000000.
  # We assume we are bound to this below.
  # Version code schema: 1AAABBCCDD
  # where
  #  * AAA - MAJOR
  #  * BB - MINOR
  #  * CC - PATCH
  #  * DD - following encoding:
  #    * 1..30 for alpha-DD,
  #    * 31..60 for beta-(DD - 30),
  #    * 61..90 for rc-(DD - 60),
  #    * 99 for final (no pre-release tag)
  MAJOR_VERSION="${BASH_REMATCH[1]}"
  MINOR_VERSION="${BASH_REMATCH[2]}"
  PATCH_VERSION="${BASH_REMATCH[3]}"

  if [[ -n ${BASH_REMATCH[4]} ]]; then
    case ${BASH_REMATCH[5]} in
      alpha)
        LAST_OFFSET=$((0 + ${BASH_REMATCH[6]}))
        ;;
      beta)
        LAST_OFFSET=$((30 + ${BASH_REMATCH[6]}))
        ;;
      rc)
        LAST_OFFSET=$((60 + ${BASH_REMATCH[6]}))
        ;;
    esac
  else
    # Final
    LAST_OFFSET=99
  fi
  SPEECHSDK_VERSION_CODE=$((
    1000000000 +
    1000000 * $MAJOR_VERSION +
    10000 * $MINOR_VERSION +
    100 * $PATCH_VERSION +
    $LAST_OFFSET))
else
  # These are non-official builds. Pick something based on build ID, plus the
  # type (int or dev). This doesn't need to be perfect, but at least, nightly
  # (timed int builds) builds should order well wrt. themselves and dev builds.
  if [[ $SPEECHSDK_BUILD_TYPE == int ]]; then
    SPEECHSDK_VERSION_CODE=$((500000000 + $_BUILD_ID))
  else
    # dev
    SPEECHSDK_VERSION_CODE=$_BUILD_ID
  fi
fi

# Build phases to run
SPEECHSDK_BUILD_PHASES=" WindowsBuild WindowsUwpBuild NuGet NuGetLinuxTest LinuxBuild LinuxDockerBuild LinuxDrop OsxBuild IosBuild AndroidBuild AndroidPackage Doxygen JavaJrePackage JavaJrePackageLinuxTest JsBuild WindowsSdlBuild LinuxPythonBuild LinuxPythonOobeTest WindowsPythonBuild OsxPythonBuild BuildPythonDocs UnityBuild "

# Running tests is default
SPEECHSDK_RUN_TESTS=true

case $SPEECHSDK_BUILD_TYPE in
  dev)
    PRERELEASE_VERSION=-alpha.0.$_BUILD_ID
    META=+$_BUILD_COMMIT
    SPEECHSDK_SIGN=false
    SPEECHSDK_NUGET_VSTS_PUSH=false
    SPEECHSDK_VSTS_FEED=6e3392a0-60e1-412f-8fc5-41de1c818f6c # CarbonPre
    SPEECHSDK_SPECTRE_MITIGATION=false
    SPEECHSDK_SDL_ALL=false
    ;;
  int)
    # TSA Upload only for Nightly, not for manually scheduled
    if [[ $BUILD_REASON == Schedule ]]; then
      SPEECHSDK_BUILD_PHASES+="TsaUpload WindowsSDLFortifyJava WackTest IosMultiPlatformTests "
    fi
    PRERELEASE_VERSION=-beta.0.$_BUILD_ID
    META=+$_BUILD_COMMIT
    SPEECHSDK_SIGN=true
    SPEECHSDK_NUGET_VSTS_PUSH=true
    SPEECHSDK_VSTS_FEED=6cad1ef4-30c7-40bd-8d67-d624d756c9c8 # Carbon
    SPEECHSDK_SPECTRE_MITIGATION=true
    SPEECHSDK_SDL_ALL=true
    ;;
  prod)
    # Prod builds take exactly the version from version.txt, no extra
    # pre-release or meta.
    SPEECHSDK_BUILD_PHASES+="WackTest "
    PRERELEASE_VERSION=
    META=
    SPEECHSDK_SIGN=true
    SPEECHSDK_NUGET_VSTS_PUSH=true
    SPEECHSDK_VSTS_FEED=6cad1ef4-30c7-40bd-8d67-d624d756c9c8 # Carbon
    SPEECHSDK_SPECTRE_MITIGATION=true
    SPEECHSDK_SDL_ALL=true
    ;;
esac

if $SPEECHSDK_SIGN; then
  SPEECHSDK_BUILD_PHASES+="CheckSignatures "
fi

# Set the _output_ variables used for conditionally running phases.

SPEECHSDK_SEMVER2NOMETA="$VERSION$PRERELEASE_VERSION"
SPEECHSDK_SEMVER2="$SPEECHSDK_SEMVER2NOMETA$META"

# Override suffix
if [[ $BUILD_REASON == PullRequest ]]; then
  _OVERRIDE_SUFFIX=_PR$SYSTEM_PULLREQUEST_PULLREQUESTID
else
  _OVERRIDE_SUFFIX=
fi

set +x

# Note: VSTS package management does not (yet?) support build meta, so upstream
# build definition will pickup SPEECHSDK_SEMVER2NOMETA for the NuGet.

for var in \
  SPEECHSDK_BUILD_AGENT_PLATFORM \
  SPEECHSDK_BUILD_TYPE \
  SPEECHSDK_MAIN_BUILD \
  SPEECHSDK_NUGET_VSTS_PUSH \
  SPEECHSDK_RUN_TESTS \
  SPEECHSDK_SEMVER2 \
  SPEECHSDK_SEMVER2NOMETA \
  SPEECHSDK_SIGN \
  SPEECHSDK_SPECTRE_MITIGATION \
  SPEECHSDK_SDL_ALL \
  SPEECHSDK_VERSION_CODE \
  SPEECHSDK_VSTS_FEED \
  ; \
do
  overrideVar=OVERRIDE_${var}${_OVERRIDE_SUFFIX}
  overrideValue="${!overrideVar}"

  [[ -n $overrideValue ]] && echo Picking override: $overrideVar=$overrideValue

  vsts_setvar $var "${overrideValue:-${!var}}"
done

# Some outputs are (cross-phase) outputs. Potentially all could / should be, still testing this.
for var in \
  SPEECHSDK_BUILD_PHASES \
  ; \
do
  overrideVar=OVERRIDE_${var}${_OVERRIDE_SUFFIX}
  overrideValue="${!overrideVar}"

  [[ -n $overrideValue ]] && echo Picking override: $overrideVar=$overrideValue

  vsts_setoutvar $var "${overrideValue:-${!var}}"
done


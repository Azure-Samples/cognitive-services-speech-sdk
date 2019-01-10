set -e -x

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
XCODE_CONFIGURATION_BUILD_DIR=${PWD}/iostestbuild

run_test() {
    local name=$1
    local os=$2
    local extraargs=$3

    local simname="${name}-${os}"
    ${SCRIPT_DIR}/ios_simulator_cli.sh ${simname} create "${name}" "${os}"
    uid=$(${SCRIPT_DIR}/ios_simulator_cli.sh ${simname} getuid)

    if [[ -n ${USEGUI} ]]; then
        ${SCRIPT_DIR}/ios_simulator_cli.sh ${simname} open
    else
        ${SCRIPT_DIR}/ios_simulator_cli.sh ${simname} boot
    fi

    rm -rf ${XCODE_CONFIGURATION_BUILD_DIR}

    xcodebuild build-for-testing -project ${PROJECT} ${extraargs}\
        -destination "platform=iOS Simulator,id=${uid}" \
        -scheme ${SCHEME} \
        CONFIGURATION_BUILD_DIR=${XCODE_CONFIGURATION_BUILD_DIR} \
        SUBSCRIPTION_KEY="$SPEECHSDK_SPEECH_KEY" SERVICE_REGION="$SPEECHSDK_SPEECH_REGION"

    ${SCRIPT_DIR}/ios_simulator_cli.sh ${simname} wait
    xcrun simctl install ${uid} ${XCODE_CONFIGURATION_BUILD_DIR}/${APPNAME}.app/

    xcodebuild test-without-building -project ${PROJECT} ${extraargs}\
        -destination "platform=iOS Simulator,id=${uid}" \
        -scheme ${SCHEME} \
        CONFIGURATION_BUILD_DIR=${XCODE_CONFIGURATION_BUILD_DIR} \
        SUBSCRIPTION_KEY="$SPEECHSDK_SPEECH_KEY" SERVICE_REGION="$SPEECHSDK_SPEECH_REGION" 2>&1 |
          tee "$(OutputDirectory)/logs/xcodebuild-ios-${testname}.log" |
          xcpretty --report junit --output test-ios-${testname}.xml

    ${SCRIPT_DIR}/ios_simulator_cli.sh ${simname} shutdown

    if [[ -n ${USEGUI} ]]; then
        osascript -e 'quit app "Simulator"'
    fi
}

PROJECT=$1  # Xcode project file
SCHEME=$2  # Xcode scheme name
APPNAME=$3  # name of the app (without .app extension)
TEST_NAME=$4  # name of the kind of test (unittest/quickstart) to use for naming the logfile
USEGUI=$5  # non-empty if the Xcode simulator needs to be started in GUI mode

if [[ -n ${USEGUI} ]]; then
    osascript -e 'quit app "Simulator"'
fi

run_test iPhone-X iOS-12-1
# run_test iPhone-8 iOS-11-4  # not installed in Xcode 10.1 toolchain
# run_test iPhone-6 iOS-10-2  # simulator times out, unrelated to test suite
# run_test iPhone-5 iOS-10-2  # simulator times out, unrelated to test suite
run_test iPhone-6s iOS-10-0
run_test iPhone-6 iOS-10-0
run_test iPhone-5 iOS-10-0
run_test iPhone-6 iOS-9-3
run_test iPhone-5 iOS-9-3 'ARCHS="i386" IPHONEOS_DEPLOYMENT_TARGET=9.2'
run_test iPhone-6 iOS-9-2
run_test iPhone-5 iOS-9-2 'ARCHS="i386" IPHONEOS_DEPLOYMENT_TARGET=9.2'


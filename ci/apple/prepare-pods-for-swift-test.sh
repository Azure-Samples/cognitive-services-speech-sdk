#! /usr/bin/env bash

set -e -x -u -o pipefail

USAGE="USAGE: prepare-pods-for-swift-test.sh <testdir> <frameworkdir>"
testdir="${1?$USAGE}"  # Xcode project file
frameworkdir="${2?$USAGE}"  # Xcode scheme name
frameworkdir=$(realpath $frameworkdir)

podfile=$testdir/Podfile
frameworkname=MicrosoftCognitiveServicesSpeech.framework
podspec_template=source/bindings/objective-c/cocoapods/MicrosoftCognitiveServicesSpeech-iOS.podspec

# zip framework
zip --symlinks -r $frameworkdir/framework.zip $frameworkdir/$frameworkname

# copy podspec file to build dir
cp $podspec_template $frameworkdir

# provision podspec file
# - version
# - source path
# - sha
sed -i '' "s/{{{MAJOR\.MINOR\.PATCH}}}/$(cat version.txt)a/ ; \
    s@https://csspeechstorage\.blob\.core\.windows\.net/drop/#{s\.version\.to_s}/MicrosoftCognitiveServicesSpeech-iOS-#{s\.version.to_s}\.zip@'file:' + __dir__ + '/framework.zip'@ ; \
    s@{{{BINARY_SHA}}}@$(sha256sum ${frameworkdir}/framework.zip)@" \
    ${frameworkdir}/MicrosoftCognitiveServicesSpeech-iOS.podspec

# provision Podfile in test dir
sed -i '' "s@{{{PODSPEC_PATH}}}@${frameworkdir}@" $podfile

# install the pod
pod install --project-directory=$testdir


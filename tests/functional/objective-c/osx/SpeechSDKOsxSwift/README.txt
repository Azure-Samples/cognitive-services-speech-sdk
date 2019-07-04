## Set up Cocoapods

These tests use Cocoapods. To run them locally, the location of the
MicrosoftCognitiveServicesSpeech.framework needs to be set with the command
(from the repo root):

```
ci/apple/prepare-pods-for-swift-test.sh tests/functional/objective-c/osx/SpeechSDKOsxSwift $FrameworkDir
```

where `$FrameworkDir` contains the MicrosoftCognitiveServicesSpeech.framework
for macOS. Note that this might make a copy of the framework, so changes to the
framework might not be reflected in the tests immediately.

## Set up environment variables

Subscription information can either be set in Xcode by editing the
SpeechSDKSwift scheme, or programmatically by editing
`SpeechSDKOsxSwift/SpeechSDKOsxSwift.xcodeproj/xcshareddata/xcschemes/SpeechSDKOsxSwift.xcscheme`.

## Set team identifier

A development team id must be set in Xcode or in
`SpeechSDKOsxSwift.xcodeproj/project.pbxproj` for the tests to run.


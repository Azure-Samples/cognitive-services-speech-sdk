This is a sample project used for testing the CocoaPods installation for the Objective-C Speech SDK in an iOS app. The CocoaPods installation is dictated by the 'Podfile' found in this directory.

The checked-in version of the sample project doesn't have any pod installed.

How to install the Speech SDK Framework as a CocoaPod:
1. Open the Microsoft-CognitiveServices-Speech-for-iOS.podspec file available here: '../../../../../source/bindings/objective-c/cocoapods/Microsoft-CognitiveServices-Speech-for-iOS.podspec'.
2. Make sure that a .zip file containing the Speech SDK framework that will be installed in the sample project is available at the URL mentioned as source in the podspec.
3. Open a terminal at the directory where the 'Podfile' is located inside of the sample project.
4. Run 'pod install' - this should download the .zip file and install the Speech SDK framework as part of the sample project,
5. From now on only the .xcworkspace needs to be used to open the solution.
6. In case you need to remove the installed CocoaPods you can run the 'pod deintegrate' followed by the 'pod clean' commands. These should be run before check-in so the sample project is left in the same state as before.
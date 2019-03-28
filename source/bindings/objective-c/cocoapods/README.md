# iOS/macOS deployment of Cocoapods

This directory contains template for releasing the iOS/macOS frameworks as CocoaPods.

Steps (tbc.):

- register the session with the spec repo: https://guides.cocoapods.org/terminal/commands.html#pod_trunk_register
- fill in version information for the placeholder in the .podspec files
- fill in the sha for the released .zip files for the placeholder in the .podspec files
- upload the binary blobs to the blob storage
- run `pod spec lint <podspec file>` on both files to check for errors
- run `pod trunk push <podspec file>` on both files to submit the podspecs


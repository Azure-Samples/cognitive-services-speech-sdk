# Directory to hold the UWP OpenSSL library

The library to be placed into this directory is created as a build artifact in our [Yml - Carbon UWP openssl build](https://msasg.visualstudio.com/DefaultCollection/Skyman/_build?definitionId=8520&_a=summary).

To retrieve the artifact from Azure DevOps, go to the following URL, replacing `BUILD_ID` with the number from the [build\_id.txt](build_id.txt) file in this directory:

    https://msasg.visualstudio.com/DefaultCollection/Skyman/_build/results?buildId=BUILD_ID

Download the build artifact called "speechsdk-static-release" as a .zip file and extract its content into this directory such that an `include` and a `lib` folder exist within this directory.

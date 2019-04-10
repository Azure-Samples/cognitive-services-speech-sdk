# Directory to hold the Android OpenSSL library

The library to be placed into this directory is created as a build artifact in our [Yml - Carbon Android Dependencies](https://msasg.visualstudio.com/DefaultCollection/Skyman/_build?definitionId=7260&_a=summary).

To retrieve the artifact from Azure DevOps, go to the following URL, replacing `BUILD_ID` with the number from the [build\_id.txt](build_id.txt) file in this directory:

    https://msasg.visualstudio.com/DefaultCollection/Skyman/_build/results?buildId=BUILD_ID

Download the build artifact called "TODO" as a .zip file and extract its content into this directory such that for each target architecture (`arm64-v8a`, `armeabi-v7a`, etc.) a folder exists within this directory.

# Directory to hold the GStreamer Android binaries

The library to be placed into this directory is created as a build artifact in our [specific build of Yml - Carbon downloader](https://msasg.visualstudio.com/DefaultCollection/Skyman/_build/results?buildId=5239893).

To retrieve the artifact from Azure DevOps, go to the following URL, replacing `BUILD_ID` with the number from the [build\_id.txt](build_id.txt) file in this directory:

    https://msasg.visualstudio.com/DefaultCollection/Skyman/_build/results?buildId=BUILD_ID

Download the build artifact called "gstreamer-android" as a .zip file and extract its content into this directory such that an `arm`, `arm64`, `armv7`, `x86` and `x86_64` folder exist within this directory.

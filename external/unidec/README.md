# Directory to hold the Unidec runtime library and offline model files

Files to be placed into this directory:

* UnidecRuntime distribution
* Offline language model (en-US)

Note: Hosam (hosamk) and Mahadevan (masrini) are our contacts for the UnidecRuntime builds.

1. Download the default UnidecRuntime nupkg (contains the common UnidecRuntime.h and the library for Windows-x64) from

    https://msasg.visualstudio.com/Skyman/_packaging?_a=package&feed=Bing&package=Richland.Speech.UnidecRuntime&protocolType=NuGet&version=VERSION_ID

	where `VERSION_ID` is the version number for package `Richland.Speech.UnidecRuntime` from the [packages.config](../../source/extensions/embedded_sr/unidec/packages.config) file.

	Create a `Richland.Speech.UnidecRuntime` folder in this directory and extract the contents of the nupkg there (using e.g. 7-Zip).

2. Download the offline model nupkg from

    https://msasg.visualstudio.com/Skyman/_packaging?_a=package&feed=CarbonOfflineModel&package=Unidec.Model&protocolType=NuGet&version=VERSION_ID

	where `VERSION_ID` is the version number for package `Unidec.Model` from the [packages.config](../../source/extensions/embedded_sr/unidec/packages.config) file.

	Create a `Unidec.Model` folder in this directory and extract the contents of the nupkg there.

3. (Optional) Download UnidecRuntime build artifacts (libraries) for Linux/Android target platforms from

    https://msasg.visualstudio.com/Bing_and_IPG/_build/results?buildId=BUILD_ID

	(from the [Richland-Linux-Android-Build-Release](https://msasg.visualstudio.com/Bing_and_IPG/_build?definitionId=6702) pipeline)

	where `BUILD_ID` is the number from the [build\_id.txt](build_id.txt) file in this directory.

	Download the build artifacts as .zip files that match the operating system and platform you want to build for. For example:

    * `linux-aarch64-platforms.zip` for Linux-arm64
    * `linux-amd64-platform.zip` for Linux-x64

	Extract the contents of a zip into the `Richland.Speech.UnidecRuntime` folder so that a subfolder for the target platform is created there.

The final directory structure must look like below (optional Linux-arm64 and Linux-x64 target platform folders are shown for clarity - any additional folders don't matter):

  ```
  external\unidec
  +---Richland.Speech.UnidecRuntime
  |   +---linux-aarch64-platforms
  |   |   +---bin
  |   |   \---lib
  |   +---linux-amd64-platform
  |   |   +---bin
  |   |   \---lib
  |   \---native
  \---Unidec.Model
      \---model
          \---en-US
  ```

Configure your workspace as usual with cmake, but add the following parameter(s):
* `-DEXTENSION_UNIDEC_ENABLED=true` to enable Unidec extension building (required)
* `-DSPEECHSDK_TARGET_PLATFORM=Linux-arm64` if building for Linux-arm64

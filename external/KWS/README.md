# Directory to hold the VoiceActivation KWS library

The library to be placed into this directory is created as a build artifact by the [VoiceActivation build](https://msasg.visualstudio.com/DefaultCollection/Skyman/_build?definitionId=5811&_a=summary) for the [VoiceActivation repository](https://msasg.visualstudio.com/DefaultCollection/Skyman/_git/f37b18b2-460d-4955-af8b-d22e2e5a5920?version=GBmaster).

> Note: Hosam (hosamk) and Mahadevan (masrini) are our contacts for the VoiceActivation repository.

To retrieve the artifact from Azure DevOps, go to the following URL, replacing `BUILD_ID` with the number from the [build\_id.txt](build_id.txt) file in this directory:

    https://msasg.visualstudio.com/DefaultCollection/Skyman/_build/results?buildId=BUILD_ID

Download the build artifacts as .zip files that match the operating system and platform you want to build for.
Make sure to download Debug and Release flavors.
Extract the archives' content into this directory, so folders for operating system, architecture, and build configuration are created in this directory.

For example, to build for Windows x64:

* Download the artifacts named `Windows_Win64_Debug` and `Windows_Win64_Release` from the build page.
* Extract them here so the directory structure looks like this:

  ```
  external\KWS
  +---Windows_Win64_Debug
  |   +---targets
  |   |   +---CortanaSDK
  |   |   |   \---Debug
  |   |   \---DDK
  |   |       \---Debug
  |   \---tools
  |       \---kwsrunner
  |           \---Debug
  \---Windows_Win64_Release
      +---targets
      |   +---CortanaSDK
      |   |   \---Release
      |   \---DDK
      |       \---Release
      \---tools
          \---kwsrunner
              \---Release
  ```

 * Configure your workspace as usual with cmake, adding the additional parameter "-DEXTENSION_KWS_ENABLED=true" to enable extension building.

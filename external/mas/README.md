# Directory to hold the Pasco RNN-T library and offline model files

Files to be placed in this directory:
- RNN-T library
- RNN-T offline model

> Note: Hosam (hosamk) and Mahadevan (masrini) are our contacts for the Pasco repository.

1. The library to be placed into this directory is created as a build artifact by [Pasco CI build](https://msasg.visualstudio.com/Skyman/_build?definitionId=14072&_a=summary) for the [Pasco repository](https://msasg.visualstudio.com/Skyman/_git/Pasco?path=%2F&version=GBmaster&_a=contents).

    To retrieve the artifact from Azure DevOps, go to the following URL, replacing `BUILD_ID` with the number from the [build\_id.txt](build_id.txt) file in this directory:

    https://msasg.visualstudio.com/Skyman/_build/results?buildId=BUILD_ID

    Download the build artifact as a .zip file. Extract the archive's content into this directory, so that the folders for operating system, architecture, and build configuration are created in this directory.

2. Download the offline model NuGet package from

    https://msasg.visualstudio.com/Skyman/_packaging?_a=package&feed=CarbonOfflineModel&package=RNNT.Model&protocolType=NuGet&version=VERSION_ID

    where `VERSION_ID` is the version number for package `RNNT.Model` from the [packages.config](../../source/extensions/embedded_sr/rnnt/packages.config) file.

    Create a `RNNT.Model` directory in this directory and extract the contents for NuGet package there.

The final directory structure should look like following:

```
external\mas
+---Android
+---iOS
+---Linux
+---OSX
+---RNNT.Model
|   \---model
+---Windows
    +---ARM
    |   +---Debug
    |   \---Release
    \---ARM64
    |   +---Debug
    |   \---Release
    \---Win32
    |   +---Debug
    |   \---Release
    \---x64
        +---Debug
        \---Release
```

 * Configure your workspace as usual with cmake, adding the additional parameter `-DEXTENSION_RNNT_ENABLED=true` to enable extension building.

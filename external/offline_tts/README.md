# Directory to hold the offline TTS engine library

The library to be placed into this directory is created as a build artifact by the [TextToSpeech engine build](https://msasg.visualstudio.com/TextToSpeech/_build?definitionId=12307&_a=summary) for the [TextToSpeechMain repository](https://msasg.visualstudio.com/TextToSpeech/_git/TextToSpeechMain).


To retrieve the artifact from Azure DevOps, go to the following URL, replacing `BUILD_ID` with the number from the [build\_id.txt](build_id.txt) file in this directory:

    https://msasg.visualstudio.com/TextToSpeech/_build/results?buildId=BUILD_ID

Download the build artifacts as .zip files that match the operating system and platform you want to build for.
Extract the archives' content into this directory, so folders for operating system, architecture, and build configuration are created in this directory.

 * Configure your workspace as usual with cmake, adding the additional parameter "-DEXTENSION_HYBRID_TTS_ENABLED=true" to enable extension building.

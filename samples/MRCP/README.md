# Sample: Using Microsoft Cognitive Speech Service in UniMRCP

This sample demonstrates how to use cognitive speech service (both SR and TTS) in [UniMRCP](http://www.unimrcp.org/) plugins.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* An Ubuntu 18.04 or Windows PC with Visual Studio
* On Ubuntu, install these packages to build and run this sample:

  ```sh
  sudo apt update
  sudo apt install build-essential libssl1.0.0 libasound2 wget
  sudo apt install pkg-config automake libtool libtool-bin
  sudo apt install libpoco-dev rapidjson-dev
  ```

* On Windows, install and config [poco](https://pocoproject.org/) and `rapidjson`. The recommended method is to use [vcpkg](https://github.com/microsoft/vcpkg).

  ```bash
  vcpkg.exe install poco rapidjson
  ```

## Build the sample

* Download and compile UniMRCP 1.6.0 on your PC, following the [installation intructions](http://www.unimrcp.org/index.php/project/get-started).

* [Download the sample code to your development PC.](../../README.md#get-the-samples)

* Copy the `ms-recog`, `ms-synth` and `ms-common` folders to `your_unimrcp_path/plugins`

### On Linux

* Download and extract the Speech SDK
  * **By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
  * Run the following commands after replacing the string `/your/path` with a directory (absolute path) of your choice:

    ```sh
    export SPEECHSDK_ROOT="/your/path"
    mkdir -p "$SPEECHSDK_ROOT"
    wget -O SpeechSDK-Linux.tar.gz https://aka.ms/csspeech/linuxbinary
    tar --strip 1 -xzf SpeechSDK-Linux.tar.gz -C "$SPEECHSDK_ROOT"
    ```

* Navigate to the directory of UniMRCP
* Open the file `configure.ac`, and add the following lines in correct position:

    ```shell
    dnl MS recognizer plugin.
    UNI_PLUGIN_ENABLED(msrecog)

    AM_CONDITIONAL([MSRECOG_PLUGIN],[test "${enable_msrecog_plugin}" = "yes"])

    dnl MS synthesizer plugin.
    UNI_PLUGIN_ENABLED(mssynth)

    AM_CONDITIONAL([MSSYNTH_PLUGIN],[test "${enable_mssynth_plugin}" = "yes"])

    ...

    plugins/ms-recog/Makefile
    plugins/ms-synth/Makefile

    ...

    echo MS recognizer plugin.......... : $enable_msrecog_plugin
    echo MS synthesizer plugin......... : $enable_mssynth_plugin
    ```

* Edit the file `plugins/Makefile.am` and add this:

    ```shell
    if MSRECOG_PLUGIN
    SUBDIRS		      += ms-recog
    endif

    if MSSYNTH_PLUGIN
    SUBDIRS		      += ms-synth
    endif
    ```

* Recompile and install the UniMRCP project.

### On Windows

* Add `ms-recog`, `ms-synth` and `ms-common` projects into the `unimrcp-2010.sln` solution.
  
* Restore the nuget packages.

* Recompile the solution in `Release` and `x64` mode.

## Run the sample

* Edit the `unimrcpserver.xml` config file (in `/usr/local/unimrcp/conf` on Linux and `Porject_folder\x64\Release\conf`).

  * In `<plugin-factory>` section, add configure for the plugins, and disable demo plugins:

  ```xml
  <engine id="Demo-Synth-1" name="demosynth" enable="false"/>
  <engine id="MS-Synth-1" name="mssynth" enable="true"/>
  <engine id="Demo-Recog-1" name="demorecog" enable="false"/>
  <engine id="MS-Recog-1" name="msrecog" enable="true"/>
  ```

  * Add 16kHz/16bit Codec support for higher accuracy and quality.

  ```xml
  <codecs own-preference="false">PCMU/97/16000 PCMA/98/16000 L16/99/16000 PCMU PCMA L16/96/8000 telephone-event/101/8000</codecs>
  ```

  * Other settings like IP address and port.

* Add a conf file `config.json` in the same folder of `unimrcpserver.xml`, the sample file is in `sample-conf` path of this project. Remember to replace `YourSubscriptionKey` and `YourServiceRegion` with your own key and region of you subscription.

### On Linux

To run the sample, you'll need to configure the loader's library path to point to the Speech SDK library.

* On an `x64` machine, run:

  ```sh
  export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$SPEECHSDK_ROOT/lib/x64"
  ```

* On an `x86` machine, run:

  ```sh
  export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$SPEECHSDK_ROOT/lib/x86"
  ```

Run the application:

  ```sh
  /usr/local/unimrcp/bin/unimrcpserver
  ```

### On Windows

Run the executable file `Porject_folder\x64\Release\bin\unimrcpserver.exe`.

## Build and Run with docker

You can use docker to build and deploy the UniMRCP with Microsoft SR/TTS plugins easily with docker.

To build the docker, run:

  ```shell
  ./docker/build.sh
  ```

Then, run following script to start the UniMRCP server:

*Note: it's better to use the host network to run the container, for easy configuration and high network efficiency.*

  ```shell
  docker run -dt -v ~/conf:/usr/local/unimrcp/conf --network=host unimrcp_ms:latest --name unimrcp_ms
  ```

The `~/conf` folder contains configure files of UniMRCP and the Microsoft plugins. You can find the sample configure files in `sample-conf` of this project.
Remember to replace `YourSubscriptionKey` and `YourServiceRegion` with your own key and region of you subscription.

## Test

For easily test, you can use `umc` in the UniMRCP project.
In `umc` console, type `run recog` and `run synth` for testing SR and TTS, respectively.

*Tips: remember to correct configurations for codec. The configs in `sample-conf` are tested, you can use them after configuring the IP.*

If you want to test the performance of the MRCP as well as the plugins, a easy way is to adopt `umc` to call SR/TTS continuously and concurrently.
Change the `UmcConsole::RunCmdLine()` method in `umcconsole.cpp` with

  ```c++
  bool UmcConsole::RunCmdLine()
  {
      for(int i = 0; i < 100; i++)
      {
          char cmdline[1024] = "run recog\0";
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
          ProcessCmdLine(cmdline);
      }
      std::this_thread::sleep_for(std::chrono::seconds(20));
      return true;
  }
  ```
  
Also append `#include <thread>` and `#include <chrono>` in the header.
You can change the repeat time and interval to run your own test.
Then, you can use the python scripts in `test_scripts` to analyze the log file to get the latency.


Alternatively, you can use FreeSWITCH to make a real call to test the plugins. See [this]("./test_with_freeswitch.md) for details.

## References

* [UniMRCP](http://www.unimrcp.org/)
* [Speech SDK API reference for C++](https://aka.ms/csspeech/cppref)

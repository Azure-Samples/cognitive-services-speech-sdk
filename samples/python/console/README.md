# Python console app

This sample demonstrates various forms of speech recognition, intent recognition, speech synthesis, translation and transcription using the Speech SDK for Python.

## Prerequisites

* On Windows and Linux Python 3.7 or later needs to be installed. Downloads are available [here](https://www.python.org/downloads/).
* The Python Speech SDK package is available for Windows (x64 and x86), Mac x64 (macOS X version 10.14 or later), Mac arm64 (macOS version 11.0 or later), and Linux (see the list of [supported Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux)).
* On Ubuntu or Debian, run the following commands for the installation of required packages:
  ```sh
  sudo apt-get update
  sudo apt-get install libssl-dev libasound2
  ```

  * On **Ubuntu 22.04 LTS** it is also required to download and install the latest **libssl1.1** package e.g. from http://security.ubuntu.com/ubuntu/pool/main/o/openssl/.

* On RHEL or CentOS, run the following commands for the installation of required packages:
  ```sh
  sudo yum update
  sudo yum install alsa-lib openssl python3
  ```

  * See also [how to configure RHEL/CentOS 7 for Speech SDK](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-configure-rhel-centos-7).

* On Windows you also need the [Microsoft Visual C++ Redistributable for Visual Studio 2017](https://support.microsoft.com/help/2977003/the-latest-supported-visual-c-downloads) for your platform.


## Build the sample

**By using the Cognitive Services Speech SDK you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

* Install the Speech SDK Python package in your Python interpreter, typically by executing the command
  ```sh
  pip install azure-cognitiveservices-speech
  ```
  in a terminal.
* For transcription sample, install the Scipy package in your Python interpreter by executing the command
  ```sh
  pip install scipy
  ```
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* To tailor the sample to your configuration, use search and replace across the whole sample directory to update the following strings:

  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with.
    For example, `westus` or `northeurope`.
  * The following settings apply for intent recognition powered by the [Language Understanding service (LUIS)](https://aka.ms/csspeech/luisdocs):
    * `YourLanguageUnderstandingSubscriptionKey`: replace with your Language Understanding service subscription key (endpoint key).
    * `YourLanguageUnderstandingServiceRegion`: replace with the region associated with your Language Understanding service subscription.
    * `YourLanguageUnderstandingAppId`: replace with the ID of a Language Understanding service application that you want to recognize intents with.
  * The following settings apply to keyword-triggered recognition:
    * `YourKeywordRecognitionModelFile.table`: replace with the location of your keyword recognition model file.
    * `YourKeyword`: replace with the phrase your keyword recognition model triggers on.
  * Some samples require audio files to be present. Put appropriate audio files somewhere on your file system and adapt the paths in the Python source files.

## Run the samples

To run the app, navigate to the `samples/python/console` directory in your local copy of the samples repository.
Start the app with the command

```sh
python3 main.py
```

Depending on your platform, the Python 3 executable might also just be called `python`.

The app displays a menu that you can navigate using your keyboard.
Choose the scenarios that you're interested in.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-python)
* [Speech SDK API reference for Python](https://aka.ms/csspeech/pythonref)

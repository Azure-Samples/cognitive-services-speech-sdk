# Quickstart: Using the Speech Service from Python

This sample shows how to use the Speech Service through the Speech SDK for Python. It illustrates how the SDK can be used to recognize speech from microphone input.

See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/speech-to-text-from-microphone?tabs=dotnet%2Cx-android%2Clinux%2Cjava-runtime%2Cwindowsinstall&pivots=programming-language-python) on the SDK documentation page for step-by-step instructions.

## Prerequisites

Before you get started, here's a list of prerequisites:

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* Python 3.5 or later needs to be installed. Downloads are available [here](https://www.python.org/downloads/).
* The Python Speech SDK package is available for Windows (x64 and x86), Mac (macOS X version 10.12 or later), and [specific Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux).
* On Ubuntu or Debian, run the following commands for the installation of required packages:
  ```sh
  sudo apt-get update
  sudo apt-get install libssl1.0.0 libasound2
  ```

  * If libssl1.0.0 is not available, install libssl1.0.x (where x is greater than 0) or libssl1.1 instead.

* On RHEL or CentOS, run the following commands for the installation of required packages:
  ```sh
  sudo yum update
  sudo yum install alsa-lib openssl python3
  ```

  * See also [how to configure RHEL/CentOS 7 for Speech SDK](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-configure-rhel-centos-7).

* On Windows you need the [Microsoft Visual C++ Redistributable for Visual Studio 2017](https://support.microsoft.com/help/2977003/the-latest-supported-visual-c-downloads) for your platform.

## Get the Speech SDK Python Package

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**

The Cognitive Services Speech SDK Python package can be installed from [pyPI](https://pypi.org/) using this command:

```sh
pip install azure-cognitiveservices-speech
```

## Run the sample

Before you run the sample code, replace the dummy values `YourSubscriptionKey` and `YourServiceRegion` by your subscription information.
You can either load the quickstart sample in your IDE and run it there or run it in a console by navigating to the directory you downloaded it to and running the command

```sh
python quickstart.py
```

Alternatively, you can run the sample `quickstart.ipynb` as a [Jupyter notebook](https://jupyter.org) file.

## Support

If you have a problem or are missing a feature, please have a look at our [support page](https://docs.microsoft.com/azure/cognitive-services/speech-service/support).

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-python)
* [Speech SDK API reference for C++](https://aka.ms/csspeech/pythonref)


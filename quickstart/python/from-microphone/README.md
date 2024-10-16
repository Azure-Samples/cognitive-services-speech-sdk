# Quickstart: Recognize speech from a microphone from Python using the Speech Service

This sample shows how to use the Speech Service through the Speech SDK for Python. It illustrates how the SDK can be used to recognize speech from microphone input.

See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-python) on the SDK documentation page for step-by-step instructions.

## Prerequisites

Before you get started, here's a list of prerequisites:

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* On Windows and Linux Python 3.6 or later needs to be installed. On Mac, minimum version for Python is 3.7. Downloads are available [here](https://www.python.org/downloads/).
* The Python Speech SDK package is available for Windows (x64 and x86), Mac x64 (macOS X version 10.14 or later), Mac arm64 (macOS version 11.0 or later), and [specific Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux).
* On Linux, see the [Linux platform requirements](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?tabs=linux&pivots=programming-language-python#platform-requirements) for installing the required dependencies.
* On Windows you need the [Microsoft Visual C++ Redistributable for Visual Studio 2017](https://support.microsoft.com/help/2977003/the-latest-supported-visual-c-downloads) for your platform.

## Get the Speech SDK Python Package

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

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


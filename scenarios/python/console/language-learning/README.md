# Scenarios: Language Learning via Azure Pronunciation Assessment with Speech to Text

This sample demonstrates how to use Azure pronunciation assessment SDK and REST API for language learning.

## Option 1: Run the Sample Easily in Visual Studio Code (Recommended)
This is the easiest way to get started. The Azure AI Speech Toolkit extension automates setup, environment configuration and run.

- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- In extension's panel, click `View Samples` and download this sample from sample gallery.
- From the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), run:
  - `Azure AI Speech Toolkit: Configure Azure Speech Resources` to select an **Azure AI Service** resource (one-time setup).
  - `Azure AI Speech Toolkit: Configure and Setup the Sample App` to prepare the project (one-time setup).
  - `Azure AI Speech Toolkit: Run the Sample App` to run the sample.

## Option 2: Manual Setup and Run (Advanced)
Follow these steps if you prefer not to use VS Code.

### Prerequisites

Before you get started, here's a list of prerequisites:

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* See the [Speech SDK installation QuickStart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-python) for details on system requirements and setup.

### Get the Speech SDK Python Package

**By downloading the Microsoft Cognitive Services Speech SDK, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**

The Cognitive Services Speech SDK Python package can be installed from [PyPI](https://pypi.org/) using this command:

```sh
pip install azure-cognitiveservices-speech
```

### Run the sample

Before you run the sample code, in `config.json` file, replace the value `your_subscription_key` with your subscription key and
replace the value `your_service_region` for your Speech resource. You can find these values in the Azure Portal under your Speech resource's "Keys and Endpoint" section.
You can either load the sample in your IDE and run it there or run it in a console by navigating to the directory you downloaded it to and running the command

```sh
python main.py
```

We also provide demo to do language learning with Azure OpenAI, refer to [this readme](chatting/README.md) for more information.

## Support

If you have a problem or are missing a feature, please have a look at our [support page](https://docs.microsoft.com/azure/cognitive-services/speech-service/support).

## References

* [QuickStart article on the SDK documentation site](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-pronunciation-assessment?pivots=programming-language-python)
* [Speech SDK API reference for Python](https://aka.ms/csspeech/pythonref)`
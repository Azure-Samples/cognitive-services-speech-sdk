# Python console app

This sample demonstrates various forms of speech recognition, intent recognition, speech synthesis, translation and transcription using the Speech SDK for Python.

## Prerequisites

* See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-python) for details on system requirements and setup.

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
  * `https://YourServiceRegion.api.cognitive.microsoft.com`: replace with the endpoint for your Speech resource. You can find this endpoint in the Azure Portal under your Speech resource's "Keys and Endpoint" section.
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

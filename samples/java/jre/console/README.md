# Java Console app for the Java Run-Time Environment (JRE) on Windows or Linux

This sample demonstrates various forms of speech recognition, intent recognition, speech synthesis, and translation using the Speech SDK for Java on Windows or Linux.

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows, Linux, Mac) capable to run [Eclipse](https://www.eclipse.org),[<sup>[1]</sup>](#footnote1) some sample scenarios require a working microphone.
* See the [Speech SDK installation quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform?pivots=programming-language-java) for details on system requirements and setup.

<small><a name="footnote1">1</a>. This sample has not been verified with Eclipse on ARM platforms.</small>

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Create an empty workspace in Eclipse and import the folder containing this sample as a project into your workspace.
* To tailor the sample to your environment, use search and replace across the whole project to update the following strings:
  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourEndpointUrl`: replace with the endpoint URL for your subscription, for example, 
    `https://westus.api.cognitive.microsoft.com/`.
  * `YourEndpointId` (optional): replace with the endpoint ID of your customized model in [CRIS](https://cris.ai).
  * `YourAudioFile.wav`: replace with a path to a `.wav` file on your disk **(required format: 16 kHz sample rate, 16 bit samples, mono / single-channel)**
  * The following settings apply for intent recognition powered by the [Language Understanding service (LUIS)](https://aka.ms/csspeech/luisdocs):
    * `YourLanguageUnderstandingSubscriptionKey`: replace with your Language Understanding service subscription key (endpoint key).
    * `YourLanguageUnderstandingServiceRegion`: replace with the region associated with your Language Understanding service subscription.
    * `YourLanguageUnderstandingAppId`: replace with the ID of a Language Understanding service application that you want to recognize intents with.
    * `YourLanguageUnderstandingIntentName1`, `YourLanguageUnderstandingIntentName2`, `YourLanguageUnderstandingIntentName3`: replace with names of intents that your Language Understanding service application recognizes.
  * The following settings apply to keyword-triggered recognition:
    * `YourKeywordRecognitionModelFile.table`: replace with the location of your keyword recognition model file.
    * `YourKeyword`: replace with the phrase your keyword recognition model triggers on.

* Save the modified files.

## Run the sample

* Press F11, or select **Run** \> **Debug**.
* Or run in terminal:

```sh
mvn clean package
java -jar ./target/SpeechSDKDemo-0.0.1-SNAPSHOT-jar-with-dependencies.jar
```
## References

* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)

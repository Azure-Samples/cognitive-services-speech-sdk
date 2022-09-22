# Java Console app for the Java Run-Time Environment (JRE) on Windows or Linux

This sample demonstrates various forms of speech recognition, intent recognition, speech synthesis, and translation using the Speech SDK for Java on Windows or Linux.

> **Note:**
> The Speech SDK for the JRE currently supports only the Windows x64 platform, and [specific Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux).

## Prerequisites

* A subscription key and region for the Speech service [Create one for free](https://azure.microsoft.com/en-us/free/cognitive-services/)
* [Create a Speech resource](https://ms.portal.azure.com/#create/Microsoft.CognitiveServicesSpeechServices) in the Azure portal.
* Get the resource key and region. After your Speech resource is deployed, select Go to resource to view and manage keys. or  more information about Cognitive Services resources, see [Get the keys for your resource](https://docs.microsoft.com/en-us/azure/cognitive-services/cognitive-services-apis-create-account?tabs=multiservice%2Cwindows#get-the-keys-for-your-resource)
* A PC (Windows x64 or a supported Linux distribution) or Mac (macOS 10.14 or later) capable to run Eclipse,[<sup>[1]</sup>](#footnote1) with a working speaker.
* On Ubuntu or Debian, run the following commands for the installation of required packages:

  ```sh
  sudo apt-get update
  sudo apt-get install libssl-dev libasound2
  ```

  * On **Ubuntu 22.04 LTS** it is also required to download and install the latest **libssl1.1** package e.g. from http://security.ubuntu.com/ubuntu/pool/main/o/openssl/.

* On RHEL or CentOS, run the following commands for the installation of required packages:

  ```sh
  sudo yum update
  sudo yum install alsa-lib java-1.8.0-openjdk-devel openssl
  ```

  * See also [how to configure RHEL/CentOS 7 for Speech SDK](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-configure-rhel-centos-7).

<small><a name="footnote1">1</a>. This sample has not been verified with Eclipse on ARM platforms.</small>

## Build the sample

* **By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license201809).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Create an empty workspace in Eclipse and import the folder containing this sample as a project into your workspace.
* To tailor the sample to your environment, use search and replace across the whole project to update the following strings:
  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with.
    For example, `westus` or `northeurope`.
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

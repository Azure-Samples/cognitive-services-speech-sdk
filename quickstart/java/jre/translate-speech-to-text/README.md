# Quickstart: Translate speech in Java on Windows or Linux

This sample demonstrates how to translate speech with the Speech SDK for Java on Windows or Linux (Ubuntu 16.04, Ubuntu 18.04, Debian 9, RHEL 8, CentOS 8).
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstarts/translate-speech-to-text?tabs=dotnet%2Cwindowsinstall&pivots=programming-language-java) on the SDK documentation page which describes how to build this sample from scratch in Eclipse.

> **Note:**
> The Speech SDK for the JRE currently supports only the Windows x64 platform, macOS 10.13 or later, and [specific Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows x64 or a supported Linux distribution) capable to run Eclipse,[<sup>[1]</sup>](#footnote1) with a working microphone.
* Java 8 or 11 JRE/JDK.
* Version 4.8 of [Eclipse](https://www.eclipse.org), 64-bit.[<sup>[1]</sup>](#footnote1)
* On Ubuntu or Debian, run the following commands for the installation of required packages:

  ```sh
  sudo apt-get update
  sudo apt-get install libssl1.0.0 libasound2
  ```

  * If libssl1.0.0 is not available, install libssl1.0.x (where x is greater than 0) or libssl1.1 instead.

* On RHEL or CentOS, run the following commands for the installation of required packages:

  ```sh
  sudo yum update
  sudo yum install alsa-lib java-1.8.0-openjdk-devel openssl
  ```

  * See also [how to configure RHEL/CentOS 7 for Speech SDK](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-configure-rhel-centos-7).

<small><a name="footnote1">1</a>. This sample has not been verified with Eclipse on ARM platforms.</small>

## Build the sample

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-translate-speech-java-jre).

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://docs.microsoft.com/azure/cognitive-services/speech-service/license).**
* [Download the sample code to your development PC.](/README.md#get-the-samples)
* Create an empty workspace in Eclipse and import the folder containing this sample as a project into your workspace.
* Edit the `Main` source:
  * Replace the string `YourSubscriptionKey` with your own subscription key.
  * Replace the string `YourServiceRegion` with the service region of your subscription.
    For example, replace with `westus` if you are using the 30-day free trial subscription.
* Save the modified file.

## Run the sample

* Press F11, or select **Run** \> **Debug**.

## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-translate-speech-java-jre)
* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)

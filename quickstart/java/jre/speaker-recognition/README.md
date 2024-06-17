# Quickstart: Recognize a speaker in Java on Windows, macOS, or Linux

This sample demonstrates how to perform speaker recognition with the Speech SDK for Java on Windows, macOS or Linux.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-java-jre) on the SDK documentation page which describes how to build this sample from scratch in Eclipse.

> **Note:**
> The Speech SDK for the JRE currently supports the Windows x64 platform, macOS x64 (10.14 or later), macOS M1 arm64 (11.0 or later), and [specific Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux).

**Note:** Microsoft limits access to speaker recognition. You can apply for access through the [Azure Cognitive Services speaker recognition limited access review](https://aka.ms/azure-speaker-recognition). For more information, see [Limited access for speaker recognition](https://docs.microsoft.com/legal/cognitive-services/speech-service/speaker-recognition/limited-access-speaker-recognition).

## Prerequisites

* A subscription key for the Speech service. See [Try the speech service for free](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started).
* A PC (Windows x64 or a supported Linux distribution) or Mac (macOS 10.14 or later) capable to run Eclipse,[<sup>[1]</sup>](#footnote1) with a working microphone.
* Java 8 or 11 JRE/JDK.
* Version 4.8 of [Eclipse](https://www.eclipse.org), 64-bit.[<sup>[1]</sup>](#footnote1)
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

> Note: more detailed step-by-step instructions are available [here](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-java-jre).

* **By downloading the Microsoft Cognitive Services Speech SDK when building this sample, you acknowledge its license, see [Speech SDK license agreement](https://aka.ms/csspeech/license).**
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

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-java-jre)
* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)

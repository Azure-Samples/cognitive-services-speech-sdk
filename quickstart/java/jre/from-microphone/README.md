# Quickstart: Recognize speech from a microphone in Java on Windows, macOS, or Linux

This sample demonstrates how to recognize speech with the Speech SDK for Java on Windows, macOS or Linux.
See the [accompanying article](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started-speech-to-text?tabs=terminal&pivots=programming-language-java) on the SDK documentation page which describes how to build this sample from scratch in Eclipse.

> **Note:**
> The Speech SDK for the JRE currently supports the Windows x64 platform, macOS x64 (10.14 or later), macOS M1 arm64 (11.0 or later), and [specific Linux distributions and target architectures](https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk?tabs=linux).

## Prerequisites


* On Ubuntu or Debian, run the following commands for the installation of required packages:

  ```sh
  sudo apt-get update
  sudo apt-get install libssl1.0.0 libasound2
  ```

  * On **Ubuntu 22.04 LTS** it is also required to download and install the latest **libssl1.1** package e.g. from http://security.ubuntu.com/ubuntu/pool/main/o/openssl/.

* On RHEL or CentOS, run the following commands for the installation of required packages:

  ```sh
  sudo yum update
  sudo yum install alsa-lib java-1.8.0-openjdk-devel openssl
  ```

  * See also [how to configure RHEL/CentOS 7 for Speech SDK](https://docs.microsoft.com/azure/cognitive-services/speech-service/how-to-configure-rhel-centos-7).

<small><a name="footnote1">1</a>. This sample has not been verified with Eclipse on ARM platforms.</small>




## References

* [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/get-started-speech-to-text?tabs=terminal&pivots=programming-language-java)
* [Speech SDK API reference for Java](https://aka.ms/csspeech/javaref)

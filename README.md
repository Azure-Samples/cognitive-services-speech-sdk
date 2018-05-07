# Sample Repository for the Cognitive Services Speech SDK

This project hosts the **samples** for the Cognitive Services Speech SDK. To find out more about the Cognitive Services Speech SDK itself, please visit the [SDK documentation site](https://aka.ms/csspeech).

## Features

This repository hosts samples that help you to get started with several features of the SDK.
In addition more complex scenarios are included to give you a head-start on using speech technology in your application.

We tested the samples with the latest released version of the SDK (0.2.12733) on the Windows 10 and Ubuntu 16.04 platform.

## Getting Started

The [SDK documentation](https://aka.ms/csspeech) has extensive sections about getting started, setting up the SDK, as well as the process to acquire the required subscription keys. You will need subscription keys to run the samples on your machines, you therefore should follow the instructions on these pages before continuing.

Once you have these keys, follow these steps:

### Windows

The sample requires a working installation of the [Visual Studio 2017 IDE](https://www.visualstudio.com/) on your system (Visual Studio Community edition is sufficient). Then clone this sample repository onto your local machine. In the `Windows` folder, you can find various samples with solution files (`*.sln`) that you can open directly in Visual Studio 2017. Compile the samples inside Visual Studio,  the required Microsoft Cognitive Services Speech SDK components will automatically be downloaded from [nuget.org](https://aka.ms/nuget).

### Linux

[Our main documentation](https://aka.ms/csspeech/linux) describes how to download and install the Speech SDK for Linux.
Then clone this sample repository onto your local machine.
Linux samples are contained in the `Linux` folder of this sample repository.
To build edit the contained `Makefile` and run `make`.
Please see [here](https://aka.ms/csspeech) for more information.

## Resources

- [Cognitive Services Speech SDK Documentation](https://aka.ms/csspeech)

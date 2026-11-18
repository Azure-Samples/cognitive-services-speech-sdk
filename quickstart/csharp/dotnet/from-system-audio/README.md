# Quickstart: Recognize speech from Default System Audio in C# under .NET Framework for Windows

This sample demonstrates how to recognize speech of System Audio, example running Video on a browser, Playing Sample Audio file on the system with C# under the .NET Framework (version 4.6.1 or above) using the Speech SDK for Windows. See the accompanying article on the SDK documentation page which describes how to build this sample from scratch in Visual Studio 2017.

## Prerequisites

- A subscription key for the Speech service. See [Try the speech service for free](https://azure.microsoft.com/free/cognitive-services/).
- A Windows PC 
- Microsoft Visual Studio 2017, Community Edition or higher.
- The .NET desktop development workload in Visual Studio. You can enable it in Tools > Get Tools and Features.
- .NET Core SDK 3.1 or later
- Microsoft.CognitiveServices.Speech package
- NAudio package
Use below command to install 
    `dotnet add package Microsoft.CognitiveServices.Speech`
    `dotnet add package NAudio`

## Build the sample

By building this sample you will download the Microsoft Cognitive Services Speech SDK. By downloading you acknowledge its [license](https://aka.ms/csspeech/license201809).

1. Download the sample code to your development PC.
2. Start Microsoft Visual Studio 2017 and select File > Open > Project/Solution.
3. Navigate to the folder containing this sample, and select the solution file contained within it.
4. Edit the Program.cs source:
   - Replace the string `AZURE_SPEECH_KEY` with your own subscription key. Use the Speech resource in Azure (not the Speech Recognition resource).
   - Replace the string `AZURE_REGION` with the service region of your subscription. For example, replace with `westus` if you are using the 30-day free trial subscription. Make sure the region in your Azure resource matches the region you put into the sample, otherwise you'll get a 401 unauthorized access error.
5. Set the active solution configuration and platform to the desired values under Build > Configuration Manager:
   - On a 64-bit Windows installation, choose `x64` as active solution platform.
   - On a 32-bit Windows installation, choose `x86` as active solution platform.
6. Press Ctrl+Shift+B, or select Build > Build Solution.

## On Terminal 
 run `dotnet run` to run the sample

## Run the sample

To debug the app and then run it, press F5 or use Debug > Start Debugging. To run the app without debugging, press Ctrl+F5 or use Debug > Start Without Debugging.


## References

- [Quickstart article on the SDK documentation site](https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-csharp-dotnetcore-windows)
- [Speech SDK API reference for C#](https://aka.ms/csspeech/csharpref)


## Installation

To install the required packages, run the following commands:



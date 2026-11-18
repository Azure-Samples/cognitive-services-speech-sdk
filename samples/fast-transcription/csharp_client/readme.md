# C# Console app for .NET Core 

This sample demonstrates audio transcription using the Fast Transcription API for C#. It runs under .NET 8.0 or later. 

## Prerequisites

1. An Azure AI Speech resource in one of the regions where the fast transcription API is available. The supported regions are: Central India, East US, Southeast Asia, and West Europe. For more information and region identifier, see [Speech service regions](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/regions)
1. An audio file (less than 2 hours long and less than 200 MB in size) in one of the formats and codecs supported by the batch transcription API. For more information about supported audio formats, see [Supported audio formats](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/batch-transcription-audio-data?tabs=portal#supported-audio-formats-and-codecs)
1. For additional explanation, see [fast transcription documentation](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/fast-transcription-create)


## Build the sample

* To tailor the sample to your configuration, use search and replace across the whole solution (for example, in Visual Studio, via **Edit** \> **Find and Replace** \> **Quick Replace**) to update the following strings:

  * `YourSubscriptionKey`: replace with your subscription key.
  * `YourServiceRegion`: replace with the [region](https://aka.ms/csspeech/region) your subscription is associated with.
    For example, `westus` or `northeurope`.
  

## Run the sample
You can run the sample using Microsoft Visual Studio 2022 on Windows or using a .NET Core CLI

### Using Visual Studio 2022

TODO


### Using the .NET Core CLI

Navigate to the directory (samples/fast-transcription/csharp_client) that contains the sample and Run the following command

```bash
cd samples/fast-transcription/csharp_client
dotnet run
```

## References

* [fast transcription documentation](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/fast-transcription-create)

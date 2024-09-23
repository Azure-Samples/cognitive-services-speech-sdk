# How to use the Speech Services Fast Transcription API from Python

## Prerequisites

1. An Azure AI Speech resource in one of the regions where the fast transcription API is available. The supported regions are: Central India, East US, Southeast Asia, and West Europe. For more information and region identifier, see [Speech service regions](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/regions)
1. An audio file (less than 2 hours long and less than 200 MB in size) in one of the formats and codecs supported by the batch transcription API. For more information about supported audio formats, see [Supported audio formats](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/batch-transcription-audio-data?tabs=portal#supported-audio-formats-and-codecs)
1. For additional explanation, see [fast transcription documentation](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/fast-transcription-create)

## Install other dependencies

The sample uses the `requests` library. You can install it with the command

```bash
pip install requests
```

## Run the sample code

The sample code itself is [main.py](python_client/main.py) and can be run using Python 3.8 or higher.
You will need to adapt the following information to run the sample:

1. Your AI Services subscription key and region.
    
    Some notes:
    - You can get the subscription key from the "Keys and Endpoint" tab on your Speech resource in the Azure Portal.

1. The audio fileThe URI of an audio recording in blob storage. Please refer to the [Azure Storage documentation](https://docs.microsoft.com//rest/api/storageservices/authorize-requests-to-azure-storage) on information on how to authorize accesses against blob storage.

You can use a development environment like Visual Studio Code to edit, debug, and execute the sample.


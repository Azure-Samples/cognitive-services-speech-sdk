# How to use the Speech Services Batch Synthesis API from Python

## Install dependencies

The sample uses the `requests` library. You can install it with the command

```sh
pip install requests
```

## Run the sample code

The sample code itself is [synthesis.py](synthesis.py) and can be run using Python 3.7 or higher.
You will need to adapt the following information to run the sample:

1. Your Azure AI Speech Service.

    Some notes:

    - You can get the subscription key and endpoint from the "Keys and Endpoint" tab on your Cognitive Services or Speech resource in the Azure Portal.
    - Batch synthesis is only available for paid subscriptions, free subscriptions are not supported.
    - Please refer to [this page](https://learn.microsoft.com/azure/ai-services/speech-service/regions#rest-apis) for a complete list of region identifiers in the expected format.

2. (Optional:) The relationship between custom voice names and deployment ID, if you want to use custom voices.
3. (Optional:) The URI of a writable Azure blob container, if you want to store the audio files in your own Azure storage.

You can use a development environment like VS Code to edit, debug, and execute the sample.

# How to use the Speech Services Batch Transcription API from Python

## Download and install the API client library

To execute the sample you need to generate the Python library for the REST API which is generated through [Swagger](swagger.io).

Follow these steps for the installation:

1. Go to https://editor.swagger.io.
1. Click **File**, then click **Import URL**.
1. Enter the Swagger URL for the Speech Services API: `https://raw.githubusercontent.com/Azure/azure-rest-api-specs/main/specification/cognitiveservices/data-plane/Speech/SpeechToText/stable/v3.1/speechtotext.json`.
1. Click **Generate Client** and select **Python**.
1. Save the client library.
1. Extract the downloaded python-client-generated.zip somewhere in your file system.
1. Install the extracted python-client module in your Python environment using pip: `pip install path/to/package/python-client`.
1. The installed package has the name `swagger_client`. You can check that the installation worked using the command `python -c "import swagger_client"`.

## Install other dependencies

The sample uses the `requests` library. You can install it with the command

```bash
pip install requests
```

## Run the sample code

The sample code itself is [main.py](python-client/main.py) and can be run using Python 3.7 or higher.
You will need to adapt the following information to run the sample:

1. Your Cognitive Services subscription key and region.
    
    Some notes:

    - You can get the subscription key from the "Keys and Endpoint" tab on your Cognitive Services or Speech resource in the Azure Portal.
    - Batch transcription is only available for paid subscriptions, free subscriptions are not supported.
    - Please refer to [this page](https://docs.microsoft.com/azure/cognitive-services/speech-service/regions#rest-apis) for a complete list of region identifiers in the expected format.

1. The URI of an audio recording in blob storage. Please refer to the [Azure Storage documentation](https://docs.microsoft.com//rest/api/storageservices/authorize-requests-to-azure-storage) on information on how to authorize accesses against blob storage.
1. (Optional:) The model ID of an adapted model, if you want to use a custom model.
1. (Optional:) The URI of a container with audio files if you want to transcribe all of them with a single request.

You can use a development environment like Visual Studio Code to edit, debug, and execute the sample.


# How to use the Speech Services Batch Synthesis API from Python


## Prerequisites
- Python 3.8 or higher
- (Optional) The relationship between custom voice names and deployment ID, if you want to use custom voices.
- (Optional) The URI of a writable Azure blob container, if you want to store the audio files in your own Azure storage.

## Notes for Your Azure Speech Service:
- We recommend using a passwordless authentication provided by the `azure-identity` library. Your Microsoft Entra user account is need to be assigned with `Cognitive Services User` or `Cognitive Services Speech User` role.
- Alternatively, you can get the subscription key from the "Keys and Endpoint" tab on your Azure AI Speech resource in the Azure Portal. (If you are using Azure AI Speech Toolkit extension within VS Code, it will handle the secret for you.)
- Batch synthesis is only available for paid subscriptions, free subscriptions are not supported.
- Please refer to [this page](https://learn.microsoft.com/azure/ai-services/speech-service/regions#rest-apis) for a complete list of region identifiers in the expected format.

## Option 1: Run the Sample Easily in Visual Studio Code (Recommended)
This is the easiest way to get started. The Azure AI Speech Toolkit extension automates setup, environment configuration, build, and run.

- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- In extension's panel, click `View Samples` and download this sample from sample gallery.
- From the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), run:
  - `Azure AI Speech Toolkit: Configure Azure Speech Resources` to select a speech resource (one-time setup).
  - `Azure AI Speech Toolkit: Configure and Setup the Sample App` to prepare the project (one-time setup).
  - `Azure AI Speech Toolkit: Build the Sample App` to compile the code.
  - `Azure AI Speech Toolkit: Run the Sample App` to run the sample.

## Option 2: Manual Setup and Run (Advanced)
Follow these steps if you prefer not to use VS Code.

### Install dependencies

The sample uses the `requests` library. You can install it with the command

```sh
pip install requests
```

We recommend using a passwordless authentication provided by the `azure-identity` library.
You can install it with the command

```sh
pip install azure-identity
```

### Set Up Environment Variables
```sh
export SPEECH_ENDPOINT="your_speech_endpoint"
export SPEECH_REGION="your_speech_region"
export SPEECH_KEY="your_speech_key"
```

### Run the Sample Code
```sh
python synthesis.py
```
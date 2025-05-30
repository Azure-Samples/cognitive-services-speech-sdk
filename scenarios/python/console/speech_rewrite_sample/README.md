# Scenario: Continuous Speech Recognition and Rewriting via Azure OpenAI
This project integrates Azure Cognitive Services Speech SDK with Azure OpenAI Service to perform real-time speech recognition and refine the recognized text for improved grammar and readability.

# Features
1. Real-time speech-to-text transcription using Azure Cognitive Services Speech SDK.
2. Automatic refinement of recognized text using Azure OpenAI Service.
3. Grammar correction, minor rewrites for improved readability, and spelling fixes for predefined phrases.

## Option 1: Run the Sample Easily in Visual Studio Code (Recommended)
This is the easiest way to get started. The Azure AI Speech Toolkit extension automates setup, environment configuration, build, and run.

- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- In extension's panel, click `View Samples` and download this sample from sample gallery.
- From the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), run:
  - `Azure AI Speech Toolkit: Configure Azure Speech Resources` to select a speech resource (one-time setup).
  - `Azure AI Speech Toolkit: Configure and Setup the Sample App` to prepare the project (one-time setup).
  - `Azure AI Speech Toolkit: Run the Sample App` to run the sample.
    

## Option 2: Manual Setup and Run (Advanced)
Follow these steps if you prefer not to use VS Code.

### Prerequisites
- Install a version of [Python from 3.7 or later](https://www.python.org/downloads/). 

### Environment Setup
- Manually set the following environment variables.

  - `SPEECH_REGION`: Azure region for the Speech Service (e.g., `eastus`).
  - `SPEECH_KEY`: Azure Cognitive Services Speech API key.
  - `AZURE_OPENAI_ENDPOINT`: Endpoint for Azure OpenAI Service (e.g., `https://<your-resource-name>.openai.azure.com`).
  - `AZURE_OPENAI_API_KEY`: API key for Azure OpenAI Service.

When running the sample app, you can set --relevant_phrases parameter.
  - `--relevant_phrases`: (Optional) Default: Azure Cognitive Services, non-profit organization, speech recognition, OpenAI API


## Example Output
Speak into the microphone. The sample application will print both the recognition result and the rewritten version.
For instance, if you speak "how ar you" into the microphone, the output will be:

```
RAW RECO: how ar you
REWRITE: How are you?
```

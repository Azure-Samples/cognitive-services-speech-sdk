# How to use the Speech Services Batch Synthesis API from C#

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

Use one of the following authentication methods:

Option 1: Token-based authentication (Recommended)

- Set the Speech endpoint (set `SPEECH_ENDPOINT`)
  - Ensure your Microsoft Entra account is assigned the "Cognitive Services Speech Contributor" or "Cognitive Services User" role.

Option 2: Key-based authentication

- Set the Speech Service subscription key (set `SPEECH_KEY`)
- Set the region matching your subscription key (set `SPEECH_REGION`)

## Advanced Usage

Update the initialization code to support extra features:
* (Optional) The relationship between custom voice names and deployment ID, if you want to use custom voices.
* (Optional)The URI of a writable Azure blob container, if you want to store the audio files in your own Azure storage.


## Note

The sample text [*The Great Gatsby* Chapter 1](./Gatsby-chapter1.txt) is an open-domain book and downloaded from [gutenberg](https://www.gutenberg.org/ebooks/64317)
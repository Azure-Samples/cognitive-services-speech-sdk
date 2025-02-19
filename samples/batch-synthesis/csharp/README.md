# How to use the Speech Services Batch Synthesis API from C#

## Run the Sample within VS Code
1. Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
2. Download this sample from sample gallery to local machine.
3. Trigger `Azure AI Speech Toolkit: Configure Azure Speech Resources` command from command palette to select speech resource.
4. Trigger `Azure AI Speech Toolkit: Configure and Setup the Sample App` command from command palette to configure and setup the sample. This command only needs to be run once.
5. Trigger `Azure AI Speech Toolkit: Build the Sample App` command from command palette to build the sample.
6. Trigger `Azure AI Speech Toolkit: Run the Sample App` command from command palette to run the sample.

## Run the Sample within VS Code
If you are running the sample **without** the [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension for VS Code, set environment variables with your Speech Service's information:
* `SPEECH_KEY`: Your speech resource key.
* `SPEECH_REGION`: Your speech resource region.

## Advanced Usage

Update the initialization code to support extra features:
* (Optional) The relationship between custom voice names and deployment ID, if you want to use custom voices.
* (Optional)The URI of a writable Azure blob container, if you want to store the audio files in your own Azure storage.


## Note

The sample text [*The Great Gatsby* Chapter 1](./Gatsby-chapter1.txt) is an open-domain book and downloaded from [gutenberg](https://www.gutenberg.org/ebooks/64317)
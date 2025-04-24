# How to use the Speech Services Batch Transcription API from Python

## Download and install the API client library

To execute the sample you need to generate the Python library for the REST API which is generated through Swagger.

Follow these steps for the installation:

1. Go to https://editor.swagger.io.
1. Click **File**, then click **Import URL**.
1. Enter the Swagger URL for the Speech Services API: `https://raw.githubusercontent.com/Azure/azure-rest-api-specs/main/specification/cognitiveservices/data-plane/Speech/SpeechToText/stable/2024-11-15/speechtotext.json`.
1. Click **Generate Client** and select **Python**.
1. Save the client library.
1. Extract the downloaded python-client-generated.zip somewhere in your file system.
1. Install the extracted python-client module in your Python environment using pip: `pip install path/to/package/python-client`.
1. The installed package has the name `swagger_client`. You can check that the installation worked using the command `python -c "import swagger_client"`.

## Prerequisites

- **recordings_blob_uris** (Azure Blob Storage SAS URI pointing to audio files) or **recordings_container_uri** (Azure Blob Storage Container SAS URI that stored audio files). You can upload your local audio or recording files to Azure Blob Storage. Alternatively, you can download example en-US audio files from [sample audio files](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/sampledata/audiofiles) and then upload it to your Azure blob storage.
- The **locale** of the audio file (e.g. en-US, zh-CN, etc.).

## Run the Sample Easily in Visual Studio Code
This is the easiest way to get started. The Azure AI Speech Toolkit extension automates setup, environment configuration, build, and run.

- Install [Azure AI Speech Toolkit](https://marketplace.visualstudio.com/items?itemName=ms-azureaispeech.azure-ai-speech-toolkit) extension in VS Code.
- In extension's panel, click `View Samples` and download this sample from sample gallery.
- From the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), run:
  - `Azure AI Speech Toolkit: Configure Azure Speech Resources` to select a speech resource (one-time setup).
  - `Azure AI Speech Toolkit: Configure and Setup the Sample App` to prepare the project (one-time setup).
  - `Azure AI Speech Toolkit: Build the Sample App` to compile the code.
  - `Azure AI Speech Toolkit: Run the Sample App` to run the sample.

## Advanced Usage

By default, the sample code uses the `recordings_blob_uris` parameter. You can specify either a single recording file blob URL or multiple URLs, separated by commas.

If you prefer to use the `recordings_container_uri` parameter, follow these steps:

1. In  **python-client/main.py**, comment out the following line:
    ```python
    transcription_definition = transcribe_from_single_blob(RECORDINGS_BLOB_URIS, properties)
    ```

2. Uncomment the following line:
    ```python
    # transcription_definition = transcribe_from_container(RECORDINGS_CONTAINER_URI, properties)
    ```
3. Rebuild the program using the `Azure AI Speech Toolkit: Build the Sample App` command.
4. Run the sample with `Azure AI Speech Toolkit: Run the Sample App`. Ensure that the SAS URI for the container has `Read` and `List` permissions.

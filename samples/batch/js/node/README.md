# How to use the Speech Services Batch Transcription API from Node.js

## Download and install the API client library

To execute the sample you need to generate the Javascript library for the REST API which is generated through [Swagger](swagger.io).

Follow these steps for the installation:

1. Go to https://editor.swagger.io.
1. Click **File**, then click **Import URL**.
1. Enter the Swagger URL for the Speech Services API: `https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/export?DocumentFormat=Swagger&ApiName=Speech%20to%20Text%20API%20v3.0`.
1. Click **Generate Client** and select **Javascript**.
1. Save the client library.
1. Extract the downloaded javascript-client-generated.zip somewhere in your file system.
1. Install the extracted javascript-client module in your nodejs environment using npm
`npm install installpath/to/package/javascript-client`


## Install other dependencies

The sample uses the `requests` and `system-sleep` library. You can install it with the command

```bash
npm install system-sleep
npm install request
```

## Locally modify the Swagger client for the issue related to 'File'
Modified node_modules/speech_to_text_api_v30/src/model/PaginatedFiles.constructFromObject as it was throwing an error reference File not found.

```bash
Was : obj.values = ApiClient.convertToType(data['values'], [File]);
Now : obj.values = ApiClient.convertToType(data['values'], ['File']);
```

## Run the sample code

The sample code itself is [main.js] and can be run using Node.js 3.0 or higher.
You will need to adapt the following information to run the sample:

1. Your subscription key and region.
1. The URI of an audio recording in blob storage.
1. (Optional:) The URI of a container with audio files if you want to transcribe all of them with a single request.

You can use a development environment like Visual Studio Code to edit, debug, and execute the sample.


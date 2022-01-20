# How to use the Speech Services Batch Transcription API from Python

## Download and install the API client library

To execute the sample you need to generate the Python library for the REST API which is generated through [Swagger](swagger.io).

Follow these steps for the installation:

1. Go to https://editor.swagger.io.
1. Click **File**, then click **Import URL**.
1. Enter the Swagger URL for the Speech Services API: `https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/export?DocumentFormat=Swagger&ApiName=Speech%20to%20Text%20API%20v3.0`.
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

1. Your subscription key and region.
   
   The code needs a Subscription Key, this is *not* your Azure Subscription key, but the Cognitive service key. Please make sure you are using the subscription key for your Cognitive Services or Speech resource, and not the subscription id of Azure. You can get the key from the "Keys and Endpoint" tab on your resource in the Azure Portal.

2. The URI of an audio recording in blob storage.
3. (Optional:) The model ID of an adapted model, if you want to use a custom model.
4. (Optional:) The URI of a container with audio files if you want to transcribe all of them with a single request.
   
   Container name can be defined simply as follows:
   
   RECORDINGS_CONTAINER_URI = "https://YourStorageName.blob.core.windows.net/FolderName" 

   On Azure portal, go to 'Access Policy' of the folder and give 'read permission'. Make sure to setup proper 'Access Level' too.

   However, destinationContainerUrl in properties, should have  proper URL with permission defined on them which can be created by going to container, and selecting "Shared access tokens" from settings. And then choose permissions from the drop down and then click on "Generate SAS token and URL". Copy the Blolb SAS URL and paste in the code where destinationContainerUrl is needed. 


You can use a development environment like PyCharm to edit, debug, and execute the sample.

## NOTE

Code ask for Service Region, In Azure portal you may see the region name as "East US" with a space, but you have to add "eastus" in code. [Here](https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/regions#rest-apis) is lists all the supported Azure regions for REST APIs. 

Batch transcription is only available with a standard (paid) subscription. Free subscription will not work for batch. 







# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import requests
import os
import logging
import json
import warnings
warnings.filterwarnings("ignore")
from azure.storage.blob import BlobServiceClient
from azure.identity import DefaultAzureCredential


# Your Credentials and secrets - You should create a .env file and store your private credentials
SUBSCRIPTION_KEY = "YourSubscriptionKey"
SERVICE_REGION = "YourServiceRegion"
STORAGE_ACCOUNT_NAME = 'YourStorageAccountName'
INPUT_CONTAINER =  'YourBlobContainerName' #This is the input container with audio recordings 
OUTPUT_CONTAINER = 'YourOutputBlobContainer'
RECORDINGS_BLOB_URL = f"https://{STORAGE_ACCOUNT_NAME}.blob.core.windows.net" #"Your storage blob url"
ENPOINT_URL = f"https://{SERVICE_REGION}.api.cognitive.microsoft.com/speechtotext/transcriptions:transcribe?api-version=2024-05-15-preview"

# Fast transcription description properties
LOCALE = ["en-US"]
PROFANITYFILTER = "Masked"
DIARIZATIONSETTINGS =  {"minSpeakers": 1, "maxSpeakers": 4}

def get_transcription_properties():
    #transcription "definition" payload 
    transcription_definition = {
        "profanityFilterMode": PROFANITYFILTER,
        "channels": [0], #Change to one channel for Mono-speaker diarization
        "diarizationSettings": DIARIZATIONSETTINGS,  # Optional
         }
        
    #transcription properties 
    transcription_properties = {
            "locales": LOCALE,
            "properties": transcription_definition
        }
    return transcription_properties

def transcribe_from_local(audio_file_path, api_key):
    """
    Transcribe a local file using the settings specified in `properties`
    using the base model for the specified locale.
    
    files (dict): Dictionary of Multi-part form data to send.
    audio_file_path (str): Path to the file to be uploaded.
    """
    #Note: Content-Type should not be set manually as requests will handle it 
    headers = {
      "Ocp-Apim-Subscription-Key": api_key
    }

    # Check if the file exists in the specified file path
    if not os.path.exists(audio_file_path):
        print("File does not exist at the specified path")
        return
    else:    
        #transcription properties payload 
        transcription_properties = get_transcription_properties()

        #Multipart transcription files payload 
        files = {
        'audio': ('test_call_audio.wav', open(audio_file_path, 'rb'), 'audio/wav'),
        # 'audio': ('test_call_audio2.wav', open(audio_file_path2, 'rb'), 'audio/wav'), #add multiple audio files
        'definition': (None, json.dumps(transcription_properties), 'application/json')
        }

        endpoint_url = ENPOINT_URL

        logging.info("Starting transcription client...")
        
        response = requests.post(endpoint_url, headers=headers, files=files)
        print(response.status_code)
        response_json = json.dumps(response.json(), indent=4)
        # print(response.json())

        if response.status_code == 200:
            response_json = response.json()
            # Write response to a JSON file
            with open("transcription_response.json", "w") as json_file:
                json.dump(response_json, json_file, indent=4)


def get_blob_container_url(storage_account_url, container_name):
    """
    Retrieve the URL of a Blob Container within Azure Blob Storage.

    storage_account_url (str): The base URL of the Azure Storage Account.
    container_name (str): The name of the container whose URL is to be retrieved.
    Returns:
    str: The URL of the specified Blob Container.
    """
    try:
        # Create the BlobServiceClient object
        blob_service_client = BlobServiceClient(account_url=storage_account_url)
        
        # Get the container client using the container name
        container_client = blob_service_client.get_container_client(container_name)
        
        # Check if the container exists
        if container_client.exists():
            # Return the Blob Container URL
            container_url = container_client.url
            print(f"Container URL: {container_url}")
            return container_url
        else:
            print("Container not found.")
            return None
    except Exception as e:
        print(f"An error occurred: {e}")
        return None
    


def transcribe_from_blob(blob_storage_url, api_key):
    """
    Transcribe all files in the container located at blob_storage_url using the settings specified in `properties`.
    """  
    storage_account_url = blob_storage_url
    container_name = INPUT_CONTAINER #'YourInputContainerName'


    cred = DefaultAzureCredential()
    blob_service_client = BlobServiceClient(storage_account_url,credential=cred)

    #use the client to connect to the container
    container_client = blob_service_client.get_container_client(container_name)

    #get a list of all blob files in the container
    blob_list = []
    for blob_i in container_client.list_blobs():
        blob_list.append(blob_i.name)
        blob_client = container_client.get_blob_client(blob_i.name)
        stream = blob_client.download_blob()
        data = stream.readall()    

    #Note: Content-Type should not be set manually as requests will handle it
    headers = {
      "Ocp-Apim-Subscription-Key": api_key
    }

    #transcription properties payload 
    transcription_properties = get_transcription_properties()
    #Multipart transcription files payload 
    files = {
        'audio': ('test_call_audio.wav', data, 'audio/wav'),
        'definition': (None, json.dumps(transcription_properties), 'application/json')
        }


    endpoint_url = ENPOINT_URL

    logging.info("Starting transcription client...")
        
    response = requests.post(endpoint_url, headers=headers, files=files)
    print(response.status_code)
    response_json = json.dumps(response.json(), indent=4)
    # print(response_json)

    # Create the container if it does not already exist
    try:
        output_container_name = OUTPUT_CONTAINER
        container_client = blob_service_client.get_container_client(output_container_name)
        container_client.create_container()  # This will do nothing if container already exists
        print(f"Container {output_container_name} created or already exists.")
    except Exception as e:
        print(f"Could not create container: {str(e)}")


    # Get a client for the target blob and upload the JSON response
    if response.status_code == 200:
        output_blob_name = f"{OUTPUT_CONTAINER}_transcriptions"
        blob_service_client = container_client.get_blob_client(output_blob_name)
        blob_service_client.upload_blob(response_json, overwrite=True)
        print(f"Response JSON has been uploaded to {output_blob_name} in {output_container_name}.")
    else:
        print(f"Failed to get valid response from API: {response.status_code}, {response.text}")


if __name__ == "__main__":  

    #Test 1: Using audio file from your local storage
    audio_file_path = "test_call_audio.wav" #"Your_audio_file_path"
    transcribe_from_local(audio_file_path, SUBSCRIPTION_KEY)

    ## Test 2: Using Azure blob storage container
    # cred = DefaultAzureCredential() 
    # transcribe_from_blob(RECORDINGS_BLOB_URL, SUBSCRIPTION_KEY)
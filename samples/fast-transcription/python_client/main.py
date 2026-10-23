# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import requests
import json
import os
import logging
import sys


logging.basicConfig(stream=sys.stdout, level=logging.DEBUG,
        format="%(asctime)s %(message)s", datefmt="%m/%d/%Y %I:%M:%S %p %Z")

# Your credentials and secrets - You should create a .env file and store your private credentials
SUBSCRIPTION_KEY = "YourSubscriptionKey"
SERVICE_REGION = "YourServiceRegion"

NAME = "Fast transcription"
DESCRIPTION = "Fast transcription description"

ENPOINT_URL = f"https://{SERVICE_REGION}.api.cognitive.microsoft.com/speechtotext/transcriptions:transcribe?api-version=2024-05-15-preview"

# Fast transcription description properties
LOCALE = ["en-US"]
PROFANITYFILTER = "Masked"
DIARIZATIONSETTINGS =  {"minSpeakers": 1, "maxSpeakers": 4}

#Fast transcription properties payload
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

def transcribe_from_local(audio_file_path, transcriptions_file_path, api_key):
    """
    Transcribe a local file using the settings specified in `properties`
    using the base model for the specified locale.
    
    files (dict): Dictionary of Multi-part form data to send.
    audio_file_path (str): Path to the file to be uploaded.
    transcriptions_file_path (str): Path to the output transcriptions JSON.
    """
    # audio_file_path = r"samples\fast-transcription\python_client\test_call_audio.wav"
    #Note: Content-Type should not be set manually as requests will handle it 
    headers = {
      "Ocp-Apim-Subscription-Key": api_key
    }

    # Check if the file exists in the specified file path
    if not os.path.exists(audio_file_path):
        print(audio_file_path)
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
            with open(transcriptions_file_path, "w") as json_file:
                json.dump(response_json, json_file, indent=4)

if __name__ == "__main__":  
    import argparse

    parser = argparse.ArgumentParser(description=DESCRIPTION, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument(
        "--input_dir",
        required=False,
        type=str,
        default= "test_call_audio.wav", #update to your local audio file directory
        help="Input directory",
    )
    parser.add_argument(
        "--output_dir",
        required=False,
        type=str,
        default="transcription_response.json", #update to your local transcriptions directory   
        help="Output directory",
    )
    parser.add_argument(
        "--loglevel",
        default="info",
        help="Provide logging level. Example --loglevel debug, default=info",
    )

    opts, args = parser.parse_known_args()
    logging.basicConfig(level=opts.loglevel.upper())

    #transcribe audio from a local file
    transcribe_from_local(opts.input_dir, opts.output_dir, SUBSCRIPTION_KEY)

#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import json
import logging
import os
import sys
import time
import uuid
from pathlib import Path

from azure.identity import DefaultAzureCredential
import requests

logging.basicConfig(stream=sys.stdout, level=logging.INFO,  # set to logging.DEBUG for verbose output
        format="[%(asctime)s] %(message)s", datefmt="%m/%d/%Y %I:%M:%S %p %Z")
logger = logging.getLogger(__name__)

# The endpoint (and key) could be gotten from the Keys and Endpoint page in the Speech service resource.
# The endpoint would be like: https://<region>.api.cognitive.microsoft.com or https://<custom_domain>.cognitiveservices.azure.com
# If you want to use passwordless authentication, custom domain is required.
SPEECH_ENDPOINT = os.environ.get('SPEECH_ENDPOINT')
# We recommend to use passwordless authentication with Azure Identity here; meanwhile, you can also use a subscription key instead
PASSWORDLESS_AUTHENTICATION = True
if not SPEECH_ENDPOINT:
    if PASSWORDLESS_AUTHENTICATION:
        CUSTOM_DOMAIN = os.environ.get('CUSTOM_DOMAIN')
        if not CUSTOM_DOMAIN:
            logger.error('SPEECH_ENDPOINT is required for passwordless authentication')
            sys.exit(1)
        SPEECH_ENDPOINT = f'https://{CUSTOM_DOMAIN}.cognitiveservices.azure.com'
    else:
        SERVICE_REGION = os.environ.get('SPEECH_REGION')
        SPEECH_ENDPOINT = f'https://{SERVICE_REGION}.api.cognitive.microsoft.com'
if not PASSWORDLESS_AUTHENTICATION:
    SUBSCRIPTION_KEY = os.environ.get('SPEECH_KEY')


API_VERSION = "2024-04-01"

def _create_job_id():
    # the job ID must be unique in current speech resource
    # you can use a GUID or a self-increasing number
    return uuid.uuid4()


def _authenticate():
    if PASSWORDLESS_AUTHENTICATION:
        # Refer to https://learn.microsoft.com/python/api/overview/azure/identity-readme?view=azure-python#defaultazurecredential
        # for more information about Azure Identity
        # For example, your app can authenticate using your Azure CLI sign-in credentials with when developing locally.
        # Your app can then use a managed identity once it has been deployed to Azure. No code changes are required for this transition.

        # When developing locally, make sure that the user account that is accessing batch avatar synthesis has the right permission.
        # You'll need Cognitive Services User or Cognitive Services Speech User role to submit batch avatar synthesis jobs.
        credential = DefaultAzureCredential()
        token = credential.get_token('https://cognitiveservices.azure.com/.default')
        return {'Authorization': f'Bearer {token.token}'}
    else:
        return {'Ocp-Apim-Subscription-Key': SUBSCRIPTION_KEY}


def submit_synthesis(job_id: str) -> bool:
    url = f'{SPEECH_ENDPOINT}/texttospeech/batchsyntheses/{job_id}?api-version={API_VERSION}'
    header = {
        'Content-Type': 'application/json'
    }
    header.update(_authenticate())

    with open(Path(__file__).absolute().parent / 'Gatsby-chapter1.txt', 'r', encoding='utf-8') as f:
        text = f.read()

    payload = {
        "inputKind": "PlainText", # or SSML
        'synthesisConfig': {
            "voice": "en-US-AvaMultilingualNeural",
        },
        # Replace with your custom voice name and deployment ID if you want to use custom voice.
        # Multiple voices are supported, the mixture of custom voices and platform voices is allowed.
        # Invalid voice name or deployment ID will be rejected.
        'customVoices': {
            # "YOUR_CUSTOM_VOICE_NAME": "YOUR_CUSTOM_VOICE_ID"
        },
        "inputs": [
            {
                "content": text
            },
        ],
        "properties": {
            "outputFormat": "audio-24khz-160kbitrate-mono-mp3",
            # "destinationContainerUrl": "<blob container url with SAS token>"
        },
    }

    response = requests.put(url, json.dumps(payload), headers=header)
    if response.status_code < 400:
        logger.info('Batch synthesis job submitted successfully')
        logger.info(f'Job ID: {response.json()["id"]}')
        return True
    else:
        logger.error(f'Failed to submit batch synthesis job: [{response.status_code}], {response.text}')
        return False


def get_synthesis(job_id: str):
    url = f'{SPEECH_ENDPOINT}/texttospeech/batchsyntheses/{job_id}?api-version={API_VERSION}'
    header = _authenticate()
    response = requests.get(url, headers=header)
    if response.status_code < 400:
        logger.info('Get batch synthesis job successfully')
        logger.info(response.json())
        return response.json()['status']
    else:
        logger.error(f'Failed to get batch synthesis job: {response.text}')


def list_synthesis_jobs(skip: int = 0, max_page_size: int = 100):
    """List all batch synthesis jobs in the subscription"""
    url = f'{SPEECH_ENDPOINT}/texttospeech/batchsyntheses?api-version={API_VERSION}&skip={skip}&maxpagesize={max_page_size}'
    header = _authenticate()
    response = requests.get(url, headers=header)
    if response.status_code < 400:
        logger.info(f'List batch synthesis jobs successfully, got {len(response.json()["values"])} jobs')
        logger.info(response.json())
    else:
        logger.error(f'Failed to list batch synthesis jobs: {response.text}')


if __name__ == '__main__':
    job_id = _create_job_id()
    if submit_synthesis(job_id):
        while True:
            status = get_synthesis(job_id)
            if status == 'Succeeded':
                logger.info('batch synthesis job succeeded')
                break
            elif status == 'Failed':
                logger.error('batch synthesis job failed')
                break
            else:
                logger.info(f'batch synthesis job is still running, status [{status}]')
                time.sleep(5)

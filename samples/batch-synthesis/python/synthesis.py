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

import requests

logging.basicConfig(stream=sys.stdout, level=logging.INFO,  # set to logging.DEBUG for verbose output
        format="[%(asctime)s] %(message)s", datefmt="%m/%d/%Y %I:%M:%S %p %Z")
logger = logging.getLogger(__name__)

# The endpoint (and key) could be gotten from the Keys and Endpoint page in the Speech service resource.
# The endpoint would be like: https://<region>.api.cognitive.microsoft.com or https://<custom_domain>.cognitiveservices.azure.com
SUBSCRIPTION_KEY = os.environ.get('SPEECH_KEY')
SPEECH_ENDPOINT = os.environ.get('SPEECH_ENDPOINT')
if not SPEECH_ENDPOINT:
    SERVICE_REGION = os.environ.get('SPEECH_REGION')
    SPEECH_ENDPOINT = f'https://{SERVICE_REGION}.api.cognitive.microsoft.com'

API_VERSION = "2024-04-01"

def _create_job_id():
    # the job ID must be unique in current speech resource
    # you can use a GUID or a self-increasing number
    return uuid.uuid4()


def submit_synthesis(job_id: str) -> bool:
    url = f'{SPEECH_ENDPOINT}/texttospeech/batchsyntheses/{job_id}?api-version={API_VERSION}'
    header = {
        'Ocp-Apim-Subscription-Key': SUBSCRIPTION_KEY,
        'Content-Type': 'application/json'
    }

    with open(Path(__file__).absolute().parent.parent / 'Gatsby-chapter1.txt', 'r') as f:
        text = f.read()

    payload = {
        "inputKind": "PlainText",
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
        logger.error(f'Failed to submit batch synthesis job: {response.text}')
        return False


def get_synthesis(job_id: str):
    url = f'{SPEECH_ENDPOINT}/texttospeech/batchsyntheses/{job_id}?api-version={API_VERSION}'
    header = {
        'Ocp-Apim-Subscription-Key': SUBSCRIPTION_KEY
    }
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
    header = {
        'Ocp-Apim-Subscription-Key': SUBSCRIPTION_KEY
    }
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

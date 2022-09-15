#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import json
import logging
from pathlib import Path
import requests
import sys
import time

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG,
        format="[%(asctime)s] %(message)s", datefmt="%m/%d/%Y %I:%M:%S %p %Z")
logger = logging.getLogger(__name__)

# Your subscription key and region for the speech service

SUBSCRIPTION_KEY = "YourSubscriptionKey"
SERVICE_REGION = "YourServiceRegion"

NAME = "Simple synthesis"
DESCRIPTION = "Simple synthesis description"


def submit_synthesis():
    url = 'https://{}.customvoice.api.speech.microsoft.com/api/texttospeech/3.1-preview1/batchsynthesis'.format(
        SERVICE_REGION)
    header = {
        'Ocp-Apim-Subscription-Key': SUBSCRIPTION_KEY,
        'Content-Type': 'application/json'
    }

    with open(Path(__file__).absolute().parent.parent / 'Gatsby-chapter1.txt', 'r') as f:
        text = f.read()

    payload = {
        'displayName': NAME,
        'description': DESCRIPTION,
        "textType": "PlainText",
        'synthesisConfig': {
            "voice": "en-US-JennyNeural",
        },
        "inputs": [
            {
                "text": text
            },
        ],
        "properties": {
            "outputFormat": "audio-24khz-160kbitrate-mono-mp3",
        },
    }


    response = requests.post(url, json.dumps(payload), headers=header)
    if response.status_code < 400:
        logger.info('Batch synthesis job submitted successfully')
        logger.info(f'Job ID: {response.json()["id"]}')
        return response.json()["id"]
    else:
        logger.error(f'Failed to submit batch synthesis job: {response.text}')


def get_synthesis(job_id):
    url = f'https://{SERVICE_REGION}.customvoice.api.speech.microsoft.com/api/texttospeech/3.1-preview1/batchsynthesis/{job_id}'
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

def list_synthesis_jobs(skip: int = 0, top: int = 100):
    """List all batch synthesis jobs in the subscription"""
    url = f'https://{SERVICE_REGION}.customvoice.api.speech.microsoft.com/api/texttospeech/3.1-preview1/batchsynthesis?skip={skip}&top={top}'
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
    job_id = submit_synthesis()
    if job_id is not None:
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
#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

from time import sleep
import uuid
import os
import datetime
import logging
from dotenv import load_dotenv
try:
    import customvoice
except ImportError:
    print('Please copy folder https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/'
          'samples/custom-voice/python/customvoice and keep the same folder structure as github.')
    quit()

# Settings are loaded from environment variables.
# Copy ../.env.sample to ../.env and fill in your values.
# See ../.env.sample for the full list of available variables.
# Load .env file from parent directory (samples/custom-voice/.env).
# This also works when running from the command line: python professional_voice_sample.py
_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
load_dotenv(os.path.join(_SCRIPT_DIR, '..', '.env'))

# Speech resource
REGION = os.environ.get('SPEECH_REGION', 'eastus')
SPEECH_KEY = os.environ.get('SPEECH_KEY', '')

# Voice consent file path
CONSENT_FILE_PATH = os.environ.get('CONSENT_FILE_PATH', os.path.join(_SCRIPT_DIR, 'TestData', 'VoiceTalentVerbalStatement.wav'))

# Training data folder (audio .wav + script .txt)
TRAINING_DATA_FOLDER = os.environ.get('TRAINING_DATA_FOLDER', '')

# Azure Blob Storage
BLOB_ACCOUNT_URL = os.environ.get('BLOB_ACCOUNT_URL', '')
BLOB_ACCOUNT_KEY = os.environ.get('BLOB_ACCOUNT_KEY', '')
BLOB_CONTAINER_NAME = os.environ.get('BLOB_CONTAINER_NAME', 'voicedata')

# Voice settings
VOICE_TALENT_NAME = os.environ.get('VOICE_TALENT_NAME', 'Sample Voice Actor')
COMPANY_NAME = os.environ.get('COMPANY_NAME', 'Contoso')
VOICE_NAME = os.environ.get('VOICE_NAME', 'SampleVoiceNeural')
RECIPE_KIND = os.environ.get('RECIPE_KIND', 'Default')

# Resource IDs (can keep defaults)
LOCALE = os.environ.get('LOCALE', 'en-us')
PROJECT_ID = os.environ.get('PROFESSIONAL_PROJECT_ID', 'professional-voice-project-1')
CONSENT_ID = os.environ.get('PROFESSIONAL_CONSENT_ID', 'professional-voice-consent-1')
TRAINING_SET_ID = os.environ.get('PROFESSIONAL_TRAINING_SET_ID', 'professional-voice-trainingset-1')
MODEL_ID = os.environ.get('PROFESSIONAL_MODEL_ID', 'professional-voice-model-1')
ENDPOINT_ID = os.environ.get('PROFESSIONAL_ENDPOINT_ID', str(uuid.uuid4()))

# ==================================================================================


def create_project():
    project = customvoice.Project.create(config, PROJECT_ID, customvoice.ProjectKind.ProfessionalVoice, locale=LOCALE)
    print('Project created. project id: %s' % project.id)


def upload_consent():
    consent = customvoice.Consent.create(config, PROJECT_ID, CONSENT_ID,
                                         voice_talent_name=VOICE_TALENT_NAME,
                                         company_name=COMPANY_NAME,
                                         audio_file_path=CONSENT_FILE_PATH,
                                         locale=LOCALE)
    print('Consent created. consent id: %s' % consent.id)


# Upload wave and script file in local_folder to Azure blob under blob_url/container_name/blob_prefix/
# If container_name doesn't exist, will create a new container.
def upload_training_set_to_blob(local_folder: str):
    blob_prefix = str(uuid.uuid4()) + '/'
    message = 'Uploading data from %s to %s/%s/%s' % (local_folder, BLOB_ACCOUNT_URL, BLOB_CONTAINER_NAME, blob_prefix)
    print(message)

    from azure.storage.blob import BlobServiceClient, generate_container_sas, ContainerSasPermissions
    from azure.core.exceptions import ResourceNotFoundError
    blob_service_client = BlobServiceClient(account_url=BLOB_ACCOUNT_URL, credential=BLOB_ACCOUNT_KEY)
    container_client = blob_service_client.get_container_client(BLOB_CONTAINER_NAME)
    try:
        container_client.get_container_properties()
    except ResourceNotFoundError:
        container_client.create_container()

    for file_name in os.listdir(local_folder):
        file_path = os.path.join(local_folder, file_name)
        print('uploading ' + file_name)
        with open(file_path, "rb") as data:
            container_client.upload_blob(name=blob_prefix+file_name, data=data)

    sas_token = generate_container_sas(
                container_client.account_name,
                container_client.container_name,
                account_key=container_client.credential.account_key,
                permission=ContainerSasPermissions(read=True, list=True),
                expiry=datetime.datetime.utcnow() + datetime.timedelta(hours=1)
            )
    return blob_prefix, sas_token


def upload_training_set():
    # Create training set
    training_set = customvoice.TrainingSet.create(config, PROJECT_ID, TRAINING_SET_ID, locale=LOCALE)

    # Upload wave and script from local folder to Azure blob.
    blob_prefix, sas_token = upload_training_set_to_blob(TRAINING_DATA_FOLDER)

    # Upload data to training set
    container_url_with_sas = BLOB_ACCOUNT_URL + '/' + BLOB_CONTAINER_NAME + '?' + sas_token
    audios = customvoice.AzureBlobContentSource(container_url_with_sas, blob_prefix, ['.wav'])
    scripts = customvoice.AzureBlobContentSource(container_url_with_sas, blob_prefix, ['.txt'])
    customvoice.TrainingSet.upload_data(config, TRAINING_SET_ID, kind=customvoice.DatasetKind.AudioAndScript,
                                        audios=audios, scripts=scripts)

    # Wait for training set ready
    print('Training set is processing in server. It takes around 5 minutes to 1 hour depending on data size.')
    training_set = customvoice.TrainingSet.get(config, TRAINING_SET_ID)
    while training_set.status != customvoice.Status.Succeeded and training_set.status != customvoice.Status.Failed:
        print('.', end='', flush=True)
        sleep(10)
        training_set = customvoice.TrainingSet.get(config, TRAINING_SET_ID)
    if training_set.status == customvoice.Status.Failed:
        print('Training set failed')
        raise Exception
    elif training_set.status == customvoice.Status.Succeeded:
        print('Training set succeeded')


def train_model():
    model = customvoice.Model.create(config, PROJECT_ID, MODEL_ID,
                                     voice_name=VOICE_NAME,
                                     recipe_kind=RECIPE_KIND,
                                     consent_id=CONSENT_ID,
                                     training_set_id=TRAINING_SET_ID)
    print('Started model training. model id: %s' % model.id)

    # Wait for model ready
    print('Model is training in server. It takes around 24 hours.')
    while model.status != customvoice.Status.Succeeded and model.status != customvoice.Status.Failed:
        print('.', end='', flush=True)
        sleep(300)
        model = customvoice.Model.get(config, MODEL_ID)
    if model.status == customvoice.Status.Failed:
        print('Model training failed. Failure reason: %s' % model.failure_reason)
        raise Exception
    elif model.status == customvoice.Status.Succeeded:
        print('Model training succeeded')


def deploy_model():
    endpoint = customvoice.Endpoint.create(config, PROJECT_ID, ENDPOINT_ID, MODEL_ID)
    print('Start deploying model. endpoint id: %s' % endpoint.id)

    # Wait for model deploy
    print('Deploying model. It takes around 1 to 5 minutes.')
    while endpoint.status != customvoice.Status.Succeeded and endpoint.status != customvoice.Status.Failed:
        print('.', end='', flush=True)
        sleep(10)
        endpoint = customvoice.Endpoint.get(config, ENDPOINT_ID)
    if endpoint.status == customvoice.Status.Failed:
        print('Model deploy failed')
        raise Exception
    elif endpoint.status == customvoice.Status.Succeeded:
        print('Model deploy succeeded')


if __name__ == '__main__':
    # Validate required settings
    missing = []
    if not SPEECH_KEY:
        missing.append('SPEECH_KEY')
    if not TRAINING_DATA_FOLDER:
        missing.append('TRAINING_DATA_FOLDER')
    if not BLOB_ACCOUNT_URL:
        missing.append('BLOB_ACCOUNT_URL')
    if not BLOB_ACCOUNT_KEY:
        missing.append('BLOB_ACCOUNT_KEY')
    if missing:
        print('ERROR: The following required environment variables are not set:')
        for var in missing:
            print('  - %s' % var)
        print()
        print('Please copy .env.sample to .env and fill in your values, or set them in .vscode/launch.json.')
        quit()

    logging.basicConfig(filename="customvoice.log",
                        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
                        filemode='w')
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)

    config = customvoice.Config(SPEECH_KEY, REGION, logger)

    # step 1: create project
    create_project()

    # step 2: upload consent file
    upload_consent()

    # step 3: upload training set
    upload_training_set()

    # step 4: train model
    train_model()

    # step 5: deploy model
    deploy_model()

    # step 6: synthesis with endpoint
    # You can find sample code here
    # https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/d806061d8ab00e9729128839bf5051c4871ab35f/samples/python/console/speech_synthesis_sample.py#LL119C1-L119C43

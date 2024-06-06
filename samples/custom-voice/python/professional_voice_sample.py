#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import json
from time import sleep
import uuid
import os
import datetime
import logging
try:
    import customvoice
except ImportError:
    print('Pleae copy folder https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/custom-voice/python/customvoice and keep the same folder structure as github.' )
    quit()


def create_project():
    project = customvoice.Project.create(config, project_id, customvoice.ProjectKind.ProfessionalVoice)
    print('Project created. project id: %s' % project.id)


def upload_consent():
    # Custom Voice training need voice consent file with template below.
    # I [voice talent name] am aware that recordings of my voice will be used by [company name] to create and use a synthetic version of my voice.
    # You can find sample consent file here
    # https://github.com/Azure-Samples/Cognitive-Speech-TTS/blob/master/CustomVoice/Sample%20Data/Individual%20utterances%20%2B%20matching%20script/VoiceTalentVerbalStatement.wav
    consent_file_path = r'D:\CNV_API\VoiceTalentVerbalStatement.wav'
    consent = customvoice.Consent.create(config, project_id, consent_id,
                                         voice_talent_name = 'Sample Voice Actor',
                                         company_name = 'Contoso',
                                         audio_file_path = consent_file_path,
                                         locale = 'en-us')
    print('Consent created. consent id: %s' % consent.id)


# Upload wave and script file in local_folder to Azure blob under https://blob_url/container_name/blob_prefix/
# If container_name doesnt exists, will create a new container.
def upload_training_set_to_blob(blob_url: str, blob_key: str, container_name: str, blob_prefix: str, local_folder: str):
    message = 'Uploading data from ' + local_folder + ' to https://' + blob_url + '/' + container_name + '/' + blob_prefix
    print(message)

    from azure.storage.blob import BlobServiceClient, generate_container_sas, ContainerSasPermissions 
    from azure.core.exceptions import ResourceNotFoundError, ResourceExistsError
    blob_service_client = BlobServiceClient(account_url=blob_url, credential=blob_key)
    container_client = blob_service_client.get_container_client(container_name)
    try:
        properties = container_client.get_container_properties()
    except ResourceNotFoundError:
        container_client.create_container()
        properties = container_client.get_container_properties()

    for file_name in os.listdir(local_folder):
        file_path = os.path.join(local_folder, file_name)
        print('uploading ' + file_name)
        with open(file_path, "rb") as data:
            container_client.upload_blob(name=blob_prefix+file_name, data=data)

    sas_token = generate_container_sas(
                container_client.account_name,
                container_client.container_name,
                account_key=container_client.credential.account_key,
                permission = ContainerSasPermissions(read = True, list = True),
                expiry=datetime.datetime.utcnow() + datetime.timedelta(hours=1)
            )
    return sas_token

def upload_training_set():
    # Create training set
    training_set = customvoice.TrainingSet.create(config, project_id, training_set_id, locale = 'en-US')

    # Upload wave and script from local_folder to Azure blob.
    blob_url = 'contoso.blob.core.windows.net'
    blob_key = '<put your Azure Storage accout key here>'
    container_name = 'voicedata'
    blob_prefix = str(uuid.uuid4()) + '/'
    print('blob_prefix: %s' % blob_prefix)
    # You can find sample script and audio file here.
    # https://github.com/Azure-Samples/Cognitive-Speech-TTS/blob/master/CustomVoice/Sample%20Data/Individual%20utterances%20%2B%20matching%20script/SampleScript.txt
    # https://github.com/Azure-Samples/Cognitive-Speech-TTS/blob/master/CustomVoice/Sample%20Data/Individual%20utterances%20%2B%20matching%20script/SampleAudios.zip
    # Pleae unzip audio file. Put both audio and script file in foler below.
    local_folder = 'D:\CNV_API\SampleAudios'
    sas_token = upload_training_set_to_blob(blob_url, blob_key, container_name, blob_prefix, local_folder)
    print('sas_token: %s' % sas_token)

    # Upload data to training et
    containeUrlWithSAS = "https://" + blob_url + '/' + container_name + '?' + sas_token
    print('containeUrl: %s' % containeUrlWithSAS)
    audios = customvoice.AzureBlobContentSource(containeUrlWithSAS, blob_prefix, ['.wav'])
    scripts = customvoice.AzureBlobContentSource(containeUrlWithSAS, blob_prefix, ['.txt'])
    customvoice.TrainingSet.upload_data(config, training_set_id, kind = customvoice.DatasetKind.AudioAndScript, audios = audios, scripts = scripts)

    # Wait for training set ready
    print('Training set is processing in server. It takes around 5 minutes to 1 hour depend on data size.')
    training_set = customvoice.TrainingSet.get(config, training_set_id)
    while training_set.status != customvoice.Status.Succeeded and training_set.status != customvoice.Status.Failed:
        print('.', end='', flush=True)
        sleep(10)
        training_set = customvoice.TrainingSet.get(config, training_set_id)
    if training_set.status == customvoice.Status.Failed:
        print('Training set failed')
        raise Exception
    elif training_set.status == customvoice.Status.Succeeded:
        print('Training set succeeded')


def train_model():
    model = customvoice.Model.create(config, project_id, model_id,
                                     voice_name='SampleVoiceNeural',
                                     recipe_kind = customvoice.RecipeKind.Default.name,
                                     consent_id=consent_id,
                                     training_set_id=training_set_id)
    print('Started model training. model id: %s' % model.id)

    # Wait for model ready
    print('Model is training in server. It takes around 24 hours.')
    while model.status != customvoice.Status.Succeeded and model.status != customvoice.Status.Failed:
        print('.', end='', flush=True)
        sleep(300)
        model = customvoice.Model.get(config, model_id)
    if model.status == customvoice.Status.Failed:
        print('Model training failed. Failure reason: %s' % model.failure_reason)
        raise Exception
    elif model.status == customvoice.Status.Succeeded:
        print('Model training succeeded')


def deploy_model():
    endpoint = customvoice.Endpoint.create(config, project_id, endpoint_id, model_id)
    print('Start deploying model . endpoint id: %s' % endpoint.id)

    # Wait for model deploy
    print('Deploying model. It takes around 1 to 5 minutes.')
    while endpoint.status != customvoice.Status.Succeeded and endpoint.status != customvoice.Status.Failed:
        print('.', end='', flush=True)
        sleep(10)
        endpoint = customvoice.Endpoint.get(config, endpoint_id)
    if endpoint.status == customvoice.Status.Failed:
        print('Model deploy failed')
        raise Exception
    elif endpoint.status == customvoice.Status.Succeeded:
        print('Model deploy succeeded')


region = 'eastus' # eastus, westeurope, southeastasia
key = '<put your speech accout key here>'

logging.basicConfig(filename="customvoice.log",
                    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
                    filemode='w')
logger = logging.getLogger()
logger.setLevel(logging.DEBUG)

config = customvoice.Config(key, region, logger)


project_id = 'professional-voice-project-1'
consent_id = 'professional-voice-consent-1'
training_set_id  = 'professional-voice-trainingset-1'
model_id  = 'professional-voice-model-1'
endpoint_id  = str(uuid.uuid4())

# step 1: creat project
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

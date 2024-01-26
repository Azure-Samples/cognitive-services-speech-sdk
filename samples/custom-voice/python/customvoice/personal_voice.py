#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import json
import requests
import logging
from time import sleep

from .helper import *
from .config import Config
from .customvoice_object import CustomVoiceObject
from .status_object import *


class PersonalVoice(StatusObject):
    def __init__(self, json_dict: dict):
        super().__init__(json_dict)
        if 'projectId' not in json_dict:
            raise ValueError("could not find 'projectId' in json_dict")
        self.project_id = json_dict['projectId']
        if 'consentId' not in json_dict:
            raise ValueError("could not find 'consentId' in json_dict")
        self.consent_id = json_dict['consentId']
        if 'speakerProfileId' not in json_dict:
            raise ValueError("could not find 'speakerProfileId' in json_dict")
        self.speaker_profile_id = json_dict['speakerProfileId']


    # get all personal voice in project
    # when project_id is None, get all personal voices in current speech account
    @staticmethod
    def list(config: Config, project_id: str = None):
        config.logger.debug('PersonalVoice.list')
        personal_voice_list = []
        api_url = config.url_prefix + 'personalvoices' + '?' + config.api_version
        if project_id is not None and len(project_id) > 0:
            api_url += "&filter=projectId eq '%s'" % project_id
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        while api_url is not None and len(api_url) > 0:
            response = requests.get(api_url, headers=headers)
            raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
            response_dict= response.json()
            for json_dict in response_dict['value']:
                speaker_profile = PersonalVoice(json_dict)
                personal_voice_list.append(speaker_profile)
            if 'nextLink' in response_dict:
                api_url = response_dict['nextLink']
            else:
                api_url = None
        return personal_voice_list


    @staticmethod
    def get(config: Config, personal_voice_id: str):
        config.logger.debug('PersonalVoice.get personal_voice_id = %s' % personal_voice_id)
        if personal_voice_id is None or len(personal_voice_id) == 0:
            raise ValueError("'personal_voice_id' is None or empty")
        api_url = config.url_prefix + 'personalvoices/' + personal_voice_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.get(api_url, headers=headers)
        raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
        personal_voice = PersonalVoice(response.json())
        return personal_voice


    @staticmethod
    def create(config: Config, project_id: str, personal_voice_id: str, consent_id: str, audio_folder: str, description = None):
        config.logger.debug('PersonalVoice.create personal_voice_id = %s' % personal_voice_id)
        if project_id is None or len(project_id) == 0:
            raise ValueError("'project_id' is None or empty")
        if personal_voice_id is None or len(personal_voice_id) == 0:
            raise ValueError("'personal_voice_id' is None or empty")
        if consent_id is None or len(consent_id) == 0:
            raise ValueError("'consent_id' is None or empty")
        if audio_folder is None or len(audio_folder) == 0:
            raise ValueError("'audio_folder' is None or empty")
        if not os.path.isdir(audio_folder):
            raise ValueError("Can't find 'audio_folder' %s" % audio_folder)

        api_url = config.url_prefix + 'personalvoices/' + personal_voice_id + '?' + config.api_version
        request_dict = {
            'description': description,
            'projectId': project_id,
            'consentId': consent_id,
        }

        files = []
        for file_name in os.listdir(audio_folder):
            file_path = os.path.join(audio_folder, file_name)
            file=('audiodata', (file_name, open(file_path, 'rb'), 'audio/wav'))
            files.append(file)

        headers = { 'Ocp-Apim-Subscription-Key': config.key }
        response = requests.post(api_url, data=request_dict, headers=headers, files=files)
        raise_exception_when_reqeust_failed('POST', api_url, response, config.logger)
        personal_voice = PersonalVoice(response.json())

        # Wait for personal voice ready. It takes 1 second.
        while (personal_voice.status != Status.Succeeded and personal_voice.status != Status.Failed):
            sleep(1)
            personal_voice = PersonalVoice.get(config, personal_voice_id)
        if personal_voice.status == Status.Succeeded:
            config.logger.debug('PersonalVoice.create succeeded personal_voice_id = %s' % personal_voice_id)
        elif personal_voice.status == Status.Failed:
            config.logger.debug('PersonalVoice.create failed personal_voice_id = %s' % personal_voice_id)
        return personal_voice


    @staticmethod
    def delete(config: Config, personal_voice_id: str):
        config.logger.debug('PersonalVoice.delete personal_voice_id = %s' % personal_voice_id)
        if personal_voice_id is None or len(personal_voice_id) == 0:
            raise ValueError("'speaker_profile_id' is None or empty")
        api_url = config.url_prefix + 'personalvoices/' + personal_voice_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.delete(api_url, headers=headers)
        raise_exception_when_reqeust_failed('DELETE', api_url, response, config.logger)

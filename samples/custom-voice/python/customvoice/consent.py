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


class Consent(StatusObject):
    def __init__(self, json_dict: dict):
        super().__init__(json_dict)
        if 'voiceTalentName' not in json_dict:
            raise ValueError("could not find 'voiceTalentName' in json_dict")
        self.voice_talent_name = json_dict['voiceTalentName']
        if 'companyName' not in json_dict:
            raise ValueError("could not find 'companyName' in json_dict")
        self.company_name = json_dict['companyName']
        if 'locale' not in json_dict:
            raise ValueError("could not find 'locale' in json_dict")
        self.locale = json_dict['locale']
        if 'projectId' not in json_dict:
            raise ValueError("could not find 'projectId' in json_dict")
        self.project_id = json_dict['projectId']

    # get all consents in project
    # when project_id is None, get all consents in current speech account
    @staticmethod
    def list(config: Config, project_id: str = None):
        config.logger.debug('Consent.list')
        consents = []
        api_url = config.url_prefix + 'consents' + '?' + config.api_version
        if project_id is not None and len(project_id) > 0:
            api_url += "&filter=projectId eq '%s'" % project_id
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        while api_url is not None and len(api_url) > 0:
            response = requests.get(api_url, headers=headers)
            raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
            response_dict= response.json()
            for json_dict in response_dict['value']:
                consent = Consent(json_dict)
                consents.append(consent)
            if 'nextLink' in response_dict:
                api_url = response_dict['nextLink']
            else:
                api_url = None
        return consents

    @staticmethod
    def get(config: Config, consent_id: str):
        config.logger.debug('Consent.get consent_id = %s' % consent_id)
        if consent_id is None or len(consent_id) == 0:
            raise ValueError("'consent_id' is None or empty")
        api_url = config.url_prefix + 'consents/' + consent_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.get(api_url, headers=headers)
        raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
        consent = Consent(response.json())
        return consent

    @staticmethod
    def create(config: Config, project_id: str, consent_id: str, voice_talent_name: str, company_name: str,
               audio_file_path: str, locale: str, description: str = None):
        config.logger.debug('Consent.create consent_id = %s' % consent_id)
        if project_id is None or len(project_id) == 0:
            raise ValueError("'project_id' is None or empty")
        if consent_id is None or len(consent_id) == 0:
            raise ValueError("'consent_id' is None or empty")
        if voice_talent_name is None or len(voice_talent_name) == 0:
            raise ValueError("'voice_talent_name' is None or empty")
        if company_name is None or len(company_name) == 0:
            raise ValueError("'company_name' is None or empty")
        if audio_file_path is None or len(audio_file_path) == 0:
            raise ValueError("'audio_file_path' is None or empty")
        if locale is None or len(locale) == 0:
            raise ValueError("'locale' is None or empty")
        if not os.path.exists(audio_file_path):
            raise ValueError("can't find file 'audio_file_path' = %s" % audio_file_path)
        audio_file_name = os.path.basename(audio_file_path)

        headers = { 'Ocp-Apim-Subscription-Key': config.key }
        api_url = config.url_prefix + 'consents/' + consent_id + '?' + config.api_version
        request_dict = {
            'projectId': project_id,
            'voiceTalentName': voice_talent_name,
            'companyName': company_name,
            'locale': locale,
            'description': description
        }
        file=('audiodata', (audio_file_name, open(audio_file_path, 'rb'), 'audio/wav'))
        response = requests.post(api_url, data=request_dict, headers=headers, files=[file])
        raise_exception_when_reqeust_failed('POST', api_url, response, config.logger)
        consent = Consent(response.json())
        consent_id = consent.id

        # Wait for consent ready. It takes 2-3 seconds.
        while (consent.status != Status.Succeeded and consent.status != Status.Failed):
            sleep(1)
            consent = Consent.get(config, consent_id)
        if consent.status == Status.Succeeded:
            config.logger.debug('Consent.create succeeded consent_id = %s' % consent_id)
        elif consent.status == Status.Failed:
            config.logger.warning('Consent.create failed consent_id = %s' % consent_id)
        return consent

    @staticmethod
    def delete(config: Config, consent_id: str):
        config.logger.debug('Consent.delete consent_id = %s' % consent_id)
        if consent_id is None or len(consent_id) == 0:
            raise ValueError("'consent_id' is None or empty")
        api_url = config.url_prefix + 'consents/' + consent_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.delete(api_url, headers=headers)
        raise_exception_when_reqeust_failed('DELETE', api_url, response, config.logger)

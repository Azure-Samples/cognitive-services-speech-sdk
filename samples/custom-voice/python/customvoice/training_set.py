#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import json
import requests
import logging
import os
from typing import NamedTuple

from .helper import *
from .config import Config
from .customvoice_object import CustomVoiceObject
from .status_object import *


class DatasetKind(Enum):
    AudioAndScript = 1
    LongAudio = 2
    AudioOnly = 3


class AzureBlobContentSource(NamedTuple):
    containerUrl: str
    prefix: str
    extensions: list[str]


class TrainingSet(StatusObject):
    def __init__(self, json_dict: dict):
        super().__init__(json_dict)
        # Seems no locale property in CNV Lite training set. So, didn't treat locale as required.
        if 'locale' in json_dict:
            self.locale = json_dict['locale']
        else:
            self.locale = ''
        if 'projectId' not in json_dict:
            raise ValueError("could not find 'projectId' in json_dict")
        self.project_id = json_dict['projectId']
        if 'properties' in json_dict:
            properties = json_dict['properties']
            if 'utteranceCount' in properties:
                self.utteranceCount = int(properties['utteranceCount'])


    # get all training sets in project
    # when project_id is None, get all training sets in current speech account
    @staticmethod
    def list(config: Config, project_id: str = None):
        config.logger.debug('TrainingSet.list')
        training_sets = []
        api_url = config.url_prefix + 'trainingsets' + '?' + config.api_version
        if project_id is not None and len(project_id) > 0:
            api_url += "&filter=projectId eq '%s'" % project_id
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        while api_url is not None and len(api_url) > 0:
            response = requests.get(api_url, headers=headers)
            raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
            response_dict= response.json()
            for json_dict in response_dict['value']:
                training_set = TrainingSet(json_dict)
                training_sets.append(training_set)
            if 'nextLink' in response_dict:
                api_url = response_dict['nextLink']
            else:
                api_url = None
        return training_sets


    @staticmethod
    def get(config: Config, training_set_id: str):
        config.logger.debug('TrainingSet.get training_set_id = %s' % training_set_id)
        if training_set_id is None or len(training_set_id) == 0:
            raise ValueError("'training_set_id' is None or empty")
        api_url = config.url_prefix + 'trainingsets/' + training_set_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.get(api_url, headers=headers)
        raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
        training_set = TrainingSet(response.json())
        return training_set


    @staticmethod
    def create(config: Config, project_id: str, training_set_id: str, locale: str, description: str = None):
        config.logger.debug('TrainingSet.create training_set_id = %s' % training_set_id)
        if project_id is None or len(project_id) == 0:
            raise ValueError("'project_id' is None or empty")
        if training_set_id is None or len(training_set_id) == 0:
            raise ValueError("'training_set_id' is None or empty")
        if locale is None or len(locale) == 0:
            raise ValueError("'locale' is None or empty")

        headers = { 'Ocp-Apim-Subscription-Key': config.key }
        api_url = config.url_prefix + 'trainingsets/' + training_set_id + '?' + config.api_version
        request_dict = { 
            'description': description,
            'locale': locale,
            'projectId': project_id,
            }
        response = requests.put(api_url, json=request_dict, headers=headers)
        raise_exception_when_reqeust_failed('PUT', api_url, response, config.logger)
        training_set = TrainingSet(response.json())
        return training_set


    @staticmethod
    def delete(config: Config, training_set_id: str):
        config.logger.debug('TrainingSet.delete training_set_id = %s' % training_set_id)
        if training_set_id is None or len(training_set_id) == 0:
            raise ValueError("'training_set_id' is None or empty")
        api_url = config.url_prefix + 'trainingsets/' + training_set_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.delete(api_url, headers=headers)
        raise_exception_when_reqeust_failed('DELETE', api_url, response, config.logger)


    @staticmethod
    def upload_data(config: Config, training_set_id: str, kind: DatasetKind, audios: AzureBlobContentSource, scripts: AzureBlobContentSource):
        config.logger.debug('TrainingSet.upload_data training_set_id = %s' % training_set_id)
        if training_set_id is None or len(training_set_id) == 0:
            raise ValueError("'training_set_id' is None or empty")
        if audios is None:
            raise ValueError("'audios' is None")
        if (kind == DatasetKind.AudioAndScript or kind == DatasetKind.LongAudio) and scripts is None:
            raise scripts("'audios' is None")

        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        api_url = config.url_prefix + 'trainingsets/' + training_set_id + ':upload' + '?' + config.api_version
        request_dict = {
            'kind': kind.name,
            'audios': {
                'containerUrl': audios.containerUrl,
                'prefix': audios.prefix,
                'extensions': audios.extensions
                }
        }
        if scripts is not None:
            request_dict['scripts'] = {
                'containerUrl': scripts.containerUrl,
                'prefix': scripts.prefix,
                'extensions': scripts.extensions
                }
        response = requests.post(api_url, json=request_dict, headers=headers)
        raise_exception_when_reqeust_failed('POST', api_url, response, config.logger)

#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

from enum import Enum
import json
import requests
import logging
from typing import NamedTuple

from .helper import *
from .config import Config
from .customvoice_object import CustomVoiceObject
from .status_object import StatusObject


class RecipeKind(Enum):
    Default = 1
    CrossLingual = 2
    MultiStyle = 3
    # Lite = 4


class Model(StatusObject):
    def __init__(self, json_dict: dict):
        super().__init__(json_dict)
        if 'locale' not in json_dict:
            raise ValueError("could not find 'locale' in json_dict")
        self.locale = json_dict['locale']
        if 'projectId' not in json_dict:
            raise ValueError("could not find 'projectId' in json_dict")
        self.project_id = json_dict['projectId']
        if 'voiceName' not in json_dict:
            raise ValueError("could not find 'voiceName' in json_dict")
        self.voice_name = json_dict['voiceName']
        # Seems no consentId property in some model. Maybe because it's a copied model. So, didn't treat consentId as required.
        if 'consentId' in json_dict:
            self.consent_id = json_dict['consentId']
        else:
            self.consent_id = ''
        # Seems no trainingSetId property in CNV Lite model. So, didn't treat trainingSetId as required.
        if 'trainingSetId' in json_dict:
            self.training_set_id = json_dict['trainingSetId']
        else:
            self.training_set_id = ''
        # Seems no raise property in CNV Lite model. So, didn't treat raise as required.
        if 'recipe' in json_dict:
            recipe = json_dict['recipe']
            if 'kind' not in recipe:
                raise ValueError("could not find 'kind' in json_dict['recipe']")
            self.recipe_kind = recipe['kind']
            if 'version' in recipe:
                self.recipe_version = recipe['version']
        if 'properties' in json_dict:
            properties = json_dict['properties']
            if 'failureReason' in properties:
                self.failure_reason = properties['failureReason']


    # get all models in project
    # when project_id is None, get all models in current speech account
    @staticmethod
    def list(config: Config, project_id: str = None):
        config.logger.debug('Model.list')
        models = []
        api_url = config.url_prefix + 'models' + '?' + config.api_version
        if project_id is not None and len(project_id) > 0:
            api_url += "&filter=projectId eq '%s'" % project_id
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        while api_url is not None and len(api_url) > 0:
            response = requests.get(api_url, headers=headers)
            raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
            response_dict= response.json()
            for json_dict in response_dict['value']:
                model = Model(json_dict)
                models.append(model)
            if 'nextLink' in response_dict:
                api_url = response_dict['nextLink']
            else:
                api_url = None
        return models


    @staticmethod
    def get(config: Config, model_id: str):
        config.logger.debug('Model.get model_id = %s' % model_id)
        if model_id is None or len(model_id) == 0:
            raise ValueError("'model_id' is None or empty")
        api_url = config.url_prefix + 'models/' + model_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.get(api_url, headers=headers)
        raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
        model = Model(response.json())
        return model

    # Use locale parameter to specific voice model target locale for CrossLingual voice.
    # Use properties parameter to specific styles for MultiStyle voice.
    @staticmethod
    def create(config: Config, project_id: str, model_id: str, voice_name:str, recipe_kind: str, consent_id: str, training_set_id: str,
               description: str = None, locale: str = None, properties: dict = None):
        config.logger.debug('Model.create model_id = %s' % model_id)
        if project_id is None or len(project_id) == 0:
            raise ValueError("'project_id' is None or empty")
        if model_id is None or len(model_id) == 0:
            raise ValueError("'model_id' is None or empty")
        if voice_name is None or len(voice_name) == 0:
            raise ValueError("'voice_name' is None or empty")
        if not voice_name.endswith('Neural'):
            raise ValueError("'voice_name' should endwith 'Neural'")
        if recipe_kind is None or len(recipe_kind) == 0:
            raise ValueError("'recipe_kind' is None or empty")
        if consent_id is None or len(consent_id) == 0:
            raise ValueError("'consent_id' is None or empty")
        if training_set_id is None or len(training_set_id) == 0:
            raise ValueError("'training_set_id' is None or empty")
        if recipe_kind == RecipeKind.Default.name and locale is not None and len(locale) > 0:
            raise ValueError("Needn't provide 'locale' for Default recipe. The locale of voice model will be the same as training set.")
        if recipe_kind == RecipeKind.CrossLingual.name and (locale is None or len(locale) == 0):
            raise ValueError("Need 'locale' parameter to specify the locale of voice model for CrossLingual recipe.")
        if recipe_kind == RecipeKind.MultiStyle.name and properties is None:
            raise ValueError("Need 'properties' parameter to specify style for MultiStyle recipe.")

        api_url = config.url_prefix + 'models/' + model_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        request_dict = {
            'voiceName': voice_name,
            'description': description,
            'recipe': {'kind': recipe_kind},
            'projectId': project_id,
            'consentId': consent_id,
            'trainingSetId': training_set_id
            }
        if locale is not None and len(locale) > 0:
            request_dict['locale'] = locale
        if properties is not None:
            request_dict['properties'] = properties
        response = requests.put(api_url, json=request_dict, headers=headers)
        raise_exception_when_reqeust_failed('PUT', api_url, response, config.logger)
        model = Model(response.json())
        return model


    @staticmethod
    def delete(config: Config, model_id: str):
        config.logger.debug('Model.delete model_id = %s' % model_id)
        if model_id is None or len(model_id) == 0:
            raise ValueError("'model_id' is None or empty")
        api_url = config.url_prefix + 'models/' + model_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.delete(api_url, headers=headers)
        raise_exception_when_reqeust_failed('DELETE', api_url, response, config.logger)

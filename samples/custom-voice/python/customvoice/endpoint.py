#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import json
import requests
import uuid
import logging

from .helper import *
from .config import Config
from .customvoice_object import CustomVoiceObject
from .status_object import StatusObject


class Endpoint(StatusObject):
    def __init__(self, json_dict: dict):
        super().__init__(json_dict)
        if 'projectId' not in json_dict:
            raise ValueError("could not find 'projectId' in json_dict")
        self.project_id = json_dict['projectId']
        if 'modelId' not in json_dict:
            raise ValueError("could not find 'modelId' in json_dict")
        self.model_id = json_dict['modelId']


    # get all endpoints in project
    # when project_id is None, get all endpoints in current speech account
    @staticmethod
    def list(config: Config, project_id: str = None):
        config.logger.debug('Endpoint.list')
        endpoints = []
        api_url = config.url_prefix + 'endpoints' + '?' + config.api_version
        if project_id is not None and len(project_id) > 0:
            api_url += "&filter=projectId eq '%s'" % project_id
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        while api_url is not None and len(api_url) > 0:
            response = requests.get(api_url, headers=headers)
            raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
            response_dict= response.json()
            for json_dict in response_dict['value']:
                endpoint = Endpoint(json_dict)
                endpoints.append(endpoint)
            if 'nextLink' in response_dict:
                api_url = response_dict['nextLink']
            else:
                api_url = None
        return endpoints


    @staticmethod
    def get(config: Config, endpoint_id: str):
        config.logger.debug('Endpoint.get endpoint_id = %s' % endpoint_id)
        if endpoint_id is None or len(endpoint_id) == 0:
            raise ValueError("'endpoint_id' is None or empty")
        try:
            uuid_obj = uuid.UUID(endpoint_id)
        except ValueError:
            raise ValueError("'endpoint_id' should be UUID")
        api_url = config.url_prefix + 'endpoints/' + endpoint_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.get(api_url, headers=headers)
        raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
        endpoint = Endpoint(response.json())
        return endpoint


    @staticmethod
    def create(config: Config, project_id: str, endpoint_id: str, model_id: str, description: str = None):
        config.logger.debug('Endpoint.create endpoint_id = %s' % endpoint_id)
        if project_id is None or len(project_id) == 0:
            raise ValueError("'project_id' is None or empty")
        if endpoint_id is None or len(endpoint_id) == 0:
            raise ValueError("'endpoint_id' is None or empty")
        try:
            uuid_obj = uuid.UUID(endpoint_id)
        except ValueError:
            raise ValueError("'endpoint_id' should be UUID")
        if model_id is None or len(model_id) == 0:
            raise ValueError("'consent_id' is None or empty")

        api_url = config.url_prefix + 'endpoints/' + endpoint_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        request_dict = {
            'description': description,
            'projectId': project_id,
            'modelId': model_id
            }
        response = requests.put(api_url, json=request_dict, headers=headers)
        raise_exception_when_reqeust_failed('PUT', api_url, response, config.logger)
        endpoint = Endpoint(response.json())
        return endpoint


    @staticmethod
    def delete(config: Config, endpoint_id: str):
        config.logger.debug('Endpoint.delete endpoint_id = %s' % endpoint_id)
        if endpoint_id is None or len(endpoint_id) == 0:
            raise ValueError("'endpoint_id' is None or empty")
        try:
            uuid_obj = uuid.UUID(endpoint_id)
        except ValueError:
            raise ValueError("'endpoint_id' should be UUID")
        api_url = config.url_prefix + 'endpoints/' + endpoint_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.delete(api_url, headers=headers)
        raise_exception_when_reqeust_failed('DELETE', api_url, response, config.logger)

#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

from enum import Enum
import json
import requests
import logging

from .helper import *
from .config import Config
from .customvoice_object import CustomVoiceObject

class ProjectKind(Enum):
    ProfessionalVoice = 1
    PersonalVoice = 2


class Project(CustomVoiceObject):

    def __init__(self, json_dict: dict):
        super().__init__(json_dict)
        if 'kind' not in json_dict:
            raise ValueError("could not find 'kind' in json_dict")
        self.kind = ProjectKind[json_dict['kind']]

    # get all projects in current speech account
    @staticmethod
    def list(config: Config):
        config.logger.debug('Project.list')
        projects = []
        api_url = config.url_prefix + 'projects' + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        while api_url is not None and len(api_url) > 0:
            response = requests.get(api_url, headers=headers)
            raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
            response_dict= response.json()
            for json_dict in response_dict['value']:
                project = Project(json_dict)
                projects.append(project)
            if 'nextLink' in response_dict:
                api_url = response_dict['nextLink']
            else:
                api_url = None
        return projects

    @staticmethod
    def get(config: Config, project_id: str):
        config.logger.debug('Project.get project_id = %s' % project_id)
        if project_id is None or len(project_id) == 0:
            raise ValueError("'project_id' is None or empty")
        api_url = config.url_prefix + 'projects/' + project_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.get(api_url, headers=headers)
        raise_exception_when_reqeust_failed('GET', api_url, response, config.logger)
        project = Project(response.json())
        return project

    @staticmethod
    def create(config: Config, project_id: str, kind: ProjectKind, description = None):
        config.logger.debug('Project.create project_id = %s' % project_id)
        if project_id is None or len(project_id) == 0:
            raise ValueError("'project_id' is None or empty")
        api_url = config.url_prefix + 'projects/' + project_id + '?' + config.api_version
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        request_dict = {'description': description, 'kind': kind.name}
        response = requests.put(api_url, json=request_dict, headers=headers)
        raise_exception_when_reqeust_failed('PUT', api_url, response, config.logger)
        project = Project(response.json())
        return project

    @staticmethod
    def delete(config: Config, project_id: str, forceDelete = False):
        config.logger.debug('Project.delete project_id = %s' % project_id)
        if project_id is None or len(project_id) == 0:
            raise ValueError("'project_id' is None or empty")
        api_url = config.url_prefix + 'projects/' + project_id + '?' + config.api_version
        if forceDelete:
            api_url += '&forceDelete=true'
        headers =  {'Ocp-Apim-Subscription-Key':config.key}
        response = requests.delete(api_url, headers=headers)
        raise_exception_when_reqeust_failed('DELETE', api_url, response, config.logger)

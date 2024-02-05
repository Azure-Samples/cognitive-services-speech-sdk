#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import json

class CustomVoiceObject(object):
    def __init__(self, json_dict: dict):
        if json_dict is None:
            raise TypeError
        if 'id' not in json_dict:
            raise ValueError("could not find 'id' in json_dict")
        self.id = json_dict['id']
        if 'displayName' in json_dict:
            self.display_name = json_dict['displayName']
        else:
            self.display_name = ''
        if 'description' in json_dict:
            self.description = json_dict['description']
        else:
            self.description = ''
        if 'createdDateTime' in json_dict:
            self.created_date_time = json_dict['createdDateTime']
        else:
            self.created_date_time = ''

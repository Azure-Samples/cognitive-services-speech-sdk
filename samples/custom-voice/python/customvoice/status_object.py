#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

from enum import Enum
from .customvoice_object import CustomVoiceObject

class Status(Enum):
    NotStarted = 1
    Running = 2
    Succeeded = 3
    Failed = 4
    Disabling = 5
    Disabled = 6

class StatusObject(CustomVoiceObject):
    def __init__(self, json_dict: dict):
        super().__init__(json_dict)
        if 'status' not in json_dict:
            raise ValueError("could not find 'status' in json_dict")
        self.status = Status[json_dict['status']]
        if 'lastActionDateTime' in json_dict:
            self.last_action_date_time = json_dict['lastActionDateTime']
        else:
            self.last_action_date_time = ''

#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import logging

class Config:
    api_version = 'api-version=2024-02-01-preview'
    
    def __init__(self, key: str, region: str, logger: logging.Logger = None):
        self.key = key
        self.region = region
        self.url_prefix = 'https://' + region + '.api.cognitive.microsoft.com/customvoice/'
        if logger is None:
            self.logger = logging.getLogger()
        else:
            self.logger = logger

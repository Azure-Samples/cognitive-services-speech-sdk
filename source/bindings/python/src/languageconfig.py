# Copyright (c) Microsoft. All rights reserved.
# See https://aka.ms/csspeech/license201809 for the full license information.
"""
Classes that are concerned with the handling of language configurations
"""

from . import speech_py_impl as impl
from typing import Optional, List
OptionalStr = Optional[str]

class SourceLanguageConfig():
    """
    Represents source language configuration, allowing specifying the source language and customized endpoint
    
    The configuration can be initialized in different ways:

    - from language: pass a language.
    - from language and custom endpoint: pass a language and an endpoint.
    
    :param language: The source language. The language is specified in BCP-47 format
    :param endpointId: The custom endpoint id    
    """
    def __init__(self, language: str, endpointId: OptionalStr = None):
        if language == "":
            raise ValueError("language cannot be an empty string")        
        if endpointId == "":
            raise ValueError("endpointId cannot be an empty string")
        self._impl = self._get_impl(impl.SourceLanguageConfig, language, endpointId)

    @staticmethod
    def _get_impl(config_type, language, endpointId):        
        _impl = None
        if endpointId is None:
            _impl = config_type._from_language(language)
        else:
            _impl = config_type._from_language(language, endpointId)
        return _impl

class AutoDetectSourceLanguageConfig():
    """
    Represents auto detection source language configuration, allowing open range, specifying the potential source languages and corresponding customized endpoint
    
    The configuration can be initialized in different ways:

    - from open range: pass nothing, for source language auto detection in synthesis.
    - from languages: pass a list of potential source languages, for source language auto detection in recognition.
    - from sourceLanguageConfigs: pass a list of source language configurations, for source language auto detection in recognition.
    
    :param languages: The list of potential source languages. The language is specified in BCP-47 format
    :param sourceLanguageConfigs: The list of source language configurations  
    """
    def __init__(self, languages: List[str] = None, sourceLanguageConfigs: List[SourceLanguageConfig] = None):
        if languages is not None and sourceLanguageConfigs is not None:
            raise ValueError("languages and sourceLanguageConfigs cannot be both specified to create AutoDetectSourceLanguageConfig")
        self._impl = self._get_impl(impl.AutoDetectSourceLanguageConfig, languages, sourceLanguageConfigs)

    @staticmethod
    def _get_impl(config_type, languages, sourceLanguageConfigs):        
        if languages is not None:
            if len(languages) == 0:
                 raise ValueError("languages list cannot be empty")            
            return config_type._from_languages(languages)
        
        if sourceLanguageConfigs is not None:
            if len(sourceLanguageConfigs) == 0:
                raise ValueError("source language config list cannot be empty")
            configs = []
            for config in sourceLanguageConfigs:
                configs.append(config._impl)
            return config_type._from_source_language_configs(configs)

        return config_type._from_open_range()

#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#

# Note: abc = abstract base classes
from collections.abc import Mapping
from datetime import time
from os import linesep
# To install, run:
# python -m pip install requests
import requests
from sys import argv
from typing import Any, Dict, List, Optional, Tuple

class Read_Only_Dict(Mapping):
    def __init__(self, data):
        self._data = data
    def __getitem__(self, key): 
        return self._data[key]
    def __len__(self):
        return len(self._data)
    def __iter__(self):
        return iter(self._data)

def get_cmd_option(option : str) -> Optional[str] :
    argc = len(argv)
    if option in argv :
        index = argv.index(option)
        if index < argc - 1 :
            # We found the option (for example, "--output"), so advance from that to the value (for example, "filename").
            return argv[index + 1]
        else :
            return None
    else :
        return None

def cmd_option_exists(option : str) -> bool :
    return option in argv

def user_config_from_args(usage : str) -> Read_Only_Dict :
    # This should not change unless the Speech REST API changes.
    partial_speech_endpoint = ".api.cognitive.microsoft.com";

    speech_subscription_key = get_cmd_option("--speechKey")
    if speech_subscription_key is None:
        raise RuntimeError(f"Missing Speech subscription key.{linesep}{usage}")
    speech_region = get_cmd_option("--speechRegion")
    if speech_region is None:
        raise RuntimeError(f"Missing Speech region.{linesep}{usage}")

    language_subscription_key = get_cmd_option("--languageKey")
    if language_subscription_key is None:
        raise RuntimeError(f"Missing Language subscription key.{linesep}{usage}")
    language_endpoint = get_cmd_option("--languageEndpoint")
    if language_endpoint is None:
        raise RuntimeError(f"Missing Language endpoint.{linesep}{usage}")

    input_audio_url = get_cmd_option("--input")
    if input_audio_url is None:
        raise RuntimeError(f"Missing input audio URL.{linesep}{usage}")

    language = get_cmd_option("--language")
    if language is None:
        language = "en"
    locale = get_cmd_option("--locale")
    if locale is None:
        locale = "en-US"

    return Read_Only_Dict({
        "use_stereo_audio" : cmd_option_exists("--stereo"),
        "language" : language,
        "locale" : locale,
        "input_audio_url" : input_audio_url,
        "output_file_path" : get_cmd_option("--output"),
        "speech_subscription_key" : speech_subscription_key,
        "speech_endpoint" : f"{speech_region}{partial_speech_endpoint}",
        "language_subscription_key" : language_subscription_key,
        "language_endpoint" : language_endpoint,
    })

def chunk(xs : List[Any], size : int) -> List[List[Any]] :
    return [xs[i : i + size] for i in range(0, len(xs), size)]

def send_get(uri : str, key : str, expected_status_codes : List[int]) -> Dict :
    headers = {"Ocp-Apim-Subscription-Key": key}
    response = requests.get(uri, headers=headers)
    if response.status_code not in expected_status_codes :
        raise Exception(f"The GET request to {uri} returned a status code {response.status_code} that was not in the expected status codes: {expected_status_codes}")
    else :
        try :
            # response.json() throws if the response is empty.
            response_json = response.json()
            return { "headers" : response.headers, "text" : response.text, "json" : response_json }
        except Exception :
            return { "headers" : response.headers, "text" : response.text, "json" : None }

def send_post(uri : str, content : Dict, key : str, expected_status_codes : List[int]) -> Dict :
    headers = {"Ocp-Apim-Subscription-Key": key}
    response = requests.post(uri, headers=headers, json=content)
    if response.status_code not in expected_status_codes :
        raise Exception(f"The POST request to {uri} returned a status code {response.status_code} that was not in the expected status codes: {expected_status_codes}")
    else :
        try :
            response_json = response.json()
            return { "headers" : response.headers, "text" : response.text, "json" : response_json }
        except Exception :
            return { "headers" : response.headers, "text" : response.text, "json" : None }

def send_delete(uri : str, key : str, expected_status_codes : List[int]) -> None :
    headers = {"Ocp-Apim-Subscription-Key": key}
    response = requests.delete(uri, headers=headers)
    if response.status_code not in expected_status_codes :
        raise Exception(f"The DELETE request to {uri} returned a status code {response.status_code} that was not in the expected status codes: {expected_status_codes}")

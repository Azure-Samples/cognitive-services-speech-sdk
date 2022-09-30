#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#

# To install, run:
# python -m pip install requests
import requests
from typing import Dict, List

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

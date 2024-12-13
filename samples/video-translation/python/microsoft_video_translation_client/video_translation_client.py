# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import urllib3
import orjson
import uuid
import requests
import datetime
import locale
import json
import dataclasses
from termcolor import colored
from enum import Enum
from datetime import datetime
from typing import List
from urllib3.util import Url
from microsoft_video_translation_client.video_translation_const import *
from microsoft_video_translation_client.video_translation_enum import *
from microsoft_video_translation_client.video_translation_dataclass import *
from microsoft_video_translation_client.video_translation_util import *
from urllib.parse import urlencode
from pydantic import BaseModel
import time

class VideoTranslationClient:
    URL_SEGMENT_NAME_TRANSLATIONS = "translations"
    URL_SEGMENT_NAME_ITERATIONS = "iterations"
    URL_PATH_ROOT = "videotranslation"
    
    region = ""
    sub_key = ""
    api_version = ""

    def __init__(self, region, sub_key, api_version):
        if region is None or sub_key is None:
            raise ValueError
        self.region = region
        self.sub_key = sub_key
        self.api_version = api_version
        
        # not retry for below response code:
        #   OK = 200,
        #   Created = 201,
        #   NoContent = 204,
        #   BadRequest = 400
        #   Unauthorized = 401
        #   Forbidden = 403
        #   NotFound = 404
        #   Conflict = 409
        status_forcelist = tuple(set(x for x in requests.status_codes._codes) - set(x for x in [200, 201, 204, 400, 401, 403, 404, 409]))
        retries = urllib3.Retry(total=5, status_forcelist=status_forcelist)
        timeout = urllib3.util.Timeout(10)
        self.http = urllib3.PoolManager(timeout=timeout, retries=retries)

    # For most common scenario, customer not need provide webvtt first iteration.
    # Even, it is supported to provide webvtt for the first iteration, customer can customize the client code if they want to run first iteration with webvtt.
    def create_translate_and_run_first_iteration_until_terminated(
        self,
        video_file_url: Url,
        source_locale: locale,
        target_locale: locale,
        voice_kind: VoiceKind,
        speaker_count: int = None,
        subtitle_max_char_count_per_segment: int = None,
        export_subtitle_in_video: bool = None
    ) -> tuple[bool, str, TranslationDefinition, IterationDefinition]:
        if video_file_url is None or source_locale is None or target_locale is None or voice_kind is None or voice_kind is None:
            raise ValueError
        
        now = datetime.now()
        nowString = now.strftime("%m%d%Y%H%M%S")
        translation_id = f"{nowString}_{source_locale}_{target_locale}_{voice_kind}"
        success, error, translation = self.create_translation_until_terminated(
            translation_id = translation_id,
            video_file_url = video_file_url,
            source_locale = source_locale,
            target_locale = target_locale,
            voice_kind = voice_kind,
        )
        if not success:
            return False, error, None, None
        
        print(colored("succesfully created translation:", 'green'))
        json_formatted_str = json.dumps(dataclasses.asdict(translation), indent = 2)
        print(json_formatted_str)

        iteration_id = f"{nowString}_default"
        success, error, iteration = self.create_iteration_until_terminated(
            translation_id = translation_id,
            iteration_id = iteration_id,
            speaker_count = speaker_count,
            subtitle_max_char_count_per_segment = subtitle_max_char_count_per_segment,
            export_subtitle_in_video = export_subtitle_in_video,
        )
        if not success:
            return False, error, None, None
        
        print(colored("succesfully created iteration:", 'green'))
        json_formatted_str = json.dumps(dataclasses.asdict(iteration), indent = 2)
        print(json_formatted_str)

        return True, None, translation, iteration
    
    # For iteration from secondary, webvtt file is required.
    def run_iteration_with_webvtt_until_terminated(
        self,
        translation_id: str,
        webvtt_file_kind: WebvttFileKind,
        webvtt_file_url: Url,
        speaker_count: int = None,
        subtitle_max_char_count_per_segment: int = None,
        export_subtitle_in_video: bool = None
    ) -> tuple[bool, str, TranslationDefinition, IterationDefinition]:
        if webvtt_file_kind is None or webvtt_file_url is None:
            raise ValueError
        
        success, error, translation = self.request_get_translation(
            translation_id = translation_id,
        )
        if not success:
            return False, error, None
        elif translation is None:
            return False, f"Not found translation ID: {translation_id}", None
        
        now = datetime.now()
        iteration_id = now.strftime("%m%d%Y%H%M%S")

        success, error, iteration = self.create_iteration_until_terminated(
            translation_id = translation_id,
            iteration_id = iteration_id,
            webvtt_file_kind = webvtt_file_kind,
            webvtt_file_url = webvtt_file_url,
            speaker_count = speaker_count,
            subtitle_max_char_count_per_segment = subtitle_max_char_count_per_segment,
            export_subtitle_in_video = export_subtitle_in_video,
        )
        if not success:
            return False, error, None
        
        return True, None, translation, iteration
    
    def create_translation_until_terminated(
        self,
        translation_id: str,
        video_file_url: Url,
        source_locale: locale,
        target_locale: locale,
        voice_kind: VoiceKind,
    ) -> tuple[bool, str, TranslationDefinition]:
        operation_id = str(uuid.uuid4())
        success, error, response_translation, operation_location = self.request_create_translation(
            translation_id = translation_id,
            video_file_url = video_file_url,
            source_locale = source_locale,
            target_locale = target_locale,
            voice_kind = voice_kind,
            speaker_count = None,
            subtitle_max_char_count_per_segment = None,
            export_subtitle_in_video = None,
            translation_display_name = None,
            translation_description = None,
            operation_id = operation_id)
        if not success or operation_location is None:
            print(colored(f"Failed to create translation with ID {translation_id} with error: {error}", 'red'))
            return False, error, None

        self.request_operation_until_terminated(operation_location)
        
        success, error, response_translation = self.request_get_translation(translation_id)
        if not success:
            print(colored(f"Failed to query translation {translation_id} with error: {error}", 'red'))
            return False, error, None
        if response_translation.status != OperationStatus.Succeeded:
            print(colored(f"Translation creation failed with error: {error}", 'red'))
            print(json.dumps(dataclasses.asdict(response_translation), indent = 2))
            return False, response_translation.translationFailureReason, None
        
        return True, None, response_translation

    def request_operation_until_terminated(
        self,
        operation_location: Url):
        success, error, response_operation = self.request_get_operation(operation_location = operation_location, printUrl = True)
        if not success or response_operation is None:
            print(colored(f"Failed to query operation for translation creation operation from location {operation_location} with error: {error}", 'red'))
            return
        
        lastStatus = None
        while response_operation.status in [OperationStatus.Running, OperationStatus.NotStarted]:
            success, error, response_operation = self.request_get_operation(operation_location = operation_location, printUrl = False)
            if not success or response_operation is None:
                print(colored(f"Failed to query operation for translation creation operation from location {operation_location} with error: {error}", 'red'))
                return
            if lastStatus != response_operation.status:
                print(response_operation.status)
                lastStatus = response_operation.status
            print(".", end="")
            # in seconds
            time.sleep(5)

        return response_operation.status
    
    def create_iteration_until_terminated(
        self,
        translation_id: str,
        iteration_id: str,
        webvtt_file_kind: WebvttFileKind = None,
        webvtt_file_url: Url = None,
        speaker_count: int = None,
        subtitle_max_char_count_per_segment: int = None,
        export_subtitle_in_video: bool = None,
    ) -> tuple[bool, str, IterationDefinition]:
        if translation_id is None or iteration_id is None:
            raise ValueError
        success, error, response_iteration, operation_location = self.request_create_iteration(
            translation_id = translation_id,
            iteration_id = iteration_id,
            webvtt_file_kind = webvtt_file_kind,
            webvtt_file_url = webvtt_file_url,
            speaker_count = speaker_count,
            subtitle_max_char_count_per_segment = subtitle_max_char_count_per_segment,
            export_subtitle_in_video = export_subtitle_in_video,
            iteration_description = None,
            operation_id = None)
        if not success:
            print(colored(f"Failed to create iteration with ID {iteration_id} for translation {translation_id} with error: {error}", 'red'))
            return False, error, None
        
        self.request_operation_until_terminated(operation_location)
        
        success, error, response_iteration = self.request_get_iteration(translation_id, iteration_id)
        if not success:
            print(colored(f"Failed to query iteration {iteration_id} for translation {translation_id} with error: {error}", 'red'))
            return False, error, None
        if response_iteration.status != OperationStatus.Succeeded:
            print(colored(f"Iteration creation failed with error: {error}", 'red'))
            print(json.dumps(dataclasses.asdict(operation_location), indent = 2))
            return False, response_iteration.translationFailureReason, None
        
        return True, None, response_iteration
    
    def build_translations_path(self) -> str:
        return f"{self.URL_PATH_ROOT}/{self.URL_SEGMENT_NAME_TRANSLATIONS}"
    
    def build_translation_path(self,
                               translation_id: str) -> str:
        if translation_id is None:
            raise ValueError
        translations_path = self.build_translations_path()
        return f"{translations_path}/{translation_id}"

    def build_iterations_path(self,
                              translation_id: str) -> str:
        if translation_id is None:
            raise ValueError
        translation_path = self.build_translation_path(translation_id)
        return f"{translation_path}/{self.URL_SEGMENT_NAME_ITERATIONS}"

    def build_iteration_path(self,
                             translation_id: str,
                             iteration_id: str) -> str:
        if translation_id is None or iteration_id is None:
            raise ValueError
        iterations_path = self.build_iterations_path(translation_id)
        return f"{iterations_path}/{iteration_id}"
    
    def build_host(self) -> str:
        return f"{self.region}.api.cognitive.microsoft.com"

    def build_url(self,
                  segments: str) -> Url:
        if segments is None:
            raise ValueError
        host = self.build_host()
        return urllib3.util.parse_url(f"https://{host}/{segments}?api-version={self.api_version}")

    def build_translations_url(self) -> Url:
        path = self.build_translations_path()
        return self.build_url(path)
    
    def build_translation_url(self,
                              translation_id: str) -> Url:
        if translation_id is None:
            raise ValueError
        path = self.build_translation_path(translation_id)
        return self.build_url(path)

    def build_iterations_url(self,
                             translation_id: str) -> Url:
        if translation_id is None:
            raise ValueError
        path = self.build_iterations_path()
        return self.build_url(path)
    
    def build_iteration_url(self,
                            translation_id: str,
                            iteration_id: str) -> Url:
        if translation_id is None or iteration_id is None:
            raise ValueError
        path = self.build_iteration_path(translation_id, iteration_id)
        return self.build_url(path)
    
    def build_request_header(self) -> dict:
        return {
            "Ocp-Apim-Subscription-Key": self.sub_key
        }
        
    # https://learn.microsoft.com/en-us/rest/api/aiservices/videotranslation/operation-operations/get-operation?view=rest-aiservices-videotranslation-2024-05-20-preview&tabs=HTTP
    def request_get_operation(self,
                              operation_location: Url,
                              printUrl: bool) -> tuple[bool, str, OperationDefinition]:
        if operation_location is None:
            raise ValueError

        headers = self.build_request_header()
        
        if printUrl:
            print(f"Requesting http GET: {operation_location}")
        response = self.http.request("GET", operation_location.url, headers = headers)
        
        #   OK = 200,
        #   NotFound = 404,
        if response.status == 200:
            response_json = response.json()
            response = dict_to_dataclass(
                data = response_json,
                dataclass_type = OperationDefinition)
            return True, None, response
        elif response.status == 404:
            return True, None, None

        return False, response.reason, None

    # https://learn.microsoft.com/en-us/rest/api/aiservices/videotranslation/translation-operations/get-translation?view=rest-aiservices-videotranslation-2024-05-20-preview&tabs=HTTP
    def request_get_translation(self,
                                translation_id: str) -> tuple[bool, str, TranslationDefinition]:
        if translation_id is None:
            raise ValueError

        url = self.build_translation_url(translation_id)
        headers = self.build_request_header()
        
        print(f"Requesting http GET: {url}")
        response = self.http.request("GET", url.url, headers = headers)
        
        #   OK = 200,
        #   NotFound = 404,
        if response.status == 200:
            response_translation_json = response.json()
            response_translation = dict_to_dataclass(
                data = response_translation_json,
                dataclass_type = TranslationDefinition)
            return True, None, response_translation
        elif response.status == 404:
            return True, None, None

        return False, response.reason, None
    
    # https://learn.microsoft.com/en-us/rest/api/aiservices/videotranslation/iteration-operations/get-iteration?view=rest-aiservices-videotranslation-2024-05-20-preview&tabs=HTTP
    def request_get_iteration(self,
                              translation_id: str,
                              iteration_id: str) -> tuple[bool, str, IterationDefinition]:
        if translation_id is None or iteration_id is None:
            raise ValueError

        url = self.build_iteration_url(translation_id, iteration_id)
        headers = self.build_request_header()
        
        print(f"Requesting http GET: {url}")
        response = self.http.request("GET", url.url, headers = headers)
        
        #   OK = 200,
        #   NotFound = 404,
        if response.status == 200:
            response_iteration_json = response.json()
            response_iteration = dict_to_dataclass(
                data = response_iteration_json,
                dataclass_type = IterationDefinition)
            return True, None, response_iteration
        elif response.status == 404:
            return True, None, None

        return False, response.reason, None

    # https://learn.microsoft.com/en-us/rest/api/aiservices/videotranslation/translation-operations/list-translation?view=rest-aiservices-videotranslation-2024-05-20-preview&tabs=HTTP
    def request_list_translations(self,
                                  top: int = None,
                                  skip: int = None,
                                  maxPageSize: int = None) -> tuple[bool, str, PagedTranslationDefinition]:
        
        url = self.build_translations_url()
        args = {}
        if top is not None:
            args["top"] = top
        if skip is not None:
            args["skip"] = skip
        if maxPageSize is not None:
            args["maxPageSize"] = maxPageSize
        
        url = append_url_args(url, args)
        
        headers = self.build_request_header()
        
        print(f"Requesting http GET: {url}")
        response = self.http.request("GET", url.url, headers = headers)
        
        #   OK = 200,
        if not response.status in [200]:
            error = response.data.decode('utf-8')
            return False, error, None
        response_translations_json = response.json()
        response_translations = dict_to_dataclass(
            data = response_translations_json,
            dataclass_type = PagedTranslationDefinition)
        return True, None, response_translations

    # https://learn.microsoft.com/en-us/rest/api/aiservices/videotranslation/iteration-operations/list-iteration?view=rest-aiservices-videotranslation-2024-05-20-preview&tabs=HTTP
    def request_list_iterations(self) -> tuple[bool, str, PagedIterationDefinition]:
        url = self.build_iterations_url()
        headers = self.build_request_header()
        
        print(f"Requesting http GET: {url}")
        response = self.http.request("GET", url.url, headers = headers)
        
        #   OK = 200,
        if not response.status in [200]:
            error = response.data.decode('utf-8')
            return False, error, None
        response_iterations_json = response.json()
        response_iterations = dict_to_dataclass(
            data = response_iterations_json,
            dataclass_type = PagedIterationDefinition)
        return True, None, response_iterations
    
    # https://learn.microsoft.com/en-us/rest/api/aiservices/videotranslation/translation-operations/delete-translation?view=rest-aiservices-videotranslation-2024-05-20-preview&tabs=HTTP
    def request_delete_translation(self,
                                   translation_id: str) -> tuple[bool, str]:
        url = self.build_translation_url(translation_id)
        headers = self.build_request_header()
        
        print(f"Requesting http DELETE: {url}")
        response = self.http.request("DELETE", url.url, headers = headers)
        
        #   NoContent = 204,
        if not response.status in [204]:
            error = response.data.decode('utf-8')
            return False, error
        return True, None
        
    # https://learn.microsoft.com/en-us/rest/api/aiservices/videotranslation/translation-operations/create-translation?view=rest-aiservices-videotranslation-2024-05-20-preview&tabs=HTTP
    def request_create_translation(
            self,
            translation_id: str,
            video_file_url: str,
            source_locale: locale,
            target_locale: locale,
            voice_kind: VoiceKind,
            speaker_count: int = None,
            subtitle_max_char_count_per_segment: int = None,
            export_subtitle_in_video: bool = None,
            translation_display_name: str = None,
            translation_description: str = None,
            operation_id: str = None,
        ) -> tuple[bool, str, TranslationDefinition, Url]:
        if translation_id is None or video_file_url is None or source_locale is None or target_locale is None or voice_kind is None:
            raise ValueError

        translation_create_input_body = TranslationInputDefinition(
            sourceLocale = source_locale,
            targetLocale = target_locale,
            voiceKind = voice_kind,
            videoFileUrl=video_file_url,
            speakerCount =speaker_count,
            subtitleMaxCharCountPerSegment = subtitle_max_char_count_per_segment,
            exportSubtitleInVideo = export_subtitle_in_video,
        )
        
        translation_create_body = TranslationDefinition(
            input = translation_create_input_body,
            displayName = translation_display_name,
            description = translation_description,
        )
        
        encoded_translation_create_body = orjson.dumps(dataclasses.asdict(translation_create_body))
        
        url = self.build_translation_url(translation_id)
        headers = self.build_request_header()
        headers["Operation-Id"] = operation_id
        
        print(f"Requesting http PUT: {url}")
        response = self.http.request("PUT", url.url, headers = headers, body=encoded_translation_create_body)
        
        #   OK = 200,
        #   Created = 201,
        if not response.status in [200, 201]:
            error = response.data.decode('utf-8')
            return False, error, None, None
        response_translation_json = response.json()
        response_translation = dict_to_dataclass(
            data = response_translation_json,
            dataclass_type = TranslationDefinition)
        operation_location = response.headers[HTTP_HEADERS_OPERATION_LOCATION]
        operation_location_url = urllib3.util.parse_url(operation_location)
        return True, None, response_translation, operation_location_url
    
    # https://learn.microsoft.com/en-us/rest/api/aiservices/videotranslation/iteration-operations/create-iteration?view=rest-aiservices-videotranslation-2024-05-20-preview&tabs=HTTP
    def request_create_iteration(
            self,
            translation_id: str,
            iteration_id: str,
            webvtt_file_kind: WebvttFileKind = None,
            webvtt_file_url: Url = None,
            speaker_count: int = None,
            subtitle_max_char_count_per_segment: int = None,
            export_subtitle_in_video: bool = None,
            iteration_description: str = None,
            operation_id: str = None,
        ) -> tuple[bool, str, IterationDefinition, Url]:
        if translation_id is None or iteration_id is None:
            raise ValueError

        translation_create_input_body = IterationInputDefinition(
            speakerCount = speaker_count,
            exportSubtitleInVideo = export_subtitle_in_video,
            subtitleMaxCharCountPerSegment = subtitle_max_char_count_per_segment,
        )
        
        if webvtt_file_kind is not None and webvtt_file_kind is not None:
            translation_create_input_body.webvttFile = WebvttFileDefinition(
                kind = webvtt_file_kind,
                url = webvtt_file_url,
            )
        
        iteration_create_body = IterationDefinition(
            input = translation_create_input_body,
            description = iteration_description,
        )
        
        encoded_iteration_create_body = orjson.dumps(dataclasses.asdict(iteration_create_body))
        url = self.build_iteration_url(translation_id, iteration_id)
        if operation_id is None:
            operation_id = str(uuid.uuid4())
        headers = self.build_request_header()
        headers["Operation-Id"] = operation_id
        
        print(f"Requesting http PUT: {url}")
        response = self.http.request("PUT", url.url, headers = headers, body=encoded_iteration_create_body)
        
        #   OK = 200,
        #   Created = 201,
        if not response.status in [200, 201]:
            error = response.data.decode('utf-8')
            return False, error, None, None
        response_iteration_json = response.json()
        response_iteration = dict_to_dataclass(
            data = response_iteration_json,
            dataclass_type = IterationDefinition)
        operation_location = response.headers[HTTP_HEADERS_OPERATION_LOCATION]
        operation_location_url = urllib3.util.parse_url(operation_location)
        return True, None, response_iteration, operation_location_url
    
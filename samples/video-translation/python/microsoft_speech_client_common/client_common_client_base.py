# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import dataclasses
import json
import orjson
import urllib3
import uuid
import time
from termcolor import colored
from urllib3.util import Url
from urllib3 import HTTPResponse
from microsoft_speech_client_common.client_common_const import (
    HTTP_HEADERS_OPERATION_LOCATION
)
from microsoft_speech_client_common.client_common_enum import (
    OperationStatus
)
from microsoft_speech_client_common.client_common_dataclass import (
    OperationDefinition
)
from microsoft_speech_client_common.client_common_util import (
    dict_to_dataclass,
    append_url_args
)


class SpeechLongRunningTaskClientBase:
    """Base class for Speech service clients that handle long-running task operations."""

    region = ""
    sub_key = ""
    api_version = ""
    service_url_segment_name = ""
    long_running_tasks_url_segment_name = ""
    http = None

    def __init__(self,
                 region: str,
                 sub_key: str,
                 api_version: str,
                 service_url_segment_name: str,
                 long_running_tasks_url_segment_name: str):
        """
        Initialize the base client with common configuration.

        Args:
            region: Azure region for the service
            sub_key: Subscription key for authentication
            api_version: API version to use
        """
        if region is None or sub_key is None:
            raise ValueError("Region and subscription key are required")

        if service_url_segment_name is None or long_running_tasks_url_segment_name is None:
            raise ValueError("Service URL segment and long-running task URL segment are required")

        self.region = region
        self.sub_key = sub_key
        self.api_version = api_version
        self.service_url_segment_name = service_url_segment_name
        self.long_running_tasks_url_segment_name = long_running_tasks_url_segment_name

        # Configure retry logic for transient failures
        # Not retrying for: 200, 201, 204, 400, 401, 403, 404, 409
        # not retry for below response code:
        #   OK = 200,
        #   Created = 201,
        #   NoContent = 204,
        #   BadRequest = 400,
        #   Unauthorized = 401,
        #   Forbidden = 403,
        #   NotFound = 404,
        #   Conflict = 409,
        status_forcelist = tuple(
            set(x for x in range(100, 600))
            - set([200, 201, 204, 400, 401, 403, 404, 409])
        )
        retries = urllib3.Retry(total=5, status_forcelist=status_forcelist)
        retries.RETRY_AFTER_STATUS_CODES = frozenset(status_forcelist)

        # Use a custom Retry subclass that logs on each retry
        class LoggingRetry(urllib3.Retry):
            def increment(self, method=None, url=None, response=None, error=None, _pool=None, _stacktrace=None):
                if response:
                    print(colored(
                        f"Retrying request: {method} {url} (status: {response.status}, "
                        f"retries remaining: {self.total})",
                        'yellow'
                    ))
                elif error:
                    print(colored(
                        f"Retrying request: {method} {url} (error: {error}, "
                        f"retries remaining: {self.total})",
                        'yellow'
                    ))
                return super().increment(method=method, url=url, response=response, error=error, _pool=_pool, _stacktrace=_stacktrace)

        retries = LoggingRetry(total=5, status_forcelist=status_forcelist)
        timeout = urllib3.util.Timeout(10)
        self.http = urllib3.PoolManager(timeout=timeout, retries=retries)

    def build_url(self,
                  segments: str) -> Url:
        if segments is None:
            raise ValueError
        rootUrl = self.root_url()
        return urllib3.util.parse_url(f"{str(rootUrl)}/{segments}?api-version={self.api_version}")

    def root_url(self) -> Url:
        """Build the service root URL based on region."""
        url = None
        if self.region.startswith("http"):
            url = self.region
        else:
            host = self.build_host()
            url = f"https://{host}"
        return urllib3.util.parse_url(url)

    def build_host(self) -> str:
        """Build the service host URL based on region."""
        return f"{self.region}.api.cognitive.microsoft.com"

    def build_request_header(self) -> dict:
        """Build common request headers with authentication."""
        return {
            "Ocp-Apim-Subscription-Key": self.sub_key
        }

    def build_long_running_tasks_path(self) -> str:
        return f"{self.service_url_segment_name}/{self.long_running_tasks_url_segment_name}"

    def build_long_running_task_path(
            self,
            id: str) -> str:
        if id is None:
            raise ValueError
        tasks_path = self.build_long_running_tasks_path()
        return f"{tasks_path}/{id}"

    def build_long_running_task_url(
            self,
            id: str) -> Url:
        if id is None:
            raise ValueError
        path = self.build_long_running_task_path(id)
        return self.build_url(path)

    def build_long_running_tasks_url(self) -> Url:
        path = self.build_long_running_tasks_path()
        return self.build_url(path)

    # https://learn.microsoft.com/rest/api/aiservices/videotranslation/translation-operations/create-translation
    def request_create_long_running_task_until_terminated(
            self,
            id: str,
            creation_body: dataclasses.dataclass,
            operation_id: str = None,
            ) -> tuple[bool, str, HTTPResponse, Url]:
        if id is None or creation_body is None:
            raise ValueError

        success, error, response, operation_location_url = (
            self.request_create_long_running_task_with_id(
                id=id,
                creation_body=creation_body,
                operation_id=operation_id))
        if not success:
            return False, error, None, None

        if not success or operation_location_url is None:
            print(colored(
                f"Failed to create task with ID {operation_location_url} "
                f"with error: {error}", 'red'))
            return False, error, None, None

        self.request_operation_until_terminated(operation_location_url)
        success, error, response = self.request_get_long_running_task(id)
        if not success:
            print(colored(f"Failed to query task {id} with error: {error}", 'red'))
            return False, error, None, None
        return True, None, response, operation_location_url

    def request_create_long_running_task_with_id(
            self,
            id: str,
            creation_body: dataclasses.dataclass,
            operation_id: str = None,
            ) -> tuple[bool, str, HTTPResponse, Url]:
        if id is None or creation_body is None:
            raise ValueError
        url = self.build_long_running_task_url(id)
        return self.request_create_long_running_task_with_url(
            url=url,
            creation_body=creation_body,
            operation_id=operation_id
        )

    def request_create_long_running_task_with_url(
            self,
            url: Url,
            creation_body: dataclasses.dataclass,
            operation_id: str = None,
            ) -> tuple[bool, str, HTTPResponse, Url]:
        if url is None or creation_body is None:
            raise ValueError
        encoded_creation_body = orjson.dumps(dataclasses.asdict(creation_body))

        headers = self.build_request_header()
        if operation_id is None:
            operation_id = str(uuid.uuid4())
        headers["Operation-Id"] = operation_id
        headers["Content-Type"] = "application/json"

        print(f"Requesting http PUT: {url}")
        json_formatted_str = json.dumps(dataclasses.asdict(creation_body), indent=2)
        print(json_formatted_str)

        # For task creation may need longer timeout, set to 60 seconds here.
        # For other APIs, can use default timeout (10 seconds as set in __init__)
        response = self.http.request(
            "PUT", url.url, headers=headers, body=encoded_creation_body,
            timeout=60)

        #   OK = 200,
        #   Created = 201,
        if response.status not in [200, 201]:
            error = response.data.decode('utf-8')
            return False, error, None, None
        operation_location = response.headers[HTTP_HEADERS_OPERATION_LOCATION]
        operation_location_url = urllib3.util.parse_url(operation_location)
        return True, None, response, operation_location_url

    def request_list_long_running_tasks(
            self,
            top: int = None,
            skip: int = None,
            maxPageSize: int = None) -> tuple[bool, str, HTTPResponse]:

        url = self.build_long_running_tasks_url()
        args = {}
        if top is not None:
            args["top"] = top
        if skip is not None:
            args["skip"] = skip
        if maxPageSize is not None:
            args["maxPageSize"] = maxPageSize

        url = append_url_args(url, args)
        return self.request_list_with_url(url)

    def request_list_with_url(
            self,
            url: Url) -> tuple[bool, str, HTTPResponse]:
        headers = self.build_request_header()

        print(f"Requesting http GET: {url}")
        response = self.http.request("GET", url.url, headers=headers)

        #   OK = 200,
        if response.status not in [200]:
            error = response.data.decode('utf-8')
            return False, error, None
        return True, None, response

    def request_get_long_running_task(
            self,
            id: str) -> tuple[bool, str, HTTPResponse]:
        if id is None:
            raise ValueError

        url = self.build_long_running_task_url(id)
        return self.request_get_with_url(url)

    def request_get_with_url(
            self,
            url: Url) -> tuple[bool, str, HTTPResponse]:
        if url is None:
            raise ValueError

        headers = self.build_request_header()

        print(f"Requesting http GET: {url}")
        response = self.http.request("GET", url.url, headers=headers)

        #   OK = 200,
        #   NotFound = 404,
        if response.status == 200:
            return True, None, response
        elif response.status == 404:
            return True, None, None

        return False, response.reason, None

    # https://learn.microsoft.com/rest/api/aiservices/videotranslation/operation-operations/get-operation
    def request_get_operation(
        self,
        operation_location: Url,
        print_url: bool = False
    ) -> tuple[bool, str, OperationDefinition]:
        """
        Query the status of a long-running operation.

        Args:
            operation_location: URL of the operation to query
            print_url: Whether to print the URL being requested

        Returns:
            Tuple of (success, error_message, operation_definition)
        """
        if operation_location is None:
            raise ValueError("Operation location is required")

        headers = self.build_request_header()

        if print_url:
            print(f"Requesting http GET: {operation_location}")

        response = self.http.request("GET", operation_location.url, headers=headers)

        #   OK = 200
        #   NotFound = 404
        if response.status == 200:
            response_json = response.json()
            operation = dict_to_dataclass(
                data=response_json,
                dataclass_type=OperationDefinition
            )
            return True, None, operation
        elif response.status == 404:
            return True, None, None

        return False, response.reason, None

    def request_delete_long_running_task(self,
                                         id: str) -> tuple[bool, str]:
        url = self.build_long_running_task_url(id)
        headers = self.build_request_header()

        print(f"Requesting http DELETE: {url}")
        response = self.http.request("DELETE", url.url, headers=headers)

        #   NoContent = 204,
        if response.status not in [204]:
            error = response.data.decode('utf-8')
            return False, error
        return True, None

    def request_operation_until_terminated(
        self,
        operation_location: Url,
        poll_interval_seconds: int = 5
    ) -> OperationStatus:
        """
        Poll a long-running operation until it reaches a terminal state.

        Args:
            operation_location: URL of the operation to poll
            poll_interval_seconds: Time to wait between polls (default: 5 seconds)

        Returns:
            Final operation status
        """
        if operation_location is None:
            raise ValueError("Operation location is required")

        success, error, response_operation = self.request_get_operation(
            operation_location=operation_location,
            print_url=True
        )

        if not success or response_operation is None:
            print(colored(
                f"Failed to query operation from location {operation_location} with error: {error}",
                'red'
            ))
            return None

        last_status = None
        while response_operation.status in [OperationStatus.Running, OperationStatus.NotStarted]:
            success, error, response_operation = self.request_get_operation(
                operation_location=operation_location,
                print_url=False
            )

            if not success or response_operation is None:
                print(colored(
                    f"Failed to query operation from location {operation_location} with error: {error}",
                    'red'
                ))
                return None

            if last_status != response_operation.status:
                print(response_operation.status)
                last_status = response_operation.status

            print(".", end="", flush=True)
            time.sleep(poll_interval_seconds)

        print()  # New line after polling dots
        return response_operation.status

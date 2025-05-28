#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#

from os import linesep
from sys import argv
from typing import Optional
import helper

# This should not change unless the Speech REST API changes.
PARTIAL_SPEECH_ENDPOINT = ".api.cognitive.microsoft.com"


def get_cmd_option(option: str) -> Optional[str]:
    argc = len(argv)
    if option.lower() in list(map(lambda arg: arg.lower(), argv)):
        index = argv.index(option)
        if index < argc - 1:
            # We found the option (for example, "--output"), so advance from that to the value (for example, "filename").
            return argv[index + 1]
        else:
            return None
    else:
        return None


def cmd_option_exists(option: str) -> bool:
    return option.lower() in list(map(lambda arg: arg.lower(), argv))


def user_config_from_args(usage: str) -> helper.Read_Only_Dict:
    input_audio_url = get_cmd_option("--input")
    input_file_path = get_cmd_option("--jsonInput")
    if input_audio_url is None and input_file_path is None:
        raise RuntimeError(f"Please specify either --input or --jsonInput.{linesep}{usage}")

    speech_subscription_key = get_cmd_option("--speechKey")
    if speech_subscription_key is None and input_file_path is None:
        err_msg = "Missing Speech subscription key. Speech subscription key is required unless --jsonInput is present."
        raise RuntimeError(f"{err_msg}{linesep}{usage}")
    speech_region = get_cmd_option("--speechRegion")
    if speech_region is None and input_file_path is None:
        raise RuntimeError(f"Missing Speech region. Speech region is required unless --jsonInput is present.{linesep}{usage}")

    language_subscription_key = get_cmd_option("--languageKey")
    if language_subscription_key is None:
        raise RuntimeError(f"Missing Language subscription key.{linesep}{usage}")
    language_endpoint = get_cmd_option("--languageEndpoint")
    if language_endpoint is None:
        raise RuntimeError(f"Missing Language endpoint.{linesep}{usage}")
    language_endpoint = language_endpoint.replace("https://", "")

    language = get_cmd_option("--language")
    if language is None:
        language = "en"
    locale = get_cmd_option("--locale")
    if locale is None:
        locale = "en-US"

    return helper.Read_Only_Dict({
        "use_stereo_audio": cmd_option_exists("--stereo"),
        "language": language,
        "locale": locale,
        "input_audio_url": input_audio_url,
        "input_file_path": input_file_path,
        "output_file_path": get_cmd_option("--output"),
        "speech_subscription_key": speech_subscription_key,
        "speech_endpoint": f"{speech_region}{PARTIAL_SPEECH_ENDPOINT}",
        "language_subscription_key": language_subscription_key,
        "language_endpoint": language_endpoint,
    })

#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#

from datetime import timedelta
from enum import Enum
from os import linesep
from sys import argv
from typing import List, Optional
import azure.cognitiveservices.speech as speechsdk # type: ignore
import helper

class CaptioningMode(Enum):
    OFFLINE = 1
    REALTIME = 2

DEFAULT_MAX_LINE_LENGTH_SBCS = 37
DEFAULT_MAX_LINE_LENGTH_MBCS = 30

def get_cmd_option(option : str) -> Optional[str] :
    argc = len(argv)
    if option.lower() in list(map(lambda arg: arg.lower(), argv)) :
        index = argv.index(option)
        if index < argc - 1 :
            # We found the option (for example, "--output"), so advance from that to the value (for example, "filename").
            return argv[index + 1]
        else :
            return None
    else :
        return None

def cmd_option_exists(option : str) -> bool :
    return option.lower() in list(map(lambda arg : arg.lower(), argv))

def get_languages() -> List[str] :
    retval : List[str] = []
    languages = get_cmd_option("--languages")
    if languages is not None :
        retval = list(map(lambda language : language.strip().lower(), languages.split(';')))
    return retval

def get_phrases() -> List[str] :
    retval : List[str] = []
    phrases = get_cmd_option("--phrases")
    if phrases is not None :
        retval = list(map(lambda phrase : phrase.strip(), phrases.split(';')))
    return retval

def get_compressed_audio_format() -> speechsdk.AudioStreamContainerFormat :
    value = get_cmd_option("--format")
    if value is None :
        return speechsdk.AudioStreamContainerFormat.ANY
    else :
        value = value.lower()
        if "alaw" == value : return speechsdk.AudioStreamContainerFormat.ALAW
        elif "flac" == value : return speechsdk.AudioStreamContainerFormat.FLAC
        elif "mp3" == value : return speechsdk.AudioStreamContainerFormat.MP3
        elif "mulaw" == value : return speechsdk.AudioStreamContainerFormat.MULAW
        elif "ogg_opus" == value : return speechsdk.AudioStreamContainerFormat.OGG_OPUS
        else : return speechsdk.AudioStreamContainerFormat.ANY;

def get_profanity_option() -> speechsdk.ProfanityOption :
    value = get_cmd_option("--profanity")
    if value is None :
        return speechsdk.ProfanityOption.Masked
    else :
        value = value.lower()
        if "raw"  == value: return speechsdk.ProfanityOption.Raw
        elif "remove" == value : return speechsdk.ProfanityOption.Removed
        else : return speechsdk.ProfanityOption.Masked

def user_config_from_args(usage : str) -> helper.Read_Only_Dict :
    key = get_cmd_option("--key")
    if key is None:
        raise RuntimeError("Missing subscription key.{}{}".format(linesep, usage))
    region = get_cmd_option("--region")
    if region is None:
        raise RuntimeError("Missing region.{}{}".format(linesep, usage))

    captioning_mode = CaptioningMode.REALTIME if cmd_option_exists("--realtime") and not cmd_option_exists("--offline") else CaptioningMode.OFFLINE

    td_remain_time = timedelta(seconds=1.0)
    s_remain_time = get_cmd_option("--remainTime")
    if s_remain_time is not None :
        flt_remain_time = float(s_remain_time)
        if flt_remain_time < 0.0 :
            flt_remain_time = 1.0
        td_remain_time = timedelta(seconds=flt_remain_time)

    td_delay = timedelta(seconds=1.0)
    s_delay = get_cmd_option("--delay")
    if s_delay is not None :
        flt_delay = float(s_delay)
        if flt_delay < 0.0 :
            flt_delay = 1.0
        td_delay = timedelta(seconds=flt_delay)
    
    int_max_line_length = DEFAULT_MAX_LINE_LENGTH_SBCS
    s_max_line_length = get_cmd_option("--maxLineLength")
    if s_max_line_length is not None :
        int_max_line_length = int(s_max_line_length)
        if int_max_line_length < 20 :
            int_max_line_length = 20
    
    int_lines = 2
    s_lines = get_cmd_option("--lines")
    if s_lines is not None :
        int_lines = int(s_lines)
        if int_lines < 1 :
            int_lines = 2

    return helper.Read_Only_Dict({
        "use_compressed_audio" : cmd_option_exists("--format"),
        "compressed_audio_format" : get_compressed_audio_format(),
        "profanity_option" : get_profanity_option(),
        "languages" : get_languages(),
        "input_file" : get_cmd_option("--input"),
        "output_file" : get_cmd_option("--output"),        
        "phrases" : get_phrases(),
        "suppress_console_output" : cmd_option_exists("--quiet"),
        "captioning_mode" : captioning_mode,
        "remain_time" : td_remain_time,
        "delay" : td_delay,
        "use_sub_rip_text_caption_format" : cmd_option_exists("--srt"),
        "max_line_length" : int_max_line_length,
        "lines" : int_lines,
        "stable_partial_result_threshold" : get_cmd_option("--threshold"),
        "subscription_key" : key,
        "region" : region,
    })

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import argparse
from termcolor import colored

from microsoft_video_translation_client.video_translation_enum import *
from microsoft_video_translation_client.video_translation_dataclass import *
from microsoft_video_translation_client.video_translation_client import *

def handle_create_translation_and_iteration_and_wait_until_terminated(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.sub_key,
        api_version = args.api_version,
    )

    success, error, translation, iteration = client.create_translate_and_run_first_iteration_until_terminated(
        video_file_url = args.video_file_blob_url,
        source_locale = args.source_locale,
        target_locale = args.target_locale,
        voice_kind = args.voice_kind,
        speaker_count = args.speaker_count,
        subtitle_max_char_count_per_segment = args.subtitle_max_char_count_per_segment,
        export_subtitle_in_video = args.export_subtitle_in_video,
    )
    if not success:
        return
    print(colored("success", 'green'))

def handle_request_create_translation_api(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.sub_key,
        api_version = args.api_version,
    )

    operation_id = args.operationId
    if operation_id is None:
        operation_id = str(uuid.uuid4())
    success, error, translation, operation_location = client.request_create_translation(
        translation_id = args.translation_id,
        video_file_url = args.video_file_blob_url,
        source_locale = args.source_locale,
        target_locale = args.target_locale,
        voice_kind = args.voice_kind,
        speaker_count = args.speaker_count,
        subtitle_max_char_count_per_segment = args.subtitle_max_char_count_per_segment,
        export_subtitle_in_video = args.export_subtitle_in_wideo,
        translation_display_name = args.translation_display_name,
        translation_description = args.translation_description,
        operation_id = operation_id,
    )
    if not success:
        print(colored(f"Failed to request create translation API with error: {error}", 'red'))
        return
    print(colored("succesfully created translation:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(translation), indent = 2)
    print(json_formatted_str)
    print(f"Operation location: {operation_location}")

def handle_request_create_iteration_api(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.sub_key,
        api_version = args.api_version,
    )

    success, error, iteration, operation_location = client.request_create_iteration(
        translation_id = args.translation_id,
        iteration_id = args.iteration_id,
        webvtt_file_kind = args.webvtt_file_kind,
        webvtt_file_url = args.webvtt_file_blob_url,
        speaker_count = args.speaker_count,
        subtitle_max_char_count_per_segment = args.subtitle_max_char_count_per_segment,
        export_subtitle_in_video = args.export_subtitle_in_wideo,
        iteration_description = args.iteration_description,
        operation_id = args.operation_id,
    )
    if not success:
        print(colored(f"Failed to request create iteration API with error: {error}", 'red'))
        return
    print(colored("succesfully created iteration:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(iteration), indent = 2)
    print(json_formatted_str)
    print(f"Operation location: {operation_location}")
    
def handle_request_get_operation_api(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.sub_key,
        api_version = args.api_version,
    )

    success, error, operation = client.request_get_operation(
        operation_location = urllib3.util.parse_url(args.operation_location),
        printUrl = True,
    )
    if not success:
        print(colored(f"Failed to request get operation API with error: {error}", 'red'))
        return
    print(colored("succesfully get operation:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(operation), indent = 2)
    print(json_formatted_str)

def handle_request_get_translation_api(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.sub_key,
        api_version = args.api_version,
    )

    success, error, translation = client.request_get_translation(
        translation_id = args.translation_id,
    )
    if not success:
        print(colored(f"Failed to request get translation API with error: {error}", 'red'))
        return
    if translation is None:
        print(colored("Translation not found", 'yellow'))
    else:
        print(colored("succesfully get translation:", 'green'))
        json_formatted_str = json.dumps(dataclasses.asdict(translation), indent = 2)
        print(json_formatted_str)
        
def handle_request_get_iteration_api(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.sub_key,
        api_version = args.api_version,
    )

    success, error, iteration = client.request_get_iteration(
        translation_id = args.translation_id,
        iteration_id = args.iteration_id,
    )
    if not success:
        print(colored(f"Failed to request get iteration API with error: {error}", 'red'))
        return
    if iteration is None:
        print(colored("Iteration not found", 'yellow'))
    else:
        print(colored("succesfully get iteration:", 'green'))
        json_formatted_str = json.dumps(dataclasses.asdict(iteration), indent = 2)
        print(json_formatted_str)

def handle_request_list_translations_api(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.sub_key,
        api_version = args.api_version,
    )

    success, error, translation = client.request_list_translations()
    if not success:
        print(colored(f"Failed to request list translation API with error: {error}", 'red'))
        return
    print(colored("succesfully list translations:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(translation), indent = 2)
    print(json_formatted_str)
    
def handle_request_delete_translation_api(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.sub_key,
        api_version = args.api_version,
    )

    success, error = client.request_delete_translation(args.translationId)
    if not success:
        print(colored(f"Failed to request delete translation API with error: {error}", 'red'))
        return
    print(colored("succesfully delete translation.", 'green'))

def handle_create_iteration_with_webvtt_and_wait_until_terminated(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.sub_key,
        api_version = args.api_version,
    )

    success, error, translation, iteration = client.run_iteration_with_webvtt_until_terminated(
        translation_id = args.translation_id,
        webvtt_file_kind = args.webvtt_file_kind,
        webvtt_file_url = args.webvtt_file_blob_url,
        speaker_count = args.speaker_count,
        subtitle_max_char_count_per_segment = args.subtitle_max_char_count_per_segment,
        export_subtitle_in_video = args.export_subtitle_in_video,
    )
    if not success:
        print(colored(error, 'red'))
        return
    print(colored("success", 'green'))

root_parser = argparse.ArgumentParser(
    prog='main.py',
    description='Translate video from source locale to target locale with PersonalVoice or PlatformVoice',
    epilog='Microsoft VideoTranslation'
)

root_parser.add_argument("--region", required = True, help="specify speech resource region.")
root_parser.add_argument("--sub_key", required = True, help="specify speech resource subscription key.")
root_parser.add_argument("--api_version", required = True, help="specify API version.")
sub_parsers = root_parser.add_subparsers(required=True, help='subcommand help')

translate_parser = sub_parsers.add_parser('create_translation_and_iteration_and_wait_until_terminated', help='Create translation with video file blob url, and create first iteration for the translation.')
translate_parser.add_argument('--video_file_blob_url', required = True, type=str, help='Video file blob url for video translation.')
translate_parser.add_argument('--source_locale', required = True, type=str, help='Source locale of the video file.')
translate_parser.add_argument('--target_locale', required = True, type=str, help='Target locale of the translation.')
translate_parser.add_argument('--voice_kind', required = True, type=str, help='Voice kind used for TTS synthesizing, value can be one of: PlatformVoice or PersonalVoice.')
translate_parser.add_argument('--speaker_count', required = False, type=int, help='Speaker count of the video, optional, auto detect if not provided.')
translate_parser.add_argument('--subtitle_max_char_count_per_segment', required = False, type=int, help='Subtitle max char count per segment, optional.')
translate_parser.add_argument('--export_subtitle_in_video', required = False, type=bool, help='Whether export subtitle in translated video, optional, False by default.')
translate_parser.set_defaults(func = handle_create_translation_and_iteration_and_wait_until_terminated)

translate_parser = sub_parsers.add_parser('create_iteration_with_webvtt_and_wait_until_terminated', help='Create iteration with provided webvtt based on created translation.')
translate_parser.add_argument('--translation_id', required = True, type=str, help='Translation ID.')
translate_parser.add_argument('--webvtt_file_kind', required = True, type=str, help='Webvtt file kind, one of: MetadataJson, SourceLocaleSubtitle, TargetLocaleSubtitle')
translate_parser.add_argument('--webvtt_file_blob_url', required = True, type=str, help='Webvtt file blob url for webvtt file.')
translate_parser.add_argument('--speaker_count', required = False, type=int, help='Speaker count of the video, optional, auto detect if not provided.')
translate_parser.add_argument('--subtitle_max_char_count_per_segment', required = False, type=int, help='Subtitle max char count per segment, optional.')
translate_parser.add_argument('--export_subtitle_in_wideo', required = False, type=bool, help='Whether export subtitle in translated video, optional, False by default.')
translate_parser.set_defaults(func = handle_create_iteration_with_webvtt_and_wait_until_terminated)

translate_parser = sub_parsers.add_parser('request_create_translation_api', help='Request create translation API.')
translate_parser.add_argument('--translation_id', required = True, type=str, help='Translation ID.')
translate_parser.add_argument('--video_file_blob_url', required = True, type=str, help='Video file blob url for video translation.')
translate_parser.add_argument('--source_locale', required = True, type=str, help='Source locale of the video file.')
translate_parser.add_argument('--target_locale', required = True, type=str, help='Target locale of the translation.')
translate_parser.add_argument('--voice_kind', required = True, type=str, help='Voice kind used for TTS synthesizing, value can be one of: PlatformVoice or PersonalVoice.')
translate_parser.add_argument('--speaker_count', required = False, type=int, help='Speaker count of the video, optional, auto detect if not provided.')
translate_parser.add_argument('--subtitle_max_char_count_per_segment', required = False, type=int, help='Subtitle max char count per segment, optional.')
translate_parser.add_argument('--export_subtitle_in_wideo', required = False, type=bool, help='Whether export subtitle in translated video, optional, False by default.')
translate_parser.add_argument('--translation_display_name', required = False, type=str, help='Translation display name.')
translate_parser.add_argument('--translation_description', required = False, type=str, help='Translation description.')
translate_parser.add_argument('--operation_id', required = False, type=str, help='Specify operation ID.')
translate_parser.set_defaults(func = handle_request_create_translation_api)

translate_parser = sub_parsers.add_parser('request_get_operation_api', help='Request get operation API.')
translate_parser.add_argument('--operation_location', required = True, type=str, help='Operation location.')
translate_parser.set_defaults(func = handle_request_get_operation_api)

translate_parser = sub_parsers.add_parser('request_get_translation_api', help='Request get translation API.')
translate_parser.add_argument('--translation_id', required = True, type=str, help='Translation ID.')
translate_parser.set_defaults(func = handle_request_get_translation_api)

translate_parser = sub_parsers.add_parser('request_list_translations_api', help='Request list translations API.')
translate_parser.set_defaults(func = handle_request_list_translations_api)

translate_parser = sub_parsers.add_parser('request_delete_translation_api', help='Request delete translation API.')
translate_parser.add_argument('--translation_id', required = True, type=str, help='Translation ID.')
translate_parser.set_defaults(func = handle_request_delete_translation_api)

translate_parser = sub_parsers.add_parser('request_create_iteration_api', help='Request create iteration API.')
translate_parser.add_argument('--translation_id', required = True, type=str, help='Translation ID.')
translate_parser.add_argument('--iteration_id', required = True, type=str, help='Iteration ID.')
translate_parser.add_argument('--webvtt_file_blob_url', required = False, type=str, help='Webvtt file blob url for content editing.')
translate_parser.add_argument('--webvtt_file_kind', required = False, type=str, help='Webvtt file kind, one of: MetadataJson, SourceLocaleSubtitle, TargetLocaleSubtitle.')
translate_parser.add_argument('--speaker_count', required = False, type=int, help='Speaker count of the video, optional, auto detect if not provided.')
translate_parser.add_argument('--subtitle_max_char_count_per_segment', required = False, type=int, help='Subtitle max char count per segment, optional.')
translate_parser.add_argument('--export_subtitle_in_video', required = False, type=bool, help='Whether export subtitle in translated video, optional, False by default.')
translate_parser.add_argument('--iteration_description', required = False, type=str, help='Iteration description.')
translate_parser.add_argument('--operation_id', required = False, type=str, help='Specify operation ID.')
translate_parser.set_defaults(func = handle_request_create_iteration_api)

translate_parser = sub_parsers.add_parser('request_get_iteration_api', help='Request get iteration API.')
translate_parser.add_argument('--translation_id', required = True, type=str, help='Translation ID.')
translate_parser.add_argument('--iteration_id', required = True, type=str, help='Iteration ID.')
translate_parser.set_defaults(func = handle_request_get_iteration_api)

args = root_parser.parse_args()
args.func(args)

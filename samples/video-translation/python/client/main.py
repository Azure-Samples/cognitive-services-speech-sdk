# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import argparse
from termcolor import colored

from microsoft_video_translation_client.video_translation_enum import *
from microsoft_video_translation_client.video_translation_dataclass import *
from microsoft_video_translation_client.video_translation_client import *

def handleCreateTranslationAndIterationAndWaitUntilTerminated(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.subKey,
    )

    success, error, translation, iteration = client.create_translate_and_run_first_iteration_until_terminated(
        video_file_url = args.videoFileBlobUrl,
        source_locale = args.sourceLocale,
        target_locale = args.targetLocale,
        voice_kind = args.voiceKind,
        speaker_count = args.speakerCount,
        subtitle_max_char_count_per_segment = args.subtitleMaxCharCountPerSegment,
        export_subtitle_in_video = args.exportSubtitleInWideo,
    )
    if not success:
        return
    print(colored("success", 'green'))

def handleRequestCreateTranslationAPI(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.subKey,
    )

    operation_id = args.operationId
    if operation_id is None:
        operation_id = str(uuid.uuid4())
    success, error, translation, operation_location = client.request_create_translation(
        translation_id = args.translationId,
        video_file_url = args.videoFileBlobUrl,
        source_locale = args.sourceLocale,
        target_locale = args.targetLocale,
        voice_kind = args.voiceKind,
        speaker_count = args.speakerCount,
        subtitle_max_char_count_per_segment = args.subtitleMaxCharCountPerSegment,
        export_subtitle_in_video = args.exportSubtitleInWideo,
        translation_display_name = args.translationDisplayName,
        translation_description = args.translationDescription,
        operation_id = operation_id,
    )
    if not success:
        print(colored(f"Failed to request create translation API with error: {error}", 'red'))
        return
    print(colored("succesfully created translation:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(translation), indent = 2)
    print(json_formatted_str)
    print(f"Operation location: {operation_location}")

def handleRequestCreateIterationAPI(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.subKey,
    )

    success, error, iteration, operation_location = client.request_create_iteration(
        translation_id = args.translationId,
        iteration_id = args.iterationId,
        webvtt_file_kind = args.webvttFileKind,
        webvtt_file_url = args.webvttFileBlobUrl,
        speaker_count = args.speakerCount,
        subtitle_max_char_count_per_segment = args.subtitleMaxCharCountPerSegment,
        export_subtitle_in_video = args.exportSubtitleInWideo,
        iteration_description = args.iterationDescription,
        operation_id = args.operationId,
    )
    if not success:
        print(colored(f"Failed to request create iteration API with error: {error}", 'red'))
        return
    print(colored("succesfully created iteration:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(iteration), indent = 2)
    print(json_formatted_str)
    print(f"Operation location: {operation_location}")
    
def handleRequestGetOperationAPI(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.subKey,
    )

    success, error, operation = client.request_get_operation(
        operation_location = urllib3.util.parse_url(args.operationLocation),
        printUrl = True,
    )
    if not success:
        print(colored(f"Failed to request get operation API with error: {error}", 'red'))
        return
    print(colored("succesfully get operation:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(operation), indent = 2)
    print(json_formatted_str)

def handleRequestGetTranslationAPI(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.subKey,
    )

    success, error, translation = client.request_get_translation(
        translation_id = args.translationId,
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
        
def handleRequestGetIterationAPI(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.subKey,
    )

    success, error, iteration = client.request_get_iteration(
        translation_id = args.translationId,
        iteration_id = args.iterationId,
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

def handleRequestListTranslationsAPI(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.subKey,
    )

    success, error, translation = client.request_list_translations()
    if not success:
        print(colored(f"Failed to request list translation API with error: {error}", 'red'))
        return
    print(colored("succesfully list translations:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(translation), indent = 2)
    print(json_formatted_str)
    
def handleRequestDeleteTranslationAPI(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.subKey,
    )

    success, error = client.request_delete_translation(args.translationId)
    if not success:
        print(colored(f"Failed to request delete translation API with error: {error}", 'red'))
        return
    print(colored("succesfully delete translation.", 'green'))

def handleCreateIterationWithWebvttAndWaitUntilTerminated(args):
    client = VideoTranslationClient(
        region = args.region,
        sub_key = args.subKey,
    )

    success, error, translation, iteration = client.run_iteration_with_webvtt_until_terminated(
        translation_id = args.translationId,
        webvtt_file_kind = args.webvttFileKind,
        webvtt_file_url = args.webvttFileBlobUrl,
        speaker_count = args.speakerCount,
        subtitle_max_char_count_per_segment = args.subtitleMaxCharCountPerSegment,
        export_subtitle_in_video = args.exportSubtitleInWideo,
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
root_parser.add_argument("--subKey", required = True, help="specify speech resource subscription key.")
sub_parsers = root_parser.add_subparsers(required=True, help='subcommand help')

translate_parser = sub_parsers.add_parser('createTranslationAndIterationAndWaitUntilTerminated', help='Create translation with video file blob url, and create first iteration for the translation.')
translate_parser.add_argument('--videoFileBlobUrl', required = True, type=str, help='Video file blob url for video translation.')
translate_parser.add_argument('--sourceLocale', required = True, type=str, help='Source locale of the video file.')
translate_parser.add_argument('--targetLocale', required = True, type=str, help='Target locale of the translation.')
translate_parser.add_argument('--voiceKind', required = True, type=str, help='Voice kind used for TTS synthesizing, value can be one of: PlatformVoice or PersonalVoice.')
translate_parser.add_argument('--speakerCount', required = False, type=int, help='Speaker count of the video, optional, auto detect if not provided.')
translate_parser.add_argument('--subtitleMaxCharCountPerSegment', required = False, type=int, help='Subtitle max char count per segment, optional.')
translate_parser.add_argument('--exportSubtitleInWideo', required = False, type=bool, help='Whether export subtitle in translated video, optional, False by default.')
translate_parser.set_defaults(func = handleCreateTranslationAndIterationAndWaitUntilTerminated)

translate_parser = sub_parsers.add_parser('createIterationWithWebvttAndWaitUntilTerminated', help='Create iteration with provided webvtt based on created translation.')
translate_parser.add_argument('--translationId', required = True, type=str, help='Translation ID.')
translate_parser.add_argument('--webvttFileKind', required = True, type=str, help='Webvtt file kind, one of: MetadataJson, SourceLocaleSubtitle, TargetLocaleSubtitle')
translate_parser.add_argument('--webvttFileBlobUrl', required = True, type=str, help='Webvtt file blob url for webvtt file.')
translate_parser.add_argument('--speakerCount', required = False, type=int, help='Speaker count of the video, optional, auto detect if not provided.')
translate_parser.add_argument('--subtitleMaxCharCountPerSegment', required = False, type=int, help='Subtitle max char count per segment, optional.')
translate_parser.add_argument('--exportSubtitleInWideo', required = False, type=bool, help='Whether export subtitle in translated video, optional, False by default.')
translate_parser.set_defaults(func = handleCreateIterationWithWebvttAndWaitUntilTerminated)

translate_parser = sub_parsers.add_parser('requestCreateTranslationAPI', help='Request create translation API.')
translate_parser.add_argument('--translationId', required = True, type=str, help='Translation ID.')
translate_parser.add_argument('--videoFileBlobUrl', required = True, type=str, help='Video file blob url for video translation.')
translate_parser.add_argument('--sourceLocale', required = True, type=str, help='Source locale of the video file.')
translate_parser.add_argument('--targetLocale', required = True, type=str, help='Target locale of the translation.')
translate_parser.add_argument('--voiceKind', required = True, type=str, help='Voice kind used for TTS synthesizing, value can be one of: PlatformVoice or PersonalVoice.')
translate_parser.add_argument('--speakerCount', required = False, type=int, help='Speaker count of the video, optional, auto detect if not provided.')
translate_parser.add_argument('--subtitleMaxCharCountPerSegment', required = False, type=int, help='Subtitle max char count per segment, optional.')
translate_parser.add_argument('--exportSubtitleInWideo', required = False, type=bool, help='Whether export subtitle in translated video, optional, False by default.')
translate_parser.add_argument('--translationDisplayName', required = False, type=str, help='Translation display name.')
translate_parser.add_argument('--translationDescription', required = False, type=str, help='Translation description.')
translate_parser.add_argument('--operationId', required = False, type=str, help='Specify operation ID.')
translate_parser.set_defaults(func = handleRequestCreateTranslationAPI)

translate_parser = sub_parsers.add_parser('requestGetOperationAPI', help='Request get operation API.')
translate_parser.add_argument('--operationLocation', required = True, type=str, help='Operation location.')
translate_parser.set_defaults(func = handleRequestGetOperationAPI)

translate_parser = sub_parsers.add_parser('requestGetTranslationAPI', help='Request get translation API.')
translate_parser.add_argument('--translationId', required = True, type=str, help='Translation ID.')
translate_parser.set_defaults(func = handleRequestGetTranslationAPI)

translate_parser = sub_parsers.add_parser('requestListTranslationsAPI', help='Request list translations API.')
translate_parser.set_defaults(func = handleRequestListTranslationsAPI)

translate_parser = sub_parsers.add_parser('requestDeleteTranslationAPI', help='Request delete translation API.')
translate_parser.add_argument('--translationId', required = True, type=str, help='Translation ID.')
translate_parser.set_defaults(func = handleRequestDeleteTranslationAPI)

translate_parser = sub_parsers.add_parser('requestCreateIterationAPI', help='Request create iteration API.')
translate_parser.add_argument('--translationId', required = True, type=str, help='Translation ID.')
translate_parser.add_argument('--iterationId', required = True, type=str, help='Iteration ID.')
translate_parser.add_argument('--webvttFileBlobUrl', required = False, type=str, help='Webvtt file blob url for content editing.')
translate_parser.add_argument('--webvttFileKind', required = False, type=str, help='Webvtt file kind, one of: MetadataJson, SourceLocaleSubtitle, TargetLocaleSubtitle.')
translate_parser.add_argument('--speakerCount', required = False, type=int, help='Speaker count of the video, optional, auto detect if not provided.')
translate_parser.add_argument('--subtitleMaxCharCountPerSegment', required = False, type=int, help='Subtitle max char count per segment, optional.')
translate_parser.add_argument('--exportSubtitleInWideo', required = False, type=bool, help='Whether export subtitle in translated video, optional, False by default.')
translate_parser.add_argument('--iterationDescription', required = False, type=str, help='Iteration description.')
translate_parser.add_argument('--operationId', required = False, type=str, help='Specify operation ID.')
translate_parser.set_defaults(func = handleRequestCreateIterationAPI)

translate_parser = sub_parsers.add_parser('requestGetIterationAPI', help='Request get iteration API.')
translate_parser.add_argument('--translationId', required = True, type=str, help='Translation ID.')
translate_parser.add_argument('--iterationId', required = True, type=str, help='Iteration ID.')
translate_parser.set_defaults(func = handleRequestGetIterationAPI)

args = root_parser.parse_args()
args.func(args)

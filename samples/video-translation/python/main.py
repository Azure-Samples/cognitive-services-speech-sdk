# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import argparse
import json
import dataclasses
import uuid
import urllib3
from termcolor import colored
from microsoft_video_translation_client.video_translation_client import VideoTranslationClient


ARGUMENT_HELP_VIDEO_FILE_BLOB_URL = (
    'Translation video file url, .mp4 file format, maxmum 5GB file size and 4 hours video duration. '
    'Provide the input media file using either videoFileUrl or audioFileUrl, these parameters are '
    'mutually exclusive—only one of them is required. If both are provided, the request will be considered invalid.'
)
ARGUMENT_HELP_AUDIO_FILE_BLOB_URL = (
    'Translation audio file url, .mp3 or .wav file format, maxmum 5GB file size and 4 hours video duration. '
    'Provide the input media file using either videoFileUrl or audioFileUrl, these parameters are '
    'mutually exclusive—only one of them is required. If both are provided, the request will be considered invalid.'
)
ARGUMENT_HELP_SOURCE_LOCALE = (
    'The source locale of the video file. Locale code follows BCP-47. You can find the text to speech locale list '
    'here https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts , if not specified, '
    'the source locale will be auto-detected from the video file, the auto detect is only supported after version 2025-05-20.'
)
ARGUMENT_HELP_TARGET_LOCALE = (
    'The target locale of the translation. Locale code follows BCP-47. You can find the text to speech locale list '
    'here https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts.'
)
ARGUMENT_HELP_VOICE_KIND = 'Voice kind used for TTS synthesizing, value can be one of: PlatformVoice or PersonalVoice.'
ARGUMENT_HELP_ENABLE_LIP_SYNC = (
    'Indicate whether to enable lip sync, if not provided, '
    'the default value is false to disable the lip sync.'
)
ARGUMENT_HELP_ENABLE_EMOTIONAL_PLATFORM_VOICE = 'Indicate whether enable emotional platform voice. '
ARGUMENT_HELP_SPEAKER_COUNT = 'Number of speakers in the video, if not provided, it will be auto-detected from the video file.'
ARGUMENT_HELP_SUBTITLE_MAX_CHAR_COUNT_PER_SEGMENT = (
    'Subtitle max display character count per segment, if not provided, it will use the '
    'language dependent default value.'
)
ARGUMENT_HELP_EXPORT_SUBTITLE_IN_VIDEO = (
    'Export subtitle in video, if not specified, the default value is false, it will not burn '
    'subtitle to the translated video file.'
)
ARGUMENT_HELP_TTS_CUSTOM_LEXICON_FILE_URL = (
    'Translate with TTS custom lexicon for TTS synthesis, provide the custom lexicon file with URL. '
    'ttsCustomLexiconFileUrl and ttsCustomLexiconFileIdInAudioContentCreation are conflict, only one of them are required.'
)
ARGUMENT_HELP_TTS_CUSTOM_LEXICON_FILE_ID_IN_AUDIO_CONTENT_CREATION = (
    'Translate with TTS custom lexicon for TTS synthesis, provide the custom lexicon file with file ID in Audio Content Creation. '
    'ttsCustomLexiconFileUrl and ttsCustomLexiconFileIdInAudioContentCreation are conflict, only one of them are required.'
)
ARGUMENT_HELP_ENABLE_VIDEO_SPEED_ADJUSTMENT = (
    'This parameter allows for the adjustment of video playback speed to ensure better alignment with translated audio. '
    'When enabled, the API can slow down or speed up the video to match the timing of the translated audio, '
    'providing a more synchronized and seamless viewing experience.'
)
ARGUMENT_HELP_ENABLE_OCR_CORRECTION_FROM_SUBTITLE = (
    'Indicate whether allow the API to correct the speech recognition (SR) results using the subtitles '
    'from the original video file. By leveraging the existing subtitles, the API can enhance the accuracy '
    'of the transcribed text, ensuring that the final output is more precise and reliable.'
)
ARGUMENT_HELP_EXPORT_TARGET_LOCALE_ADVANCED_SUBTITLE_FILE = (
    'This parameter, when enabled, allows the API to export subtitles in the Advanced SubStation Alpha format. '
    'The subtitle file can specify font styles and colors, which helps in addressing character display issues '
    'in certain target locales such as Arabic (Ar), Japanese (Ja), Korean (Ko), and Chinese (Ch). By using this parameter, '
    'you can ensure that the subtitles are visually appealing and correctly rendered across different languages and regions.'
)
ARGUMENT_HELP_SUBTITLE_PRIMARY_COLOR = (
    'This parameter specifies the primary color of the subtitles in the video translation output. '
    'The value should be provided in the format <rr><gg><bb>, #<rr><gg><bb>, <rr><gg><bb><aa> or #<rr><gg><bb><aa>, '
    'where <rr> represents the red component of the color, <gg> represents the green component, <bb> represents the blue component, '
    '<aa> represents the alpha component. For example, EBA205 or #EBA205 would set the subtitle color to a specific shade of yellow. '
    'This parameter allows for customization of subtitle appearance to enhance readability and visual appeal.'
)
ARGUMENT_HELP_SUBTITLE_OUTLINE_COLOR = (
    'This parameter specifies the outline color of the subtitles in the video translation output. '
    'The value should be provided in the format <rr><gg><bb>, #<rr><gg><bb>, <rr><gg><bb><aa> or #<rr><gg><bb><aa>, '
    'where <rr> represents the red component of the color, <gg> represents the green component, '
    '<bb> represents the blue component, <aa> represents the alpha component. For example, '
    'EBA205 or #EBA205 would set the subtitle color to a specific shade of yellow. '
    'This parameter allows for customization of subtitle appearance to enhance readability and visual appeal.'
)
ARGUMENT_HELP_SUBTITLE_FONT_SIZE = (
    'This parameter specifies the font size of subtitles in the video translation output. '
    'If not specified, it will use the language dependent default value.'
)
ARGUMENT_HELP_WEBVTT_FILE_KIND = (
    'Webvtt file kind for content editing, this parameter is required from the second iteration creation '
    'request of the translation, one of: MetadataJson, SourceLocaleSubtitle, TargetLocaleSubtitle.'
)
ARGUMENT_HELP_WEBVTT_FILE_BLOB_URL = (
    'Webvtt file blob url for content editing, this parameter is required from the second '
    'iteration creation request of the translation.'
)


def handle_create_translation_and_iteration_and_wait_until_terminated(args):
    client = VideoTranslationClient(
        region=args.region,
        sub_key=args.sub_key,
        api_version=args.api_version,
    )

    success, error, translation, iteration = client.create_translate_and_run_first_iteration_until_terminated(
        video_file_url=args.video_file_blob_url,
        audio_file_url=args.audio_file_blob_url,
        source_locale=args.source_locale,
        target_locale=args.target_locale,
        voice_kind=args.voice_kind,
        enable_lip_sync=args.enable_lip_sync,
        enable_emotional_platform_voice=args.enable_emotional_platform_voice,
        speaker_count=args.speaker_count,
        subtitle_max_char_count_per_segment=args.subtitle_max_char_count_per_segment,
        export_subtitle_in_video=args.export_subtitle_in_video,
        tts_custom_lexicon_file_url=args.tts_custom_lexicon_file_url,
        tts_custom_lexicon_file_id_in_audio_content_creation=args.tts_custom_lexicon_file_id_in_audio_content_creation,
        enable_video_speed_adjustment=args.enable_video_speed_adjustment,
        enable_ocr_correction_from_subtitle=args.enable_ocr_correction_from_subtitle,
        export_target_locale_advanced_subtitle_file=args.export_target_locale_advanced_subtitle_file,
        subtitle_primary_color=args.subtitle_primary_color,
        subtitle_outline_color=args.subtitle_outline_color,
        subtitle_font_size=args.subtitle_font_size,
    )
    if not success:
        return

    print(colored("success", "green"))


def handle_request_create_translation_api(args):
    client = VideoTranslationClient(
        region=args.region,
        sub_key=args.sub_key,
        api_version=args.api_version,
    )

    operation_id = args.operation_id
    if operation_id is None:
        operation_id = str(uuid.uuid4())
    success, error, translation, operation_location = client.request_create_translation(
        translation_id=args.translation_id,
        video_file_url=args.video_file_blob_url,
        audio_file_url=args.audio_file_blob_url,
        source_locale=args.source_locale,
        target_locale=args.target_locale,
        voice_kind=args.voice_kind,
        enable_lip_sync=args.enable_lip_sync,
        speaker_count=args.speaker_count,
        subtitle_max_char_count_per_segment=args.subtitle_max_char_count_per_segment,
        export_subtitle_in_video=args.export_subtitle_in_video,
        translation_display_name=args.translation_display_name,
        translation_description=args.translation_description,
        operation_id=operation_id,
    )
    if not success:
        print(colored(f"Failed to request create translation API with error: {error}", "red"))
        return
    print(colored("succesfully created translation:", "green"))
    json_formatted_str = json.dumps(dataclasses.asdict(translation), indent=2)
    print(json_formatted_str)
    print(f"Operation location: {operation_location}")


def handle_request_create_iteration_api(args):
    client = VideoTranslationClient(
        region=args.region,
        sub_key=args.sub_key,
        api_version=args.api_version,
    )

    success, error, iteration, operation_location = client.request_create_iteration(
        translation_id=args.translation_id,
        iteration_id=args.iteration_id,
        webvtt_file_kind=args.webvtt_file_kind,
        webvtt_file_url=args.webvtt_file_blob_url,
        speaker_count=args.speaker_count,
        subtitle_max_char_count_per_segment=args.subtitle_max_char_count_per_segment,
        export_subtitle_in_video=args.export_subtitle_in_video,
        iteration_description=args.iteration_description,
        operation_id=args.operation_id,
        tts_custom_lexicon_file_url=args.tts_custom_lexicon_file_url,
        tts_custom_lexicon_file_id_in_audio_content_creation=args.tts_custom_lexicon_file_id_in_audio_content_creation,
        enable_video_speed_adjustment=args.enable_video_speed_adjustment,
        enable_ocr_correction_from_subtitle=args.enable_ocr_correction_from_subtitle,
        export_target_locale_advanced_subtitle_file=args.export_target_locale_advanced_subtitle_file,
        subtitle_primary_color=args.subtitle_primary_color,
        subtitle_outline_color=args.subtitle_outline_color,
        subtitle_font_size=args.subtitle_font_size
    )
    if not success:
        print(colored(f"Failed to request create iteration API with error: {error}", 'red'))
        return
    print(colored("succesfully created iteration:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(iteration), indent=2)
    print(json_formatted_str)
    print(f"Operation location: {operation_location}")


def handle_request_get_operation_api(args):
    client = VideoTranslationClient(
        region=args.region,
        sub_key=args.sub_key,
        api_version=args.api_version,
    )

    success, error, operation = client.request_get_operation(
        operation_location=urllib3.util.parse_url(args.operation_location),
        printUrl=True,
    )
    if not success:
        print(colored(f"Failed to request get operation API with error: {error}", 'red'))
        return
    print(colored("succesfully get operation:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(operation), indent=2)
    print(json_formatted_str)


def handle_request_get_translation_api(args):
    client = VideoTranslationClient(
        region=args.region,
        sub_key=args.sub_key,
        api_version=args.api_version,
    )

    success, error, translation = client.request_get_translation(
        translation_id=args.translation_id,
    )
    if not success:
        print(colored(f"Failed to request get translation API with error: {error}", 'red'))
        return
    if translation is None:
        print(colored("Translation not found", 'yellow'))
    else:
        print(colored("succesfully get translation:", 'green'))
        json_formatted_str = json.dumps(dataclasses.asdict(translation), indent=2)
        print(json_formatted_str)


def handle_request_get_iteration_api(args):
    client = VideoTranslationClient(
        region=args.region,
        sub_key=args.sub_key,
        api_version=args.api_version,
    )

    success, error, iteration = client.request_get_iteration(
        translation_id=args.translation_id,
        iteration_id=args.iteration_id,
    )
    if not success:
        print(colored(f"Failed to request get iteration API with error: {error}", 'red'))
        return
    if iteration is None:
        print(colored("Iteration not found", 'yellow'))
    else:
        print(colored("succesfully get iteration:", 'green'))
        json_formatted_str = json.dumps(dataclasses.asdict(iteration), indent=2)
        print(json_formatted_str)


def handle_request_list_translations_api(args):
    client = VideoTranslationClient(
        region=args.region,
        sub_key=args.sub_key,
        api_version=args.api_version,
    )

    success, error, translation = client.request_list_translations()
    if not success:
        print(colored(f"Failed to request list translation API with error: {error}", 'red'))
        return
    print(colored("succesfully list translations:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(translation), indent=2)
    print(json_formatted_str)


def handle_request_delete_translation_api(args):
    client = VideoTranslationClient(
        region=args.region,
        sub_key=args.sub_key,
        api_version=args.api_version,
    )

    success, error = client.request_delete_translation(args.translation_id)
    if not success:
        print(colored(f"Failed to request delete translation API with error: {error}", 'red'))
        return
    print(colored("succesfully delete translation.", 'green'))


def handle_create_iteration_with_webvtt_and_wait_until_terminated(args):
    client = VideoTranslationClient(
        region=args.region,
        sub_key=args.sub_key,
        api_version=args.api_version,
    )

    success, error, translation, iteration = client.run_iteration_with_webvtt_until_terminated(
        translation_id=args.translation_id,
        webvtt_file_kind=args.webvtt_file_kind,
        webvtt_file_url=args.webvtt_file_blob_url,
        speaker_count=args.speaker_count,
        enable_emotional_platform_voice=args.enable_emotional_platform_voice,
        subtitle_max_char_count_per_segment=args.subtitle_max_char_count_per_segment,
        export_subtitle_in_video=args.export_subtitle_in_video,
        tts_custom_lexicon_file_url=args.tts_custom_lexicon_file_url,
        tts_custom_lexicon_file_id_in_audio_content_creation=args.tts_custom_lexicon_file_id_in_audio_content_creation,
        enable_video_speed_adjustment=args.enable_video_speed_adjustment,
        enable_ocr_correction_from_subtitle=args.enable_ocr_correction_from_subtitle,
        export_target_locale_advanced_subtitle_file=args.export_target_locale_advanced_subtitle_file,
        subtitle_primary_color=args.subtitle_primary_color,
        subtitle_outline_color=args.subtitle_outline_color,
        subtitle_font_size=args.subtitle_font_size
    )
    if not success:
        print(colored(error, 'red'))
        return
    print(colored("succesfully completed iteration:", 'green'))
    json_formatted_str = json.dumps(dataclasses.asdict(iteration), indent=2)
    print(json_formatted_str)


root_parser = argparse.ArgumentParser(
    prog='main.py',
    description='Translate video from source locale to target locale with PersonalVoice or PlatformVoice',
    epilog='Microsoft VideoTranslation'
)

root_parser.add_argument("--region", required=True, help="specify speech resource region.")
root_parser.add_argument("--sub_key", required=True, help="specify speech resource subscription key.")
root_parser.add_argument("--api_version", required=True, help="specify API version.")
sub_parsers = root_parser.add_subparsers(required=True, help='subcommand help')

translate_parser = sub_parsers.add_parser(
    'create_translation_and_iteration_and_wait_until_terminated',
    help='Create translation with video file blob url, and create first iteration for the translation.')

translate_parser.add_argument('--video_file_blob_url', required=False, type=str, help=ARGUMENT_HELP_VIDEO_FILE_BLOB_URL)
translate_parser.add_argument('--audio_file_blob_url', required=False, type=str, help=ARGUMENT_HELP_AUDIO_FILE_BLOB_URL)
translate_parser.add_argument('--source_locale', required=False, type=str, help=ARGUMENT_HELP_SOURCE_LOCALE)
translate_parser.add_argument('--target_locale', required=True, type=str, help=ARGUMENT_HELP_TARGET_LOCALE)
translate_parser.add_argument('--voice_kind', required=True, type=str, help=ARGUMENT_HELP_VOICE_KIND)
translate_parser.add_argument('--enable_lip_sync', required=False, type=str, help=ARGUMENT_HELP_ENABLE_LIP_SYNC)
translate_parser.add_argument(
    '--enable_emotional_platform_voice', required=False, type=str,
    help=ARGUMENT_HELP_ENABLE_EMOTIONAL_PLATFORM_VOICE)
translate_parser.add_argument('--speaker_count', required=False, type=int, help=ARGUMENT_HELP_SPEAKER_COUNT)
translate_parser.add_argument(
    '--subtitle_max_char_count_per_segment', required=False, type=int,
    help=ARGUMENT_HELP_SUBTITLE_MAX_CHAR_COUNT_PER_SEGMENT)
translate_parser.add_argument('--export_subtitle_in_video', required=False, type=bool, help=ARGUMENT_HELP_EXPORT_SUBTITLE_IN_VIDEO)
translate_parser.add_argument(
    '--tts_custom_lexicon_file_url', required=False, type=str,
    help=ARGUMENT_HELP_TTS_CUSTOM_LEXICON_FILE_URL)
translate_parser.add_argument(
    '--tts_custom_lexicon_file_id_in_audio_content_creation', required=False, type=str,
    help=ARGUMENT_HELP_TTS_CUSTOM_LEXICON_FILE_ID_IN_AUDIO_CONTENT_CREATION)
translate_parser.add_argument(
    '--enable_video_speed_adjustment', required=False, type=bool,
    help=ARGUMENT_HELP_ENABLE_VIDEO_SPEED_ADJUSTMENT)
translate_parser.add_argument(
    '--enable_ocr_correction_from_subtitle', required=False, type=bool,
    help=ARGUMENT_HELP_ENABLE_OCR_CORRECTION_FROM_SUBTITLE)
translate_parser.add_argument(
    '--export_target_locale_advanced_subtitle_file', required=False, type=bool,
    help=ARGUMENT_HELP_EXPORT_TARGET_LOCALE_ADVANCED_SUBTITLE_FILE)
translate_parser.add_argument('--subtitle_primary_color', required=False, type=str, help=ARGUMENT_HELP_SUBTITLE_PRIMARY_COLOR)
translate_parser.add_argument('--subtitle_outline_color', required=False, type=str, help=ARGUMENT_HELP_SUBTITLE_OUTLINE_COLOR)
translate_parser.add_argument('--subtitle_font_size', required=False, type=int, help=ARGUMENT_HELP_SUBTITLE_FONT_SIZE)
translate_parser.set_defaults(func=handle_create_translation_and_iteration_and_wait_until_terminated)


translate_parser = sub_parsers.add_parser(
    'create_iteration_with_webvtt_and_wait_until_terminated',
    help='Create iteration with provided webvtt based on created translation.'
)
translate_parser.add_argument('--translation_id', required=True, type=str, help='Translation ID.')
translate_parser.add_argument('--webvtt_file_kind', required=True, type=str, help=ARGUMENT_HELP_WEBVTT_FILE_KIND)
translate_parser.add_argument('--webvtt_file_blob_url', required=True, type=str, help=ARGUMENT_HELP_WEBVTT_FILE_BLOB_URL)
translate_parser.add_argument(
    '--enable_emotional_platform_voice', required=False, type=str,
    help=ARGUMENT_HELP_ENABLE_EMOTIONAL_PLATFORM_VOICE)
translate_parser.add_argument('--speaker_count', required=False, type=int, help=ARGUMENT_HELP_SPEAKER_COUNT)
translate_parser.add_argument(
    '--subtitle_max_char_count_per_segment', required=False, type=int,
    help=ARGUMENT_HELP_SUBTITLE_MAX_CHAR_COUNT_PER_SEGMENT)
translate_parser.add_argument(
    '--export_subtitle_in_video', required=False, type=bool,
    help=ARGUMENT_HELP_EXPORT_SUBTITLE_IN_VIDEO)
translate_parser.add_argument(
    '--tts_custom_lexicon_file_url', required=False, type=str,
    help=ARGUMENT_HELP_TTS_CUSTOM_LEXICON_FILE_URL)
translate_parser.add_argument(
    '--tts_custom_lexicon_file_id_in_audio_content_creation', required=False, type=str,
    help=ARGUMENT_HELP_TTS_CUSTOM_LEXICON_FILE_ID_IN_AUDIO_CONTENT_CREATION)
translate_parser.add_argument(
    '--enable_video_speed_adjustment', required=False, type=bool,
    help=ARGUMENT_HELP_ENABLE_VIDEO_SPEED_ADJUSTMENT)
translate_parser.add_argument(
    '--enable_ocr_correction_from_subtitle', required=False, type=bool,
    help=ARGUMENT_HELP_ENABLE_OCR_CORRECTION_FROM_SUBTITLE)
translate_parser.add_argument(
    '--export_target_locale_advanced_subtitle_file', required=False, type=bool,
    help=ARGUMENT_HELP_EXPORT_TARGET_LOCALE_ADVANCED_SUBTITLE_FILE)
translate_parser.add_argument('--subtitle_primary_color', required=False, type=str, help=ARGUMENT_HELP_SUBTITLE_PRIMARY_COLOR)
translate_parser.add_argument('--subtitle_outline_color', required=False, type=str, help=ARGUMENT_HELP_SUBTITLE_OUTLINE_COLOR)
translate_parser.add_argument('--subtitle_font_size', required=False, type=int, help=ARGUMENT_HELP_SUBTITLE_FONT_SIZE)
translate_parser.set_defaults(func=handle_create_iteration_with_webvtt_and_wait_until_terminated)

translate_parser = sub_parsers.add_parser('request_create_translation_api', help='Request create translation API.')
translate_parser.add_argument('--translation_id', required=True, type=str, help='Translation resource ID.')
translate_parser.add_argument('--video_file_blob_url', required=False, type=str, help=ARGUMENT_HELP_VIDEO_FILE_BLOB_URL)
translate_parser.add_argument('--audio_file_blob_url', required=False, type=str, help=ARGUMENT_HELP_AUDIO_FILE_BLOB_URL)
translate_parser.add_argument('--source_locale', required=False, type=str, help=ARGUMENT_HELP_SOURCE_LOCALE)
translate_parser.add_argument('--target_locale', required=True, type=str, help=ARGUMENT_HELP_TARGET_LOCALE)
translate_parser.add_argument('--voice_kind', required=True, type=str, help=ARGUMENT_HELP_VOICE_KIND)
translate_parser.add_argument('--enable_lip_sync', required=False, type=str, help=ARGUMENT_HELP_ENABLE_LIP_SYNC)
translate_parser.add_argument('--speaker_count', required=False, type=int, help=ARGUMENT_HELP_SPEAKER_COUNT)
translate_parser.add_argument(
    '--subtitle_max_char_count_per_segment', required=False, type=int,
    help=ARGUMENT_HELP_SUBTITLE_MAX_CHAR_COUNT_PER_SEGMENT)
translate_parser.add_argument('--export_subtitle_in_wideo', required=False, type=bool, help=ARGUMENT_HELP_EXPORT_SUBTITLE_IN_VIDEO)
translate_parser.add_argument('--translation_display_name', required=False, type=str, help='Translation display name.')
translate_parser.add_argument('--translation_description', required=False, type=str, help='Translation description.')
translate_parser.add_argument('--operation_id', required=False, type=str, help='Specify operation ID.')
translate_parser.set_defaults(func=handle_request_create_translation_api)

translate_parser = sub_parsers.add_parser('request_get_operation_api', help='Request get operation API.')
translate_parser.add_argument('--operation_location', required=True, type=str, help='Operation location.')
translate_parser.set_defaults(func=handle_request_get_operation_api)

translate_parser = sub_parsers.add_parser('request_get_translation_api', help='Request get translation API.')
translate_parser.add_argument('--translation_id', required=True, type=str, help='Translation ID.')
translate_parser.set_defaults(func=handle_request_get_translation_api)

translate_parser = sub_parsers.add_parser('request_list_translations_api', help='Request list translations API.')
translate_parser.set_defaults(func=handle_request_list_translations_api)

translate_parser = sub_parsers.add_parser('request_delete_translation_api', help='Request delete translation API.')
translate_parser.add_argument('--translation_id', required=True, type=str, help='Translation ID.')
translate_parser.set_defaults(func=handle_request_delete_translation_api)

translate_parser = sub_parsers.add_parser('request_create_iteration_api', help='Request create iteration API.')
translate_parser.add_argument('--translation_id', required=True, type=str, help='Translation ID.')
translate_parser.add_argument('--iteration_id', required=True, type=str, help='Iteration ID.')
translate_parser.add_argument('--iteration_description', required=False, type=str, help='Iteration description.')
translate_parser.add_argument('--operation_id', required=False, type=str, help='Specify operation ID.')
translate_parser.add_argument('--webvtt_file_blob_url', required=False, type=str, help=ARGUMENT_HELP_WEBVTT_FILE_BLOB_URL)
translate_parser.add_argument('--webvtt_file_kind', required=False, type=str, help=ARGUMENT_HELP_WEBVTT_FILE_KIND)
translate_parser.add_argument('--speaker_count', required=False, type=int, help=ARGUMENT_HELP_SPEAKER_COUNT)
translate_parser.add_argument(
    '--subtitle_max_char_count_per_segment', required=False, type=int,
    help=ARGUMENT_HELP_SUBTITLE_MAX_CHAR_COUNT_PER_SEGMENT)
translate_parser.add_argument(
    '--export_subtitle_in_video', required=False, type=bool,
    help=ARGUMENT_HELP_EXPORT_SUBTITLE_IN_VIDEO)
translate_parser.add_argument(
    '--tts_custom_lexicon_file_url', required=False, type=str,
    help=ARGUMENT_HELP_TTS_CUSTOM_LEXICON_FILE_URL)
translate_parser.add_argument(
    '--tts_custom_lexicon_file_id_in_audio_content_creation', required=False, type=str,
    help=ARGUMENT_HELP_TTS_CUSTOM_LEXICON_FILE_ID_IN_AUDIO_CONTENT_CREATION)
translate_parser.add_argument(
    '--enable_video_speed_adjustment', required=False, type=bool,
    help=ARGUMENT_HELP_ENABLE_VIDEO_SPEED_ADJUSTMENT)
translate_parser.add_argument(
    '--enable_ocr_correction_from_subtitle', required=False, type=bool,
    help=ARGUMENT_HELP_ENABLE_OCR_CORRECTION_FROM_SUBTITLE)
translate_parser.add_argument(
    '--export_target_locale_advanced_subtitle_file', required=False, type=bool,
    help=ARGUMENT_HELP_EXPORT_TARGET_LOCALE_ADVANCED_SUBTITLE_FILE)
translate_parser.add_argument(
    '--subtitle_primary_color', required=False, type=str,
    help=ARGUMENT_HELP_SUBTITLE_PRIMARY_COLOR)
translate_parser.add_argument(
    '--subtitle_outline_color', required=False, type=str,
    help=ARGUMENT_HELP_SUBTITLE_OUTLINE_COLOR)
translate_parser.add_argument(
    '--subtitle_font_size', required=False, type=int, help=ARGUMENT_HELP_SUBTITLE_FONT_SIZE)
translate_parser.set_defaults(func=handle_request_create_iteration_api)

translate_parser = sub_parsers.add_parser('request_get_iteration_api', help='Request get iteration API.')
translate_parser.add_argument('--translation_id', required=True, type=str, help='Translation ID.')
translate_parser.add_argument('--iteration_id', required=True, type=str, help='Iteration ID.')
translate_parser.set_defaults(func=handle_request_get_iteration_api)

args = root_parser.parse_args()
args.func(args)

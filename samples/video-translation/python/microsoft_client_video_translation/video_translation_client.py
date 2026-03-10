# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import uuid
import locale
import json
import dataclasses
from termcolor import colored
from datetime import datetime
from urllib3.util import Url
from microsoft_client_video_translation.video_translation_enum import (
    VoiceKind, WebvttFileKind, EnableEmotionalPlatformVoice
)
from microsoft_client_video_translation.video_translation_dataclass import (
    WebvttFileDefinition, TranslationInputDefinition,
    IterationInputDefinition, IterationDefinition, TranslationDefinition,
    PagedTranslationDefinition, PagedIterationDefinition
)
from microsoft_speech_client_common.client_common_util import (
    dict_to_dataclass
)
from microsoft_speech_client_common.client_common_enum import (
    OperationStatus
)
from microsoft_speech_client_common.client_common_client_base import (
    SpeechLongRunningTaskClientBase
)


class VideoTranslationClient(SpeechLongRunningTaskClientBase):
    URL_SEGMENT_NAME_TRANSLATIONS = "translations"
    URL_SEGMENT_NAME_ITERATIONS = "iterations"
    URL_PATH_ROOT = "videotranslation"

    def __init__(self, region, sub_key, api_version):
        super().__init__(
            region=region,
            sub_key=sub_key,
            api_version=api_version,
            service_url_segment_name=self.URL_PATH_ROOT,
            long_running_tasks_url_segment_name=self.URL_SEGMENT_NAME_TRANSLATIONS
        )

    # For most common scenario, customer not need provide webvtt first iteration.
    # Even, it is supported to provide webvtt for the first iteration, customer can
    # customize the client code if they want to run first iteration with webvtt.
    def create_translate_and_run_first_iteration_until_terminated(
        self,
        video_file_url: Url,
        audio_file_url: Url,
        source_locale: locale,
        target_locale: locale,
        voice_kind: VoiceKind,
        webvtt_file_kind: WebvttFileKind = None,
        webvtt_file_url: Url = None,
        enable_emotional_platform_voice: EnableEmotionalPlatformVoice = None,
        enable_lip_sync: bool = None,
        speaker_count: int = None,
        subtitle_max_char_count_per_segment: int = None,
        export_subtitle_in_video: bool = None,
        tts_custom_lexicon_file_url: Url = None,
        tts_custom_lexicon_file_id_in_audio_content_creation: str = None,
        enable_video_speed_adjustment: bool = None,
        enable_ocr_correction_from_subtitle: bool = None,
        export_target_locale_advanced_subtitle_file: bool = None,
        subtitle_primary_color: str = None,
        subtitle_outline_color: str = None,
        subtitle_font_size: int = None,
        subtitle_vertical_margin: int = None,
        adjust_webvtt_alignment: bool = None,
        use24k_prompt_audio: bool = None,
        export_adjusted_prompt_audio_in_webvtt: bool = None,
        adjust_background_volume_multiplier: float = None,
        push_result_to_azure_storage_blob_dir_url: Url = None,
        push_result_to_azure_storage_blob_managed_identity_client_id: str = None,
        input_file_azure_storage_blob_managed_identity_client_id: str = None,
        input_file_source_kind: str = None
    ) -> tuple[bool, str, TranslationDefinition]:
        if (video_file_url is None and audio_file_url is None) or target_locale is None or voice_kind is None:
            raise ValueError

        now = datetime.now()
        nowString = now.strftime("%m%d%Y%H%M%S")
        translation_id = f"{nowString}_{source_locale}_{target_locale}_{voice_kind}"

        # Create translation
        operation_id = str(uuid.uuid4())
        create_translation_request_body = self.create_translation_creation_request_body(
            video_file_url=video_file_url,
            audio_file_url=audio_file_url,
            source_locale=source_locale,
            target_locale=target_locale,
            voice_kind=voice_kind,
            enable_lip_sync=enable_lip_sync,
            speaker_count=speaker_count,
            subtitle_max_char_count_per_segment=subtitle_max_char_count_per_segment,
            export_subtitle_in_video=export_subtitle_in_video,
            input_file_azure_storage_blob_managed_identity_client_id=input_file_azure_storage_blob_managed_identity_client_id,
            input_file_source_kind=input_file_source_kind,
            translation_display_name=None,
            translation_description=None
        )

        create_translation_request_body.input.autoCreateFirstIteration = True
        create_translation_request_body.firstIterationInput = self.create_iteration_creation_input_request_body(
            webvtt_file_kind=webvtt_file_kind,
            webvtt_file_url=webvtt_file_url,
            speaker_count=speaker_count,
            enable_emotional_platform_voice=enable_emotional_platform_voice,
            subtitle_max_char_count_per_segment=subtitle_max_char_count_per_segment,
            export_subtitle_in_video=export_subtitle_in_video,
            tts_custom_lexicon_file_url=tts_custom_lexicon_file_url,
            tts_custom_lexicon_file_id_in_audio_content_creation=tts_custom_lexicon_file_id_in_audio_content_creation,
            enable_video_speed_adjustment=enable_video_speed_adjustment,
            enable_ocr_correction_from_subtitle=enable_ocr_correction_from_subtitle,
            export_target_locale_advanced_subtitle_file=export_target_locale_advanced_subtitle_file,
            subtitle_primary_color=subtitle_primary_color,
            subtitle_outline_color=subtitle_outline_color,
            subtitle_font_size=subtitle_font_size,
            subtitle_vertical_margin=subtitle_vertical_margin,
            adjust_webvtt_alignment=adjust_webvtt_alignment,
            use24k_prompt_audio=use24k_prompt_audio,
            export_adjusted_prompt_audio_in_webvtt=export_adjusted_prompt_audio_in_webvtt,
            adjust_background_volume_multiplier=adjust_background_volume_multiplier,
            push_result_to_azure_storage_blob_dir_url=push_result_to_azure_storage_blob_dir_url,
            push_result_to_azure_storage_blob_managed_identity_client_id=push_result_to_azure_storage_blob_managed_identity_client_id,
        )

        success, error, response_translation, operation_location = self.request_create_translation(
            translation_id=translation_id,
            translation_create_body=create_translation_request_body,
            operation_id=operation_id)
        if not success or operation_location is None:
            print(colored(f"Failed to create translation with ID {translation_id} with error: {error}", 'red'))
            return False, error, None

        self.request_operation_until_terminated(operation_location)

        success, error, translation = self.request_get_translation(translation_id)
        if not success:
            print(colored(f"Failed to query translation {translation_id} with error: {error}", 'red'))
            return False, error, None
        if translation.status != OperationStatus.Succeeded:
            print(colored(f"Translation creation failed with error: {error}", 'red'))
            print(json.dumps(dataclasses.asdict(translation), indent=2))
            return False, translation.translationFailureReason, None

        print(colored("succesfully created translation with first iteration:", 'green'))
        json_formatted_str = json.dumps(dataclasses.asdict(translation), indent=2)
        print(json_formatted_str)

        return True, None, translation

    # For iteration from secondary, webvtt file is required.
    def run_iteration_with_webvtt_until_terminated(
        self,
        translation_id: str,
        webvtt_file_kind: WebvttFileKind,
        webvtt_file_url: Url,
        speaker_count: int = None,
        enable_emotional_platform_voice: EnableEmotionalPlatformVoice = None,
        subtitle_max_char_count_per_segment: int = None,
        export_subtitle_in_video: bool = None,
        tts_custom_lexicon_file_url: Url = None,
        tts_custom_lexicon_file_id_in_audio_content_creation: str = None,
        enable_video_speed_adjustment: bool = None,
        enable_ocr_correction_from_subtitle: bool = None,
        export_target_locale_advanced_subtitle_file: bool = None,
        subtitle_primary_color: str = None,
        subtitle_outline_color: str = None,
        subtitle_font_size: int = None,
        subtitle_vertical_margin: int = None,
        adjust_webvtt_alignment: bool = None,
        use24k_prompt_audio: bool = None,
        export_adjusted_prompt_audio_in_webvtt: bool = None,
        adjust_background_volume_multiplier: float = None,
        push_result_to_azure_storage_blob_dir_url: Url = None,
        push_result_to_azure_storage_blob_managed_identity_client_id: str = None
    ) -> tuple[bool, str, TranslationDefinition, IterationDefinition]:
        if webvtt_file_kind is None or webvtt_file_url is None:
            raise ValueError

        success, error, translation = self.request_get_translation(
            translation_id=translation_id,
        )
        if not success:
            return False, error, None, None
        elif translation is None:
            return False, f"Not found translation ID: {translation_id}", None, None

        now = datetime.now()
        iteration_id = now.strftime("%m%d%Y%H%M%S")

        success, error, iteration = self.create_iteration_until_terminated(
            translation_id=translation_id,
            iteration_id=iteration_id,
            webvtt_file_kind=webvtt_file_kind,
            webvtt_file_url=webvtt_file_url,
            speaker_count=speaker_count,
            enable_emotional_platform_voice=enable_emotional_platform_voice,
            subtitle_max_char_count_per_segment=subtitle_max_char_count_per_segment,
            export_subtitle_in_video=export_subtitle_in_video,
            tts_custom_lexicon_file_url=tts_custom_lexicon_file_url,
            tts_custom_lexicon_file_id_in_audio_content_creation=tts_custom_lexicon_file_id_in_audio_content_creation,
            enable_video_speed_adjustment=enable_video_speed_adjustment,
            enable_ocr_correction_from_subtitle=enable_ocr_correction_from_subtitle,
            export_target_locale_advanced_subtitle_file=export_target_locale_advanced_subtitle_file,
            subtitle_primary_color=subtitle_primary_color,
            subtitle_outline_color=subtitle_outline_color,
            subtitle_font_size=subtitle_font_size,
            subtitle_vertical_margin=subtitle_vertical_margin,
            adjust_webvtt_alignment=adjust_webvtt_alignment,
            use24k_prompt_audio=use24k_prompt_audio,
            export_adjusted_prompt_audio_in_webvtt=export_adjusted_prompt_audio_in_webvtt,
            adjust_background_volume_multiplier=adjust_background_volume_multiplier,
            push_result_to_azure_storage_blob_dir_url=push_result_to_azure_storage_blob_dir_url,
            push_result_to_azure_storage_blob_managed_identity_client_id=push_result_to_azure_storage_blob_managed_identity_client_id
        )
        if not success:
            return False, error, None, None

        return True, None, translation, iteration

    def create_translation_until_terminated(
        self,
        translation_id: str,
        video_file_url: Url,
        audio_file_url: Url,
        source_locale: locale,
        target_locale: locale,
        voice_kind: VoiceKind,
        enable_lip_sync: bool,
        speaker_count: int = None,
        subtitle_max_char_count_per_segment: int = None,
        export_subtitle_in_video: bool = None,
        input_file_azure_storage_blob_managed_identity_client_id: str = None,
        input_file_source_kind: str = None,
    ) -> tuple[bool, str, TranslationDefinition]:
        operation_id = str(uuid.uuid4())
        request_body = self.create_translation_creation_request_body(
            video_file_url=video_file_url,
            audio_file_url=audio_file_url,
            source_locale=source_locale,
            target_locale=target_locale,
            voice_kind=voice_kind,
            enable_lip_sync=enable_lip_sync,
            speaker_count=speaker_count,
            subtitle_max_char_count_per_segment=subtitle_max_char_count_per_segment,
            export_subtitle_in_video=export_subtitle_in_video,
            input_file_azure_storage_blob_managed_identity_client_id=input_file_azure_storage_blob_managed_identity_client_id,
            input_file_source_kind=input_file_source_kind,
            translation_display_name=None,
            translation_description=None
        )
        success, error, response_translation, operation_location = self.request_create_translation(
            translation_id=translation_id,
            translation_create_body=request_body,
            operation_id=operation_id)
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
            print(json.dumps(dataclasses.asdict(response_translation), indent=2))
            return False, response_translation.translationFailureReason, None

        return True, None, response_translation

    def create_iteration_until_terminated(
        self,
        translation_id: str,
        iteration_id: str,
        webvtt_file_kind: WebvttFileKind = None,
        webvtt_file_url: Url = None,
        speaker_count: int = None,
        enable_emotional_platform_voice: EnableEmotionalPlatformVoice = None,
        subtitle_max_char_count_per_segment: int = None,
        export_subtitle_in_video: bool = None,
        tts_custom_lexicon_file_url: Url = None,
        tts_custom_lexicon_file_id_in_audio_content_creation: str = None,
        enable_video_speed_adjustment: bool = None,
        enable_ocr_correction_from_subtitle: bool = None,
        export_target_locale_advanced_subtitle_file: bool = None,
        subtitle_primary_color: str = None,
        subtitle_outline_color: str = None,
        subtitle_font_size: int = None,
        subtitle_vertical_margin: int = None,
        adjust_webvtt_alignment: bool = None,
        use24k_prompt_audio: bool = None,
        export_adjusted_prompt_audio_in_webvtt: bool = None,
        adjust_background_volume_multiplier: float = None,
        push_result_to_azure_storage_blob_dir_url: Url = None,
        push_result_to_azure_storage_blob_managed_identity_client_id: str = None
    ) -> tuple[bool, str, IterationDefinition]:
        if translation_id is None or iteration_id is None:
            raise ValueError
        success, error, response_iteration, operation_location = self.request_create_iteration(
            translation_id=translation_id,
            iteration_id=iteration_id,
            webvtt_file_kind=webvtt_file_kind,
            webvtt_file_url=webvtt_file_url,
            speaker_count=speaker_count,
            enable_emotional_platform_voice=enable_emotional_platform_voice,
            subtitle_max_char_count_per_segment=subtitle_max_char_count_per_segment,
            export_subtitle_in_video=export_subtitle_in_video,
            iteration_description=None,
            operation_id=None,
            tts_custom_lexicon_file_url=tts_custom_lexicon_file_url,
            tts_custom_lexicon_file_id_in_audio_content_creation=tts_custom_lexicon_file_id_in_audio_content_creation,
            enable_video_speed_adjustment=enable_video_speed_adjustment,
            enable_ocr_correction_from_subtitle=enable_ocr_correction_from_subtitle,
            export_target_locale_advanced_subtitle_file=export_target_locale_advanced_subtitle_file,
            subtitle_primary_color=subtitle_primary_color,
            subtitle_outline_color=subtitle_outline_color,
            subtitle_font_size=subtitle_font_size,
            subtitle_vertical_margin=subtitle_vertical_margin,
            adjust_webvtt_alignment=adjust_webvtt_alignment,
            use24k_prompt_audio=use24k_prompt_audio,
            export_adjusted_prompt_audio_in_webvtt=export_adjusted_prompt_audio_in_webvtt,
            adjust_background_volume_multiplier=adjust_background_volume_multiplier,
            push_result_to_azure_storage_blob_dir_url=push_result_to_azure_storage_blob_dir_url,
            push_result_to_azure_storage_blob_managed_identity_client_id=push_result_to_azure_storage_blob_managed_identity_client_id)
        if not success:
            print(colored(f"Failed to create iteration with ID {iteration_id} for translation {translation_id} with error: {error}",
                          'red'))
            return False, error, None

        self.request_operation_until_terminated(operation_location)

        success, error, response_iteration = self.request_get_iteration(translation_id, iteration_id)
        if not success:
            print(colored(f"Failed to query iteration {iteration_id} for translation {translation_id} with error: {error}", 'red'))
            return False, error, None
        if response_iteration.status != OperationStatus.Succeeded:
            print(colored(f"Iteration creation failed with error: {error}", 'red'))
            print(json.dumps(dataclasses.asdict(operation_location), indent=2))
            return False, response_iteration.translationFailureReason, None

        return True, None, response_iteration

    def build_iterations_path(self,
                              translation_id: str) -> str:
        if translation_id is None:
            raise ValueError
        translation_path = self.build_long_running_task_path(translation_id)
        return f"{translation_path}/{self.URL_SEGMENT_NAME_ITERATIONS}"

    def build_iteration_path(self,
                             translation_id: str,
                             iteration_id: str) -> str:
        if translation_id is None or iteration_id is None:
            raise ValueError
        iterations_path = self.build_iterations_path(translation_id)
        return f"{iterations_path}/{iteration_id}"

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

    # https://learn.microsoft.com/rest/api/aiservices/videotranslation/translation-operations/get-translation
    def request_get_translation(self,
                                translation_id: str) -> tuple[bool, str, TranslationDefinition]:
        success, error, response = self.request_get_long_running_task(translation_id)
        if not success or response is None:
            return success, error, response
        response_translation_json = response.json()
        response_translation = dict_to_dataclass(
            data=response_translation_json,
            dataclass_type=TranslationDefinition)
        return True, None, response_translation

    # https://learn.microsoft.com/rest/api/aiservices/videotranslation/iteration-operations/get-iteration
    def request_get_iteration(self,
                              translation_id: str,
                              iteration_id: str) -> tuple[bool, str, IterationDefinition]:
        if translation_id is None or iteration_id is None:
            raise ValueError

        url = self.build_iteration_url(translation_id, iteration_id)
        (success, error, response) = self.request_get_with_url(url)
        if not success or response is None:
            return success, error, response

        response_iteration_json = response.json()
        response_iteration = dict_to_dataclass(
            data=response_iteration_json,
            dataclass_type=IterationDefinition)
        return True, None, response_iteration

    # https://learn.microsoft.com/rest/api/aiservices/videotranslation/translation-operations/list-translation
    def request_list_translations(self,
                                  top: int = None,
                                  skip: int = None,
                                  maxPageSize: int = None) -> tuple[bool, str, PagedTranslationDefinition]:

        success, error, response = self.request_list_long_running_tasks(
            top=top,
            skip=skip,
            maxPageSize=maxPageSize)
        if not success:
            return False, error, None

        response_translations_json = response.json()
        response_translations = dict_to_dataclass(
            data=response_translations_json,
            dataclass_type=PagedTranslationDefinition)
        return True, None, response_translations

    # https://learn.microsoft.com/rest/api/aiservices/videotranslation/iteration-operations/list-iteration
    def request_list_iterations(self) -> tuple[bool, str, PagedIterationDefinition]:
        url = self.build_iterations_url()
        success, error, response = self.request_list_with_url(url)
        if not success:
            return False, error, None

        response_iterations_json = response.json()
        response_iterations = dict_to_dataclass(
            data=response_iterations_json,
            dataclass_type=PagedIterationDefinition)
        return True, None, response_iterations

    # https://learn.microsoft.com/rest/api/aiservices/videotranslation/translation-operations/delete-translation
    def request_delete_translation(self,
                                   translation_id: str) -> tuple[bool, str]:
        return self.request_delete_long_running_task(translation_id)

    # https://learn.microsoft.com/rest/api/aiservices/videotranslation/translation-operations/create-translation
    def create_translation_creation_request_body(
            self,
            video_file_url: str,
            audio_file_url: str,
            source_locale: locale,
            target_locale: locale,
            voice_kind: VoiceKind,
            enable_lip_sync: bool = None,
            speaker_count: int = None,
            subtitle_max_char_count_per_segment: int = None,
            export_subtitle_in_video: bool = None,
            input_file_azure_storage_blob_managed_identity_client_id: str = None,
            input_file_source_kind: str = None,
            translation_display_name: str = None,
            translation_description: str = None,
            ) -> TranslationDefinition:
        if (video_file_url is None and audio_file_url is None) or target_locale is None or voice_kind is None:
            raise ValueError
        translation_create_input_body = TranslationInputDefinition(
            sourceLocale=source_locale,
            targetLocale=target_locale,
            voiceKind=voice_kind,
            enableLipSync=enable_lip_sync,
            videoFileUrl=video_file_url,
            audioFileUrl=audio_file_url,
            speakerCount=speaker_count,
            subtitleMaxCharCountPerSegment=subtitle_max_char_count_per_segment,
            exportSubtitleInVideo=export_subtitle_in_video,
            inputFileAzureStorageBlobManagedIdentityClientId=input_file_azure_storage_blob_managed_identity_client_id,
            inputFileSourceKind=input_file_source_kind,
        )
        translation_create_body = TranslationDefinition(
            input=translation_create_input_body,
            displayName=translation_display_name,
            description=translation_description,
        )
        return translation_create_body

    # https://learn.microsoft.com/rest/api/aiservices/videotranslation/iteration-operations/create-iteration
    def create_iteration_creation_input_request_body(
            self,
            webvtt_file_kind: WebvttFileKind = None,
            webvtt_file_url: Url = None,
            speaker_count: int = None,
            enable_emotional_platform_voice: EnableEmotionalPlatformVoice = None,
            subtitle_max_char_count_per_segment: int = None,
            export_subtitle_in_video: bool = None,
            tts_custom_lexicon_file_url: Url = None,
            tts_custom_lexicon_file_id_in_audio_content_creation: str = None,
            enable_video_speed_adjustment: bool = None,
            enable_ocr_correction_from_subtitle: bool = None,
            export_target_locale_advanced_subtitle_file: bool = None,
            subtitle_primary_color: str = None,
            subtitle_outline_color: str = None,
            subtitle_font_size: int = None,
            subtitle_vertical_margin: int = None,
            adjust_webvtt_alignment: bool = None,
            use24k_prompt_audio: bool = None,
            export_adjusted_prompt_audio_in_webvtt: bool = None,
            adjust_background_volume_multiplier: float = None,
            push_result_to_azure_storage_blob_dir_url: Url = None,
            push_result_to_azure_storage_blob_managed_identity_client_id: str = None,
            iteration_description: str = None,
            ) -> IterationDefinition:
        iteration_create_input = IterationInputDefinition()

        if speaker_count is not None:
            iteration_create_input.speakerCount = speaker_count

        if subtitle_max_char_count_per_segment is not None:
            iteration_create_input.subtitleMaxCharCountPerSegment = subtitle_max_char_count_per_segment

        if export_subtitle_in_video is not None:
            iteration_create_input.exportSubtitleInVideo = export_subtitle_in_video

        if tts_custom_lexicon_file_url is not None:
            iteration_create_input.ttsCustomLexiconFileUrl = tts_custom_lexicon_file_url

        if tts_custom_lexicon_file_id_in_audio_content_creation is not None:
            iteration_create_input.ttsCustomLexiconFileIdInAudioContentCreation = tts_custom_lexicon_file_id_in_audio_content_creation

        if enable_video_speed_adjustment is not None:
            iteration_create_input.enableVideoSpeedAdjustment = enable_video_speed_adjustment

        if enable_ocr_correction_from_subtitle is not None:
            iteration_create_input.enableOcrCorrectionFromSubtitle = enable_ocr_correction_from_subtitle

        if export_target_locale_advanced_subtitle_file is not None:
            iteration_create_input.exportTargetLocaleAdvancedSubtitleFile = export_target_locale_advanced_subtitle_file

        if subtitle_primary_color is not None:
            iteration_create_input.subtitlePrimaryColor = subtitle_primary_color

        if subtitle_outline_color is not None:
            iteration_create_input.subtitleOutlineColor = subtitle_outline_color

        if subtitle_font_size is not None:
            iteration_create_input.subtitleFontSize = subtitle_font_size

        if subtitle_vertical_margin is not None:
            iteration_create_input.subtitleVerticalMargin = subtitle_vertical_margin

        if adjust_background_volume_multiplier is not None:
            iteration_create_input.adjustBackgroundVolumeMultiplier = adjust_background_volume_multiplier

        if push_result_to_azure_storage_blob_dir_url is not None:
            iteration_create_input.pushResultToAzureStorageBlobDirUrl = (
                push_result_to_azure_storage_blob_dir_url)

        if push_result_to_azure_storage_blob_managed_identity_client_id is not None:
            iteration_create_input.pushResultToAzureStorageBlobManagedIdentityClientId = (
                push_result_to_azure_storage_blob_managed_identity_client_id)

        if webvtt_file_kind is not None and webvtt_file_url is not None:
            iteration_create_input.webvttFile = WebvttFileDefinition(
                kind=webvtt_file_kind,
                url=webvtt_file_url,
            )

        # This parameter is only needed when webvtt file is provided.
        if adjust_webvtt_alignment is not None and iteration_create_input.webvttFile is not None:
            iteration_create_input.adjustWebvttAlignment = adjust_webvtt_alignment

        if use24k_prompt_audio is not None:
            iteration_create_input.use24kPromptAudio = use24k_prompt_audio

        if export_adjusted_prompt_audio_in_webvtt is not None:
            iteration_create_input.exportAdjustedPromptAudioInWebvtt = export_adjusted_prompt_audio_in_webvtt

        if enable_emotional_platform_voice is not None:
            iteration_create_input.enableEmotionalPlatformVoice = enable_emotional_platform_voice

        return iteration_create_input

    def request_create_translation(
            self,
            translation_id: str,
            translation_create_body: TranslationDefinition,
            operation_id: str = None
            ) -> tuple[bool, str, TranslationDefinition, Url]:
        if translation_id is None or translation_create_body is None:
            raise ValueError
        success, error, response, operation_location_url = self.request_create_long_running_task_with_id(
            id=translation_id,
            creation_body=translation_create_body,
            operation_id=operation_id)

        if not success:
            return False, error, None, None

        response_translation_json = response.json()
        response_translation = dict_to_dataclass(
            data=response_translation_json,
            dataclass_type=TranslationDefinition)
        return True, None, response_translation, operation_location_url

    # https://learn.microsoft.com/rest/api/aiservices/videotranslation/iteration-operations/create-iteration
    def request_create_iteration(
            self,
            translation_id: str,
            iteration_id: str,
            webvtt_file_kind: WebvttFileKind = None,
            webvtt_file_url: Url = None,
            speaker_count: int = None,
            enable_emotional_platform_voice: EnableEmotionalPlatformVoice = None,
            subtitle_max_char_count_per_segment: int = None,
            export_subtitle_in_video: bool = None,
            iteration_description: str = None,
            operation_id: str = None,
            tts_custom_lexicon_file_url: Url = None,
            tts_custom_lexicon_file_id_in_audio_content_creation: str = None,
            enable_video_speed_adjustment: bool = None,
            enable_ocr_correction_from_subtitle: bool = None,
            export_target_locale_advanced_subtitle_file: bool = None,
            subtitle_primary_color: str = None,
            subtitle_outline_color: str = None,
            subtitle_font_size: int = None,
            subtitle_vertical_margin: int = None,
            adjust_webvtt_alignment: bool = None,
            use24k_prompt_audio: bool = None,
            export_adjusted_prompt_audio_in_webvtt: bool = None,
            adjust_background_volume_multiplier: float = None,
            push_result_to_azure_storage_blob_dir_url: Url = None,
            push_result_to_azure_storage_blob_managed_identity_client_id: str = None
            ) -> tuple[bool, str, IterationDefinition, Url]:
        if translation_id is None or iteration_id is None:
            raise ValueError

        iteration_create_body = IterationDefinition(
            description=iteration_description,
        )

        iteration_create_body.input = self.create_iteration_creation_input_request_body(
            webvtt_file_kind=webvtt_file_kind,
            webvtt_file_url=webvtt_file_url,
            speaker_count=speaker_count,
            enable_emotional_platform_voice=enable_emotional_platform_voice,
            subtitle_max_char_count_per_segment=subtitle_max_char_count_per_segment,
            export_subtitle_in_video=export_subtitle_in_video,
            tts_custom_lexicon_file_url=tts_custom_lexicon_file_url,
            tts_custom_lexicon_file_id_in_audio_content_creation=tts_custom_lexicon_file_id_in_audio_content_creation,
            enable_video_speed_adjustment=enable_video_speed_adjustment,
            enable_ocr_correction_from_subtitle=enable_ocr_correction_from_subtitle,
            export_target_locale_advanced_subtitle_file=export_target_locale_advanced_subtitle_file,
            subtitle_primary_color=subtitle_primary_color,
            subtitle_outline_color=subtitle_outline_color,
            subtitle_font_size=subtitle_font_size,
            subtitle_vertical_margin=subtitle_vertical_margin,
            adjust_webvtt_alignment=adjust_webvtt_alignment,
            use24k_prompt_audio=use24k_prompt_audio,
            export_adjusted_prompt_audio_in_webvtt=export_adjusted_prompt_audio_in_webvtt,
            adjust_background_volume_multiplier=adjust_background_volume_multiplier,
            push_result_to_azure_storage_blob_dir_url=push_result_to_azure_storage_blob_dir_url,
            push_result_to_azure_storage_blob_managed_identity_client_id=push_result_to_azure_storage_blob_managed_identity_client_id,
            iteration_description=iteration_description,
        )

        url = self.build_iteration_url(translation_id, iteration_id)
        success, error, response, operation_location_url = self.request_create_long_running_task_with_url(
            url=url,
            creation_body=iteration_create_body,
            operation_id=operation_id
        )

        if not success:
            return False, error, None, None
        response_iteration_json = response.json()
        response_iteration = dict_to_dataclass(
            data=response_iteration_json,
            dataclass_type=IterationDefinition)
        return True, None, response_iteration, operation_location_url

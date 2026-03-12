
# Video translation client sample code for python

# Prerequisite
## Tested OS:
    Ubuntu 24.04.1 LTS
    Windows 11 Enterprise
## Python version:
    3.11.10
## Dependency modules:
    pip3 install termcolor
    pip3 install orjson
    pip3 install urllib3
    pip3 install requests
    pip3 install pydantic

# Platform dependency:
## VS Code
### Create environment with command, Ctrl+Shift+P:
    Python: Create Environment
    Python: Select Interpreter
    Python: 3.11.10
    
### Debug 
    Copy .\.vscode\launch_sample_video_translation.json file to .\.vscode\launch.json
    And replace the placeholder with actual values like: sub_key, source_locale, target_locale, translation_id, video_file_blob_url etc.

# Conda support:
    conda create -n VideoTranslation_ClientSampleCode python=3.11.10
    conda activate VideoTranslation_ClientSampleCode

# File Description
| Files | Description |
| --- | --- |
| [main_video_translation.py](main_video_translation.py)  | client tool main definition |
| [video_translation_client.py](microsoft_client_video_translation/video_translation_client.py)  | video translation client definition  |
| [video_translation_dataclass.py](microsoft_client_video_translation/video_translation_dataclass.py)  | video translation data contract definition  |
| [video_translation_enum.py](microsoft_client_video_translation/video_translation_enum.py)  | video translation enum definition  |
| [video_translation_const.py](microsoft_client_video_translation/video_translation_const.py)  | video translation constant definition  |

# Usage for command line tool:
## Usage
Run main_video_translation.py with command in below pattern:
    python main_video_translation.py --api-version 2024-05-20-preview --region eastus --sub_key [YourSpeechresourceKey] [SubCommands] [args...]
## Supported API version
| API version | Description |
| --- | --- |
| 2026-03-01 | This is latest API version |
| 2025-05-20 | This is public GA version |
| 2024-05-20-preview | Released public preview version |

## Global parameters
| Argument name | Description | 
| --- | --- |
| region | region of the speech resource |
| sub-key | speech resource key |
| api-version | API version, supported version: 2024-05-20-preview |

## Sub commands definition
| SubCommand | Description |
| --- | --- |
| create_translation_and_iteration_and_wait_until_terminated  | Create translation and run first iteration for the video file from source locale to target locale, and wait until iteration terminated |
| create_iteration_with_webvtt_and_wait_until_terminated  | Run iteration on an existing translation with webvtt, and wait until iteration terminated |
| request_create_translation_api  | Request create translation API |
| request_get_operation_api  | Request get operation by ID API |
| request_get_translation_api  | Request get translation by ID API |
| request_list_translations_api  | Request list translations API |
| request_delete_translation_api  | Request delete translation API |
| request_create_iteration_api  | Request create iteration API |
| request_get_iteration_api  | Request get iteration API |

## HTTP client library
Video translation client is defined as class VideoTranslationClient in file [video_translation_client.py](microsoft_video_translation_client/video_translation_client.py)
### Function definitions:
| Function | Description |
| --- | --- |
| create_translate_and_run_first_iteration_until_terminated | Create translation and run first iteration for the video file from source locale to target locale, and wait until iteration terminated |
| run_iteration_with_webvtt_until_terminated    | Run iteration on an existing translation with webvtt, and wait until iteration terminated |
| create_translation_until_terminated  | Create translation and wait until terminated |
| create_iteration_until_terminated  | Create iteration and wait until terminated |
| request_operation_until_terminated  | Query operation and wait until terminated |
| request_create_translation  | Request create translation PUT API |
| request_get_operation  | Request query operation GET API |
| request_get_translation  | Query get translation GET API |
| request_delete_translation  | Delete translation DELETE API |
| request_create_iteration  | Request create iteration PUT API |
| request_list_translations  | Query list translations LIST API |
| request_get_iteration  | Query get iteration GET API |
| request_list_iterations  | Query list iterations LIST API |

# Usage sample for client class:
```
    client = VideoTranslationClient(
        region = "eastus",
        sub_key = "[YourSpeechresourceKey]",
    )
    success, error, translation, iteration = client.create_translate_and_run_first_iteration_until_terminated(
        video_file_url = "https://xx.blob.core.windows.net/users/xx/xx.mp4?sv=xx",
        source_locale = "zh-CN",
        target_locale = "en-US",
        voice_kind = "PlatformVoice",
        speaker_count = "2",
        subtitle_max_char_count_per_segment = "30",
        export_subtitle_in_video = True,
    )
    if not success:
        return
    print(colored("success", 'green'))
```
Reference function handleCreateTranslationAndIterationAndWaitUntilTerminated in [main.py](main.py)


# Arguments usage

| Argument | Description |
| --- | --- |
| --video_file_blob_url | Translation video file Azure blob url, .mp4 file format, maximum 5GB file size and 4 hours video duration. Provide the input media file using either videoFileUrl or audioFileUrl, these parameters are mutually exclusive—only one of them is required. If both are provided, the request will be considered invalid. |
| --audio_file_blob_url | Translation audio file Azure blob url, .mp3 or .wav file format, maximum 5GB file size and 4 hours video duration. Provide the input media file using either videoFileUrl or audioFileUrl, these parameters are mutually exclusive—only one of them is required. If both are provided, the request will be considered invalid. |
| --source_locale | The source locale of the video file. Locale code follows BCP-47. You can find the text to speech locale list at https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts. If not specified, the source locale will be auto-detected from the video file. |
| --target_locale | The target locale of the translation. Locale code follows BCP-47. You can find the text to speech locale list at https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts. |
| --voice_kind | Translation voice kind. Valid values: PlatformVoice, PersonalVoice. |
| --enable_lip_sync | Indicate whether to enable lip sync. If not provided, the default value is false to disable the lip sync. |
| --enable_emotional_platform_voice | This parameter specifies whether to enable emotion for platform voice. By default, the server determines whether to apply emotion based on the target locale to optimize quality. If not specified, the API will automatically decide whether to enable emotional expression on the server side. |
| --speaker_count | Number of speakers in the video. If not provided, it will be auto-detected from the video file. |
| --subtitle_max_char_count_per_segment | Subtitle max display character count per segment. If not provided, it will use the language dependent default value. |
| --export_subtitle_in_video | Export subtitle in video. If not specified, the default value is false, it will not burn subtitle to the translated video file. |
| --tts_custom_lexicon_file_url | Translate with TTS custom lexicon for speech synthesis. Provide the custom lexicon file using either ttsCustomLexiconFileUrl or ttsCustomLexiconFileIdInAudioContentCreation. These parameters are mutually exclusive—only one of them is required. If both are provided, the request will be considered invalid. |
| --tts_custom_lexicon_file_id_in_audio_content_creation | Translate with TTS custom lexicon for speech synthesis. Provide the custom lexicon file using either ttsCustomLexiconFileUrl or ttsCustomLexiconFileIdInAudioContentCreation. These parameters are mutually exclusive—only one of them is required. If both are provided, the request will be considered invalid. |
| --enable_video_speed_adjustment | This parameter allows for the adjustment of video playback speed to ensure better alignment with translated audio. When enabled, the API can slow down or speed up the video to match the timing of the translated audio, providing a more synchronized and seamless viewing experience. If not specified, video speed will not be adjusted. |
| --enable_ocr_correction_from_subtitle | Indicate whether to allow the API to correct the speech recognition (SR) results using the subtitles from the original video file. By leveraging the existing subtitles, the API can enhance the accuracy of the transcribed text, ensuring that the final output is more precise and reliable. If not specified, translation will not do correction from OCR subtitle. |
| --export_target_locale_advanced_subtitle_file | This parameter, when enabled, allows the API to export subtitles in the Advanced SubStation Alpha format. The subtitle file can specify font styles and colors, which helps in addressing character display issues in certain target locales such as Arabic (Ar), Japanese (Ja), Korean (Ko), and Chinese (Ch). By using this parameter, you can ensure that the subtitles are visually appealing and correctly rendered across different languages and regions. If not specified, iteration response will not include advanced subtitle. |
| --subtitle_primary_color | This parameter specifies the primary color of the subtitles in the video translation output. The value should be provided in the format <rr><gg><bb>, #<rr><gg><bb>, <rr><gg><bb><aa> or #<rr><gg><bb><aa>, where <rr> represents the red component of the color, <gg> represents the green component, <bb> represents the blue component, <aa> represents the alpha component (00 means fully opaque, FF means fully transparent). For example, EBA205 or #EBA205 would set the subtitle color to a specific shade of yellow. If not specified, it will use default white color. |
| --subtitle_outline_color | This parameter specifies the outline color of the subtitles in the video translation output. The value should be provided in the format <rr><gg><bb>, #<rr><gg><bb>, <rr><gg><bb><aa> or #<rr><gg><bb><aa>, where <rr> represents the red component of the color, <gg> represents the green component, <bb> represents the blue component, <aa> represents the alpha component (00 means fully opaque, FF means fully transparent). For example, EBA205 or #EBA205 would set the subtitle color to a specific shade of yellow. If not specified, it will use default black color. |
| --subtitle_font_size | This parameter specifies the font size of subtitles in the video translation output between 5 and 30. If not specified, it will use the language dependent default value. |
| --subtitle_vertical_margin | Specify the vertical margin (in pixels) for subtitle positioning in the video. This allows customization of the space between subtitles and the video edge. |
| --webvtt_file_kind | Translation webvtt file kind. Specifies the type of webvtt file to use for translation. Valid values: SourceLocaleSubtitle, TargetLocaleSubtitle, MetadataJson. Default: MetadataJson. |
| --webvtt_file_blob_url | Translation webvtt file url. Provides the URL to a webvtt file for use in translation or iteration creation. |
| --adjust_webvtt_alignment | Recommended: Do not set this parameter unless you have specific requirements. The default behavior is optimized for most customers. Specify whether to adjust webvtt alignment according to the synthesized audio for the translated text. If true (default), alignment will be adjusted. If false, alignment will not be adjusted. When not specified, the default value is true (provided as null to the API). |
| --use24k_prompt_audio | Recommended: Do not set this parameter unless you have specific requirements. The default behavior is optimized for most customers. When this parameter is set to true, the service uses a higher sample rate (24 kHz) prompt audio for voice prompting. If this parameter is not specified or is set to false, the service continues to use 16 kHz prompt audio by default, which generally provides better overall synthesis quality. |
| --export_adjusted_prompt_audio_in_webvtt | Specify whether to export adjusted prompt audio in webvtt. If not specified, the default value is false. When enabled, the adjusted prompt audio will be included in the webvtt output file. |
| --adjust_background_volume_multiplier | Specify a multiplier to adjust the background audio volume in the translated video. For example, 0.5 reduces the background volume by half, while 2.0 doubles it. |
| --input_file_azure_storage_blob_managed_identity_client_id | Specify the Azure Managed Identity Client ID (GUID) to access the input file from Azure Storage Blob. This allows secure access to storage using managed identity authentication. |
| --input_file_source_kind | Specify the source kind for input files. Currently supported value: AzureStorageBlobManagedIdentity. This indicates how the service should access the input file. |
| --push_result_to_azure_storage_blob_dir_url | Specify the Azure Storage Blob directory URL where the translation results should be pushed. This enables automatic upload of output files to the specified storage location. |
| --push_result_to_azure_storage_blob_managed_identity_client_id | Specify the Azure Managed Identity Client ID (GUID) to push results to Azure Storage Blob. This allows secure write access to storage using managed identity authentication. |
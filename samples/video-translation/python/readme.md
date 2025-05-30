
# Video translation client sample code for python

# Prerepuest
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
    Copy .\.vscode\launch_sample.json file to .\.vscode\launch.json
    And replace the placeholder with actual vaules like: sub_key, source_locale, target_locale, translation_id, video_file_blob_url etc.

# Conda support:
    conda create -n VideoTranslation_ClientSampleCode python=3.11.10
    conda activate VideoTranslation_ClientSampleCode

# File Description
| Files | Description |
| --- | --- |
| [main.py](main.py)  | client tool main definition |
| [video_translation_client.py](microsoft_video_translation_client/video_translation_client.py)  | video translation client definition  |
| [video_translation_dataclass.py](microsoft_video_translation_client/video_translation_dataclass.py)  | video translation data contract definition  |
| [video_translation_enum.py](microsoft_video_translation_client/video_translation_enum.py)  | video translation enum definition  |
| [video_translation_const.py](microsoft_video_translation_client/video_translation_const.py)  | video translation constant definition  |
| [video_translation_util.py](microsoft_video_translation_client/video_translation_util.py)  | video translation utility function definition  |

# Usage for command line tool:
## Usage
Run main.py with command in below pattern:
    python main.py --api-version 2024-05-20-preview --region eastus --sub_key [YourSpeechresourceKey] [SubCommands] [args...]
## Supported API version
| API version | Description |
| --- | --- |
| 2024-05-20-preview | Released public preview version |
| 2025-05-20 | This is public GA version which will be available after 2025-06-15 |

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

Below arguments are only available after API version: 2025-05-20
| Argument | Description |
| --- | --- |
| --source_locale | Support optional for auto detect |
| --audio_file_blob_url | Support translate audio file |  
| --tts_custom_lexicon_file_url | Translate with TTS custom lexicon for TTS synthesis, provide the custom lexicon file with URL, ttsCustomLexiconFileUrl and ttsCustomLexiconFileIdInAudioContentCreation are conflict, only one of them are required. |
|  --tts_custom_lexicon_file_id_in_audio_content_creation | Translate with TTS custom lexicon for TTS synthesis, provide the custom lexicon file with file ID in Audio Content Creation, ttsCustomLexiconFileUrl and ttsCustomLexiconFileIdInAudioContentCreation are conflict, only one of them are required. |
| --enable_video_speed_adjustment | This parameter allows for the adjustment of video playback speed to ensure better alignment with translated audio. When enabled, the API can slow down or speed up the video to match the timing of the translated audio, providing a more synchronized and seamless viewing experience. |
| --enable_ocr_correction_from_subtitle | Indicate whether allow the API to correct the speech recognition (SR) results using the subtitles from the original video file. By leveraging the existing subtitles, the API can enhance the accuracy of the transcribed text, ensuring that the final output is more precise and reliable. |
| --export_target_locale_advanced_subtitle_file | This parameter, when enabled, allows the API to export subtitles in the Advanced SubStation Alpha format. The subtitle file can specify font styles and colors, which helps in addressing character display issues in certain target locales such as Arabic (Ar), Japanese (Ja), Korean (Ko), and Chinese (Ch). By using this parameter, you can ensure that the subtitles are visually appealing and correctly rendered across different languages and regions. |
| --subtitle_primary_color | This parameter specifies the primary color of the subtitles in the video translation output. The value should be provided in the format <rr><gg><bb>, #<rr><gg><bb>, <rr><gg><bb><aa> or #<rr><gg><bb><aa>, where <rr> represents the red component of the color, <gg> represents the green component, <bb> represents the blue component, <aa> represents the alpha component. For example, EBA205 or #EBA205  would set the subtitle color to a specific shade of yellow. This parameter allows for customization of subtitle appearance to enhance readability and visual appeal. |
| --subtitle_outline_color | This parameter specifies the outline color of the subtitles in the video translation output. The value should be provided in the format <rr><gg><bb>, #<rr><gg><bb>, <rr><gg><bb><aa> or #<rr><gg><bb><aa>, where <rr> represents the red component of the color, <gg> represents the green component, <bb> represents the blue component, <aa> represents the alpha component. For example, EBA205 or #EBA205  would set the subtitle color to a specific shade of yellow. This parameter allows for customization of subtitle appearance to enhance readability and visual appeal. |
| --subtitle_font_size | This parameter specifies the font size of subtitles in the video translation output. |

# Video translation client sample code for python

# Dependency for VS code:
## Create environment with command, Ctrl+Shift+P:
    Python: Create Environment
    Python: Select Interpreter
    Python: 3.11.10
## Install below dependencies under this .\python folder in terminal:
    pip3 install orjson
    pip3 install urllib3
    pip3 install requests
    pip3 install termcolor
    pip3 install pydantic


# File Description
| Files | Description |
| --- | --- |
| [main.py](client\\main.py)  | client tool main definition |
| [video_translation_client.py](lib\\microsoft_video_translation_client\video_translation_client.py)  | video translation client definition  |
| [video_translation_dataclass.py](lib\\microsoft_video_translation_client\video_translation_dataclass.py)  | video translation data contract definition  |
| [video_translation_enum.py](lib\\microsoft_video_translation_client\video_translation_enum.py)  | video translation enum definition  |
| [video_translation_const.py](lib\\microsoft_video_translation_client\video_translation_const.py)  | video translation constant definition  |
| [video_translation_util.py](lib\\microsoft_video_translation_client\video_translation_util.py)  | video translation utility function definition  |

# Usage for command line tool:
## Usage
Run main.py with command in below pattern:
    python.exe main.py --region eastus --subKey [YourSpeechresourceKey] [SubCommands] [args...]

## Sub commands definition
| SubCommand | Description |
| --- | --- |
| createTranslationAndIterationAndWaitUntilTerminated  | Create translation and run first iteration for the video file from source locale to target locale, and wait until iteration terminated |
| createIterationWithWebvttAndWaitUntilTerminated  | Run iteration on an existing translation with webvtt, and wait until iteration terminated |
| requestCreateTranslationAPI  | Request create translation API |
| requestGetTranslationAPI  | Request get translation API |
| requestListTranslationsAPI  | Request list translations API |
| requestDeleteTranslationAPI  | Request delete translation API |
| requestCreateIterationAPI  | Request create iteration API |
| requestGetIterationAPI  | Request get iteration API |

## HTTP client library
Video translation client is defined as class VideoTranslationClient in file [video_translation_client.py](lib\\microsoft_video_translation_client\video_translation_client.py)
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
        video_file_url = "https://videotranslationpipeline.blob.core.windows.net/users/poleli/DapengPersonalRecording_zhCN.mp4?sv=xx",
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
Reference function handleCreateTranslationAndIterationAndWaitUntilTerminated in [main.py](client\\main.py)



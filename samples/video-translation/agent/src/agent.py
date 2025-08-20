# Copyright (c) Microsoft. All rights reserved.

import asyncio
import os
import logging
from typing import Annotated, Optional, Union
from dotenv import load_dotenv
from datetime import datetime, timedelta

from azure.identity import AzureCliCredential, get_bearer_token_provider
from azure.identity.aio import DefaultAzureCredential as AsyncDefaultAzureCredential
from azure.storage.blob import generate_blob_sas, BlobSasPermissions
from azure.storage.blob.aio import BlobServiceClient

from semantic_kernel import Kernel
from semantic_kernel.connectors.ai.open_ai import AzureChatCompletion
from semantic_kernel.functions import kernel_function
from semantic_kernel.contents import ChatHistory
from semantic_kernel.agents import ChatCompletionAgent

from video_translation_client import VideoTranslationClient
from video_translation_enum import VoiceKind, WebvttFileKind

logging.basicConfig(
    level=logging.ERROR,  # Change to DEBUG for more verbose output
    format='%(asctime)s [%(levelname)s] %(name)s: %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)
logger = logging.getLogger(__name__)

load_dotenv()


class VideoTranslationPlugin:
    """A plugin that provides access to video translation capabilities."""

    def __init__(self):
        print("Initializing VideoTranslationPlugin...")
        try:
            logger.debug("Attempting to use AzureCliCredential...")
            credential = AzureCliCredential()

            token_provider = get_bearer_token_provider(
                credential,
                "https://cognitiveservices.azure.com/.default"
            )

            self.client = VideoTranslationClient(
                api_version=os.getenv("API_VERSION", "2024-05-20-preview"),
                credential=credential,
                token_provider=token_provider
            )
            logger.debug("Successfully initialized VideoTranslationClient with AzureCliCredential")
        except Exception as e:
            # Fall back to default credential
            logger.warning(f"Failed to initialize with AzureCliCredential: {e}")
            logger.debug("Falling back to default credential...")
            self.client = VideoTranslationClient(
                api_version=os.getenv("API_VERSION", "2024-05-20-preview")
            )
            logger.debug("Successfully initialized VideoTranslationClient with default credential")

    @kernel_function(description="Translates a video file from source language to target language")
    def translate_video(self,
                        video_url: Annotated[str, "The URL of the video file to translate"],
                        source_locale: Annotated[str, "The source language code (e.g., en-US, ja-JP)"],
                        target_locale: Annotated[str, "The target language code (e.g., en-US, ja-JP)"],
                        voice_kind: Annotated[str, "The voice type to use: 'PlatformVoice' or 'PersonalVoice'"] = "PlatformVoice",
                        speaker_count: Annotated[Optional[Union[int, str]], "Number of speakers in the video (optional)"] = None,
                        subtitle_max_char_count: Annotated[Optional[Union[int, str]],
                                                           "Maximum characters per subtitle segment (optional)"] = None,
                        export_subtitle_in_video: Annotated[bool, "Whether to embed subtitles in video (optional)"] = False
                        ) -> Annotated[str, "Returns the status of the translation request"]:
        """Translates a video from source language to target language."""
        try:
            logger.debug(f"Starting video translation: {video_url[:50]}... from {source_locale} to {target_locale}")

            if not video_url or not video_url.strip():
                logger.error("Missing video URL parameter")
                return "Error: Video URL is required. Please provide a valid URL to your video file."

            if not source_locale or not source_locale.strip():
                logger.error("Missing source locale parameter")
                return "Error: Source language code is required (e.g., en-US, ja-JP)."

            if not target_locale or not target_locale.strip():
                logger.error("Missing target locale parameter")
                return "Error: Target language code is required (e.g., en-US, ja-JP)."

            # Validate voice kind
            if voice_kind not in ["PlatformVoice", "PersonalVoice"]:
                logger.error(f"Invalid voice kind: {voice_kind}")
                return "Error: Voice kind must be either 'PlatformVoice' or 'PersonalVoice'."

            voice_kind_enum = VoiceKind.PlatformVoice if voice_kind == "PlatformVoice" else VoiceKind.PersonalVoice

            # Convert string parameters to appropriate types if provided
            speaker_count_int = None
            if speaker_count is not None:
                try:
                    speaker_count_int = int(speaker_count)
                except ValueError:
                    logger.error(f"Invalid speaker count: {speaker_count}. Must be a number.")
                    return "Error: Speaker count must be a valid number."

            subtitle_max_char_count_int = None
            if subtitle_max_char_count is not None:
                try:
                    subtitle_max_char_count_int = int(subtitle_max_char_count)
                except ValueError:
                    logger.error(f"Invalid subtitle max char count: {subtitle_max_char_count}. Must be a number.")
                    return "Error: Subtitle max character count must be a valid number."

            logger.debug(f"Calling translation API with voice_kind={voice_kind}, "
                         f"speaker_count={speaker_count_int}")
            success, error, translation, iteration, translation_info, iteration_info = (
                self.client.create_translate_and_run_first_iteration_until_terminated(
                    video_file_url=video_url,
                    source_locale=source_locale,
                    target_locale=target_locale,
                    voice_kind=voice_kind_enum,
                    speaker_count=speaker_count_int,
                    subtitle_max_char_count_per_segment=subtitle_max_char_count_int,
                    export_subtitle_in_video=export_subtitle_in_video
                )
            )

            if success:
                translation_id = translation.id
                iteration_id = iteration.id

                logger.debug(f"Translation successful! Translation ID: {translation_id}, Iteration ID: {iteration_id}")
                return (f"Translation successful! Translation ID:{translation_id} Result translation:{translation_info} "
                        f"Iteration ID:{iteration_id} Result iteration:{iteration_info}")
            else:
                logger.error(f"Translation request failed: {error}")
                return f"Translation request failed: {error}\nPlease check your video URL, language codes, and try again."
        except Exception as e:
            logger.exception(f"Exception during translation: {str(e)}")
            return f"An error occurred during translation: {str(e)}\nPlease verify your inputs and try again."

    @kernel_function(description="Creates an iteration with WebVTT file for an existing translation")
    def create_iteration_with_webvtt(self,
                                     translation_id: Annotated[str, "The ID of the translation to iterate on"],
                                     webvtt_file_url: Annotated[str, "URL of the WebVTT file"],
                                     webvtt_file_kind: Annotated[str, ("Kind of WebVTT file: 'MetadataJson', "
                                                                       "'SourceLocaleSubtitle', or 'TargetLocaleSubtitle'")],
                                     export_subtitle_in_video: Annotated[bool, "Whether to embed subtitles in video (optional)"] = False
                                     ) -> Annotated[str, "Returns the status of the iteration creation"]:
        """Creates a new iteration with a WebVTT file for an existing translation."""
        try:
            logger.debug(f"Creating iteration with WebVTT for translation ID: {translation_id}")

            if not translation_id or not translation_id.strip():
                logger.error("Missing translation ID parameter")
                return "Error: Translation ID is required. Please provide a valid translation ID."

            if not webvtt_file_url or not webvtt_file_url.strip():
                logger.error("Missing WebVTT file URL parameter")
                return "Error: WebVTT file URL is required. Please provide a valid URL to your WebVTT file."

            if not webvtt_file_kind or not webvtt_file_kind.strip():
                logger.error("Missing WebVTT file kind parameter")
                return "Error: WebVTT file kind is required. Must be 'MetadataJson', 'SourceLocaleSubtitle', or 'TargetLocaleSubtitle'."

            # Convert string webvtt_file_kind to enum
            webvtt_kind_enum = None
            if webvtt_file_kind == "MetadataJson":
                webvtt_kind_enum = WebvttFileKind.MetadataJson
            elif webvtt_file_kind == "SourceLocaleSubtitle":
                webvtt_kind_enum = WebvttFileKind.SourceLocaleSubtitle
            elif webvtt_file_kind == "TargetLocaleSubtitle":
                webvtt_kind_enum = WebvttFileKind.TargetLocaleSubtitle
            else:
                logger.error(f"Invalid WebVTT file kind: {webvtt_file_kind}")
                return (f"Error: Invalid WebVTT file kind: '{webvtt_file_kind}'. "
                        f"Must be 'MetadataJson', 'SourceLocaleSubtitle', or 'TargetLocaleSubtitle'.")

            logger.debug(f"Running iteration with WebVTT kind: {webvtt_file_kind}")
            success, error, translation, iteration = self.client.run_iteration_with_webvtt_until_terminated(
                translation_id=translation_id,
                webvtt_file_kind=webvtt_kind_enum,
                webvtt_file_url=webvtt_file_url,
                export_subtitle_in_video=export_subtitle_in_video
            )

            if success:
                iteration_id = iteration.id
                results = ""

                logger.debug(f"Iteration created successfully: {iteration_id}")

                if iteration.result:
                    if iteration.result.translatedVideoFileUrl:
                        logger.debug(f"Translated video available at: {iteration.result.translatedVideoFileUrl}")
                        results += f"Translated Video: {iteration.result.translatedVideoFileUrl}\n"
                    if iteration.result.sourceLocaleSubtitleWebvttFileUrl:
                        logger.debug(f"Source subtitles available at: {iteration.result.sourceLocaleSubtitleWebvttFileUrl}")
                        results += f"Source Subtitles: {iteration.result.sourceLocaleSubtitleWebvttFileUrl}\n"
                    if iteration.result.targetLocaleSubtitleWebvttFileUrl:
                        logger.debug(f"Target subtitles available at: {iteration.result.targetLocaleSubtitleWebvttFileUrl}")
                        results += f"Target Subtitles: {iteration.result.targetLocaleSubtitleWebvttFileUrl}\n"
                    if iteration.result.metadataJsonWebvttFileUrl:
                        logger.debug(f"Metadata available at: {iteration.result.metadataJsonWebvttFileUrl}")
                        results += f"Metadata: {iteration.result.metadataJsonWebvttFileUrl}\n"

                return (f"Iteration successfully created! Iteration ID: {iteration_id}\n"
                        f"Translation: {translation}\nIteration Results:\n{results}")
            else:
                logger.error(f"Iteration creation failed: {error}")
                return f"Iteration creation failed: {error}\nPlease verify your translation ID and WebVTT file URL."
        except Exception as e:
            logger.exception(f"Exception during iteration creation: {str(e)}")
            return f"An error occurred during iteration creation: {str(e)}\nPlease check your inputs and try again."

    @kernel_function(description="Lists all translations for the user")
    def list_translations(self) -> Annotated[str, "Returns a list of all translations"]:
        """Lists all translations."""
        try:
            logger.debug("Listing all translations")
            success, error, translations = self.client.request_list_translations()

            if success and translations:
                logger.debug(f"Successfully retrieved {len(translations) if isinstance(translations, list) else 'some'} translations")
                return translations
            else:
                logger.warning(f"No translations found or error: {error}")
                return f"Unable to retrieve translation list: {error or 'No translations found'}"
        except Exception as e:
            logger.exception(f"Exception while listing translations: {str(e)}")
            return f"An error occurred while listing translations: {str(e)}\nPlease try again later."

    @kernel_function(description="Gets details about a specific translation")
    def get_translation_details(self, translation_id: Annotated[str, "The ID of the translation"]) -> Annotated[
            str, "Returns details about the specified translation"]:
        """Gets details about a specific translation."""
        try:
            logger.debug(f"Getting details for translation ID: {translation_id}")
            if not translation_id or not translation_id.strip():
                logger.error("Missing translation ID parameter")
                return "Error: Translation ID is required. Please provide a valid translation ID."

            success, error, translation = self.client.request_get_translation(translation_id=translation_id)

            if success and translation:
                logger.debug(f"Successfully retrieved translation details for ID: {translation_id}")
                return translation
            else:
                logger.warning(f"Translation not found or error: {error}")
                return f"Translation not found or error: {error}\nPlease verify your translation ID."
        except Exception as e:
            logger.exception(f"Exception while getting translation details: {str(e)}")
            return (f"An error occurred while getting translation details: {str(e)}\n"
                    f"Please check your translation ID and try again.")

    @kernel_function(description="Deletes a specific translation")
    def delete_translation(self, translation_id: Annotated[str, "The ID of the translation to delete"]) -> Annotated[
            str, "Returns the status of the delete operation"]:
        """Deletes a specific translation."""
        try:
            logger.debug(f"Attempting to delete translation ID: {translation_id}")
            if not translation_id or not translation_id.strip():
                logger.error("Missing translation ID parameter")
                return "Error: Translation ID is required. Please provide a valid translation ID."

            success, error = self.client.request_delete_translation(translation_id)

            if success:
                logger.debug(f"Translation {translation_id} deleted successfully")
                return f"Translation {translation_id} deleted successfully."
            else:
                logger.error(f"Failed to delete translation {translation_id}: {error}")
                return f"Failed to delete translation: {error}"
        except Exception as e:
            logger.exception(f"Exception while deleting translation: {str(e)}")
            return f"An error occurred while deleting the translation: {str(e)}\nPlease check your translation ID and try again."

    @kernel_function(description="Uploads a local file to Azure Blob Storage")
    async def upload_to_azure_blob(self, local_file_path, container_name, blob_name):
        """Uploads a local video file to Azure Blob Storage"""
        try:
            logger.debug(f"Uploading file {local_file_path} to {container_name}/{blob_name}")
            if not local_file_path or not os.path.exists(local_file_path):
                logger.error(f"File not found: {local_file_path}")
                return f"Error: File not found at path: {local_file_path}. Please provide a valid file path."

            if not container_name or not container_name.strip():
                logger.error("Missing container name parameter")
                return "Error: Container name is required. Please provide a valid container name."

            if not blob_name or not blob_name.strip():
                logger.error("Missing blob name parameter")
                return "Error: Blob name is required. Please provide a valid blob name."

            storage_account_name = os.getenv('AZURE_STORAGE_ACCOUNT_NAME')
            if not storage_account_name:
                logger.error("AZURE_STORAGE_ACCOUNT_NAME environment variable not set")
                return "Error: AZURE_STORAGE_ACCOUNT_NAME environment variable is not set. Please configure your environment."

            account_url = f"https://{storage_account_name}.blob.core.windows.net"

            blob_url = f"https://{storage_account_name}.blob.core.windows.net/{container_name}/{blob_name}"

            logger.debug(f"Starting upload to {account_url}")

            async with AsyncDefaultAzureCredential() as credential:
                blob_service_client = BlobServiceClient(account_url, credential=credential)

                try:
                    container_client = blob_service_client.get_container_client(container_name)

                    try:
                        logger.debug(f"Creating container {container_name} if it doesn't exist")
                        await container_client.create_container()
                    except Exception as e:
                        logger.debug(f"Container {container_name} already exists or error occurred: {str(e)}")

                    blob_client = container_client.get_blob_client(blob_name)

                    logger.debug(f"Beginning file upload: {local_file_path}")
                    file_size = os.path.getsize(local_file_path)
                    logger.debug(f"Uploading {file_size} bytes")

                    with open(file=local_file_path, mode="rb") as data:
                        await blob_client.upload_blob(data=data, overwrite=True)

                    logger.debug("File upload complete")

                    start_time = datetime.now()
                    expiry_time = start_time + timedelta(hours=24)

                    try:
                        logger.debug("Generating SAS token")
                        user_delegation_key = await blob_service_client.get_user_delegation_key(
                            key_start_time=start_time,
                            key_expiry_time=expiry_time
                        )

                        sas_token = generate_blob_sas(
                            account_name=storage_account_name,
                            container_name=container_name,
                            blob_name=blob_name,
                            user_delegation_key=user_delegation_key,
                            permission=BlobSasPermissions(read=True),
                            expiry=expiry_time
                        )

                        sas_url = f"{blob_url}?{sas_token}"
                        logger.debug("SAS URL generated (valid for 24 hours)")

                        return (f"File uploaded successfully! \nAccess URL: {blob_url} \n"
                                f"Secure access URL (valid for 24 hours): {sas_url}")
                    except Exception as e:
                        logger.warning(f"Failed to generate SAS URL: {str(e)}")
                        return (f"File uploaded successfully! \nAccess URL: {blob_url} \n"
                                f"Note: Could not generate a secure access URL. Error: {str(e)} "
                                f"The user can either manually generate the SAS url from Azure portal "
                                f"or double check their permissions and credentials before trying again.")
                finally:
                    logger.debug("Closing blob service client")
                    await blob_service_client.close()

        except Exception as e:
            logger.exception(f"Exception during file upload: {str(e)}")
            return f"Error uploading file: {str(e)}\nPlease check your file path, permissions, and Azure credentials."


async def main() -> None:
    """Main function using ChatCompletionAgent pattern."""
    print("Starting Video Translation Agent...")
    print("Type 'exit' or 'quit' to end the conversation.")
    print("-" * 50)

    try:
        # Initialize the kernel
        kernel = Kernel()

        # Get configuration from environment variables
        endpoint = os.getenv("AZURE_AI_AGENT_ENDPOINT") or os.getenv("COGNITIVE_SERVICES_ENDPOINT")
        deployment_name = os.getenv("AZURE_AI_AGENT_MODEL_DEPLOYMENT_NAME", "gpt-4")

        if not endpoint:
            raise ValueError("Missing AZURE_AI_AGENT_ENDPOINT or COGNITIVE_SERVICES_ENDPOINT in environment variables")

        logger.debug(f"Using endpoint: {endpoint}")
        logger.debug(f"Using deployment: {deployment_name}")

        # Use Azure CLI credential for authentication
        credential = AzureCliCredential()
        token_provider = get_bearer_token_provider(
            credential,
            "https://cognitiveservices.azure.com/.default"
        )

        chat_service = AzureChatCompletion(
            service_id="azure_chat_completion",
            deployment_name=deployment_name,
            endpoint=endpoint,
            ad_token_provider=token_provider,
        )
        kernel.add_service(chat_service)

        # Add the video translation plugin to the kernel
        video_plugin = VideoTranslationPlugin()
        kernel.add_plugin(video_plugin, plugin_name="VideoTranslation")

        logger.debug("Successfully initialized kernel with chat service and video translation plugin")

        # Create a ChatCompletionAgent
        agent = ChatCompletionAgent(
            service=chat_service,
            kernel=kernel,
            name="VideoTranslationAgent",
            instructions="""You are a helpful video translation agent. Help users translate their videos from one language to another.

When a user wants to translate a video, determine whether they have a local video file or a remote URL:

For local video files:
1. First, offer to upload their local video file to Azure Blob Storage using the upload_to_azure_blob function
2. Use the generated SAS URL as the video_url for the translation request

For remote video URLs:
1. Use the provided URL directly for the translation request
2. Make sure the URL is accessible (has necessary SAS token if from Azure Storage)

In either case, gather the following information:
1. Source language (e.g., en-US, ja-JP, es-ES)
2. Target language (e.g., en-US, ja-JP, es-ES)
3. Voice kind (PlatformVoice or PersonalVoice)

Available functions:
- translate_video: Translates a video file from source to target language
- create_iteration_with_webvtt: Creates an iteration with WebVTT file for existing translation
- list_translations: Lists all translations for the user
- get_translation_details: Gets details about a specific translation
- delete_translation: Deletes a specific translation
- upload_to_azure_blob: Uploads a local file to Azure Blob Storage

Be friendly, helpful, and guide users through the process. When a user provides a local file path, always offer to upload it first.""",
        )

        # Create chat history
        chat_history = ChatHistory()

        print("Video Translation Agent is ready! How can I help you translate your videos today?")

        while True:
            # Get user input from console
            user_input = input("\nYou: ")

            # Check for exit commands
            if user_input.lower() in ["exit", "quit"]:
                print("Ending conversation...")
                break

            # Add user message to chat history
            chat_history.add_user_message(user_input)

            print("Agent is processing...")

            try:
                # Invoke the agent
                async for response in agent.invoke(chat_history):
                    print(f"\nAgent: {response.content}")

            except Exception as e:
                logger.error(f"Error during chat: {e}")
                print(f"Error during chat: {e}")
                print("Please try again with a different request.")
                continue

    except Exception as e:
        logger.error(f"Fatal error in main: {e}")
        print(f"Fatal error in main: {e}")
        raise


if __name__ == "__main__":
    asyncio.run(main())

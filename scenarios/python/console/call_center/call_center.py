#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#

from datetime import datetime
from functools import reduce
from http import HTTPStatus
from itertools import chain
from json import dumps, loads
from os import linesep
from pathlib import Path
from time import sleep
from typing import Dict, List, Tuple
import uuid
import helper

# This should not change unless you switch to a new version of the Speech REST API.
speech_transcription_path = "/speechtotext/v3.0/transcriptions"

# These should not change unless you switch to a new version of the Cognitive Language REST API.
sentiment_analysis_path = "/language/:analyze-text";
sentiment_analysis_query = "?api-version=2022-05-01";
conversation_analysis_path = "/language/analyze-conversations/jobs";
conversation_analysis_query = "?api-version=2022-05-15-preview";
conversation_summary_model_version = "2022-05-15-preview";

# How long to wait while polling batch transcription and conversation analysis status.
wait_seconds = 10

def create_transcription(user_config : helper.Read_Only_Dict) -> str :
    uri = f"https://{user_config['speech_endpoint']}{speech_transcription_path}"

    # Create Transcription API JSON request sample and schema:
    # https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
    # Notes:
    # - locale and displayName are required.
    # - diarizationEnabled should only be used with mono audio input.
    content = {
        "contentUrls" : [user_config["input_audio_url"]],
        "properties" : { "diarizationEnabled" : not user_config["use_stereo_audio"] },
        "locale" : user_config["locale"],
        "displayName" : f"call_center_{datetime.now()}",
    }

    response = helper.send_post(uri=uri, content=content, key=user_config["speech_subscription_key"], expected_status_codes=[HTTPStatus.CREATED])
    
    # Create Transcription API JSON response sample and schema:
    # https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
    transcription_uri = response["json"]["self"]
    # The transcription ID is at the end of the transcription URI.
    transcription_id = transcription_uri.split("/")[-1];
    # Verify the transcription ID is a valid GUID.
    try :
        uuid.UUID(transcription_id)
        return transcription_id
    except ValueError:
        raise Exception(f"Unable to parse response from Create Transcription API:{linesep}{response['text']}")

def get_transcription_status(transcription_id : str, user_config : helper.Read_Only_Dict) -> bool :
    uri = f"https://{user_config['speech_endpoint']}{speech_transcription_path}/{transcription_id}"
    response = helper.send_get(uri=uri, key=user_config["speech_subscription_key"], expected_status_codes=[HTTPStatus.OK])
    if "failed" == response["json"]["status"].lower() :
        raise Exception(f"Unable to transcribe audio input. Response:{linesep}{response['text']}")
    else :
        return "succeeded" == response["json"]["status"].lower()

def wait_for_transcription(transcription_id : str, user_config : helper.Read_Only_Dict) -> None :
    done = False
    while not done :
        print(f"Waiting {wait_seconds} seconds for transcription to complete.")
        sleep(wait_seconds)
        done = get_transcription_status(transcription_id, user_config=user_config)

def get_transcription_files(transcription_id : str, user_config : helper.Read_Only_Dict) -> Dict :
    uri = f"https://{user_config['speech_endpoint']}{speech_transcription_path}/{transcription_id}/files"
    response = helper.send_get(uri=uri, key=user_config["speech_subscription_key"], expected_status_codes=[HTTPStatus.OK])
    return response["json"]

def get_transcription_uri(transcription_files : Dict, user_config : helper.Read_Only_Dict) -> str :
    # Get Transcription Files JSON response sample and schema:
    # https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
    value = next(filter(lambda value: "Transcription" == value["kind"], transcription_files["values"]), None)    
    if value is None :
        raise Exception (f"Unable to parse response from Get Transcription Files API:{linesep}{transcription_files['text']}")
    return value["links"]["contentUrl"]

def get_transcription(transcription_uri : str) -> Dict :
    response = helper.send_get(uri=transcription_uri, key="", expected_status_codes=[HTTPStatus.OK])
    return response["json"]

def get_transcription_phrases_and_speakers(transcription : Dict, user_config : helper.Read_Only_Dict) -> List[Dict] :
    def helper(phrase : Dict) -> Dict :
        return {
            "phrase" : phrase["nBest"][0]["display"],
            # If the user specified stereo audio, and therefore we turned off diarization,
            # the speaker number is replaced by a channel number.
            "speaker_number" : int(phrase["channel"]) if user_config["use_stereo_audio"] else int(phrase["speaker"]),
            "offset" : float(phrase["offsetInTicks"]),
        }
    phrases_1 = list(map(helper, transcription["recognizedPhrases"]))
    # For stereo audio, the phrases are sorted by channel number, so resort them by offset.
    return sorted(phrases_1, key=lambda x : x["offset"]) if user_config["use_stereo_audio"] else phrases_1

def delete_transcription(transcription_id : str, user_config : helper.Read_Only_Dict) -> None :
    uri = f"https://{user_config['speech_endpoint']}{speech_transcription_path}/{transcription_id}"
    helper.send_delete(uri=uri, key=user_config["speech_subscription_key"], expected_status_codes=[HTTPStatus.NO_CONTENT])

def get_sentiments_helper(documents : List[Dict], user_config : helper.Read_Only_Dict) -> Dict :
    uri = f"https://{user_config['language_endpoint']}{sentiment_analysis_path}{sentiment_analysis_query}"
    content = {
        "kind" : "SentimentAnalysis",
        "analysisInput" : { "documents" : documents },
    }
    response = helper.send_post(uri = uri, content=content, key=user_config["language_subscription_key"], expected_status_codes=[HTTPStatus.OK])
    return response["json"]["results"]["documents"]

def get_sentiment_analysis(phrases : List[str], user_config : helper.Read_Only_Dict) -> Dict :
    # Convert each transcription phrase to a "document" as expected by the sentiment analysis REST API.
    # Include a counter to use as a document ID.
    # We can only analyze sentiment for 10 documents per request.
    documents_to_send = helper.chunk ([{
        "id" : id,
        "language" : user_config["language"],
        "text" : phrase,
    } for id, phrase in enumerate(phrases)], 10)
    # Get the sentiments for each chunk of documents.
    documents_1 = list(map(lambda xs : get_sentiments_helper(xs, user_config), documents_to_send))
    # Flatten the results into a single list.
    documents_2 = list(chain.from_iterable(documents_1))
    return {
        "kind" : "SentimentAnalysisResults",
        "results" :
        {
            "documents" : documents_2,
        },
    }

def get_sentiments(sentiment_analysis : Dict) -> List[str] :
    sentiments_1 = [{
        "id" : int(document["id"]),
        "sentiment" : document["sentiment"],
    } for document in sentiment_analysis["results"]["documents"]]
    # Sort by document ID.
    sentiments_2 = sorted(sentiments_1, key=lambda x : x["id"])
    # Discard document ID.
    return list(map(lambda x : x["sentiment"], sentiments_2))

def transcription_phrases_to_conversation_items(phrases_and_speakers : List[Dict]) -> List[Dict] :
    return [{
        "id" : document_id,
        "text" : phrase_and_speaker["phrase"],
        # The first person to speak is probably the agent.
        "role" : "Agent" if 1 == phrase_and_speaker["speaker_number"] else "Customer",
        "participantId" : phrase_and_speaker["speaker_number"]
    } for document_id, phrase_and_speaker in enumerate(phrases_and_speakers)]

def request_conversation_analysis(conversation_items : List[Dict], user_config : helper.Read_Only_Dict) -> str :
    uri = f"https://{user_config['language_endpoint']}{conversation_analysis_path}{conversation_analysis_query}"
    content = {
        "displayName" : f"call_center_{datetime.now()}",
        "analysisInput" : {
            "conversations" : [{
                "id" : "conversation1",
                "language" : user_config["language"],
                "modality" : "text",
                "conversationItems" : conversation_items,
            }],
        },
        "tasks" : [
            {
                "taskName" : "summary_1",
                "kind" : "ConversationalSummarizationTask",
                "parameters" : {
                   "modelVersion" : conversation_summary_model_version,
                    "summaryAspects" : [
                        "Issue",
                        "Resolution"
                    ],
                }
            },
            {
                "taskName" : "PII_1",
                "kind" : "ConversationalPIITask",
            }
        ]
    }
    response = helper.send_post(uri=uri, content=content, key=user_config["language_subscription_key"], expected_status_codes=[HTTPStatus.ACCEPTED])
    return response["headers"]["operation-location"]

def get_conversation_analysis_status(conversation_analysis_url : str, user_config : helper.Read_Only_Dict) -> bool :
    response = helper.send_get(uri=conversation_analysis_url, key=user_config["language_subscription_key"], expected_status_codes=[HTTPStatus.OK])
    if "failed" == response["json"]["status"].lower() :
        raise Exception(f"Unable to analyze conversation. Response:{linesep}{response['text']}")
    else :
        return "succeeded" == response["json"]["status"].lower()

def wait_for_conversation_analysis(conversation_analysis_url : str, user_config : helper.Read_Only_Dict) -> None :
    done = False
    while not done :
        print(f"Waiting {wait_seconds} seconds for conversation analysis to complete.")
        sleep(wait_seconds)
        done = get_conversation_analysis_status(conversation_analysis_url, user_config=user_config)

def get_conversation_analysis(conversation_analysis_url : str, user_config : helper.Read_Only_Dict) -> Dict :
    response = helper.send_get(uri=conversation_analysis_url, key=user_config["language_subscription_key"], expected_status_codes=[HTTPStatus.OK])
    return response["json"]

def get_conversation_analysis_result(conversation_analysis : Dict, user_config : helper.Read_Only_Dict) -> Dict :
    tasks = conversation_analysis["tasks"]["items"]
    summary_task = next(filter(lambda task : "summary_1" == task["taskName"], tasks), None)
    if summary_task is None :
        raise Exception (f"Unable to parse response from Get Conversation Analysis API:{linesep}{conversation_analysis['text']}")
    conversation = summary_task["results"]["conversations"][0]
    aspects = list(map(lambda summary : { "aspect" : summary["aspect"], "text" : summary["text"] }, conversation["summaries"]))

    pii_task = next(filter(lambda task : "PII_1" == task["taskName"], tasks), None)
    if pii_task is None :
        raise Exception (f"Unable to parse response from Get Conversation Analysis API:{linesep}{conversation_analysis['text']}")
    conversation = pii_task["results"]["conversations"][0]
    pii = [
        [{
            "category" : entity["category"],
            "text" : entity["text"],
        } for entity in conversation_item["entities"]]
    for conversation_item in conversation["conversationItems"]]

    return {
        "conversation_summary" : aspects,
        "pii_analysis" : pii,
    }

def get_results(phrases_and_speakers : List[Dict], sentiments : List[str], conversation_analysis : Dict) -> str :
    result = ""
    for i, phrase_and_speaker in enumerate(phrases_and_speakers) :
        result += f"Phrase: {phrases_and_speakers[i]['phrase']}{linesep}"
        result += f"Speaker: {phrases_and_speakers[i]['speaker_number']}{linesep}"
        if i < len(sentiments) :
            result += f"Sentiment: {sentiments[i]}{linesep}"
        if i < len(conversation_analysis["pii_analysis"]) :
            if len(conversation_analysis["pii_analysis"][i]) > 0 :
                entities = reduce(lambda acc, entity : f"{acc}    Category: {entity['category']}. Text: {entity['text']}.{linesep}", conversation_analysis["pii_analysis"][i])
                result += f"Recognized entities (PII):{linesep}{entities}"
            else :
                result += f"Recognized entities (PII): none.{linesep}"
        result += linesep
        result += reduce(lambda acc, item : f"{acc}    Aspect: {item['aspect']}. Summary: {item['text']}.{linesep}", conversation_analysis["conversation_summary"], f"Conversation summary:{linesep}")
    return result

def print_results(transcription : Dict, sentiment_analysis : Dict, conversation_analysis : Dict) -> None :
    results = {
        "transcription" : transcription,
        "sentiment_analysis" : sentiment_analysis,
        "conversation_analysis" : conversation_analysis
    }
    print(dumps(results, indent=4));

def run() -> None :
    usage = """USAGE: dotnet run -- [...]

  HELP
    --help                          Show this help and stop.

  CONNECTION
    --speechKey KEY                 Your Azure Speech service subscription key. Required.
    --speechRegion REGION           Your Azure Speech service region. Required.
                                    Examples: westus, eastus
    --languageKey KEY               Your Azure Cognitive Language subscription key. Required.
    --languageEndpoint ENDPOINT     Your Azure Cognitive Language endpoint. Required.

  LANGUAGE
    --language LANGUAGE             The language to use for sentiment analysis and conversation analysis.
                                    This should be a two-letter ISO 639-1 code.
                                    Default: en
    --locale LOCALE                 The locale to use for batch transcription of audio.
                                    Default: en-US

  INPUT
    --input URL                     Input audio from URL. Required.
    --stereo                        Use stereo audio format.
                                    If this is not present, mono is assumed.
                                    
  OUTPUT
    --output FILE                   Output phrase list and conversation summary to text file.
"""

    if helper.cmd_option_exists("--help") :
        print(usage)
    else :
        user_config = helper.user_config_from_args(usage)
        # How to use batch transcription:
        # https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
        transcription_id = create_transcription(user_config)
        wait_for_transcription(transcription_id, user_config)
        print(f"Transcription ID: {transcription_id}")
        transcription_files = get_transcription_files(transcription_id, user_config)
        transcription_uri = get_transcription_uri(transcription_files, user_config)
        transcription = get_transcription(transcription_uri);
        phrases_and_speakers = get_transcription_phrases_and_speakers(transcription, user_config)
        phrases = list(map(lambda x : x["phrase"], phrases_and_speakers))
        sentiment_analysis = get_sentiment_analysis(phrases, user_config);
        conversation_items = transcription_phrases_to_conversation_items(phrases_and_speakers)
        # NOTE: Conversation summary is currently in gated public preview. You can sign up here:
        # https://aka.ms/applyforconversationsummarization/
        conversation_analysis_url = request_conversation_analysis(conversation_items, user_config)
        wait_for_conversation_analysis(conversation_analysis_url, user_config)
        conversation_analysis = get_conversation_analysis(conversation_analysis_url, user_config)
        if user_config["output_file_path"] is not None :
            sentiments = get_sentiments(sentiment_analysis)
            conversation_analysis_result = get_conversation_analysis_result(conversation_analysis, user_config)
            output_file_path = Path(user_config["output_file_path"])
            with open(output_file_path, mode="w", newline="") as f :
                f.write(get_results(phrases_and_speakers, sentiments, conversation_analysis_result))
        print_results(transcription, sentiment_analysis, conversation_analysis)
        # Clean up resources.
        print("Deleting transcription.")
        delete_transcription(transcription_id, user_config)

run()
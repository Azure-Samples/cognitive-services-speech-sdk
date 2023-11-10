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
import rest_helper
import user_config_helper

# This should not change unless you switch to a new version of the Speech REST API.
SPEECH_TRANSCRIPTION_PATH = "/speechtotext/v3.0/transcriptions"

# These should not change unless you switch to a new version of the Cognitive Language REST API.
SENTIMENT_ANALYSIS_PATH = "/language/:analyze-text"
SENTIMENT_ANALYSIS_QUERY = "?api-version=2022-05-01"
CONVERSATION_ANALYSIS_PATH = "/language/analyze-conversations/jobs"
CONVERSATION_ANALYSIS_QUERY = "?api-version=2022-05-15-preview"
CONVERSATION_SUMMARY_MODEL_VERSION = "2022-05-15-preview"

# How long to wait while polling batch transcription and conversation analysis status.
WAIT_SECONDS = 10

class TranscriptionPhrase(object) :
    def __init__(self, id : int, text : str, itn : str, lexical : str, speaker_number : int, offset : str, offset_in_ticks : float) :
        self.id = id
        self.text = text
        self.itn = itn
        self.lexical = lexical
        self.speaker_number = speaker_number
        self.offset = offset
        self.offset_in_ticks = offset_in_ticks
        
class SentimentAnalysisResult(object) :
    def __init__(self, speaker_number : int, offset_in_ticks : float, document : Dict) :
        self.speaker_number = speaker_number
        self.offset_in_ticks = offset_in_ticks
        self.document = document

class ConversationAnalysisSummaryItem(object) :
    def __init__(self, aspect : str, summary : str) :
        self.aspect = aspect
        self.summary = summary

class ConversationAnalysisPiiItem(object) :
    def __init__(self, category : str, text : str) :
        self.category = category
        self.text = text

class ConversationAnalysisForSimpleOutput(object) :
    def __init__(self, summary : List[ConversationAnalysisSummaryItem], pii_analysis : List[List[ConversationAnalysisPiiItem]]) :
        self.summary = summary
        self.pii_analysis = pii_analysis

# This needs to be serialized to JSON, so we use a Dict instead of a class.
def get_combined_redacted_content(channel : int) -> Dict :
    return {
        "channel" : channel,
        "display" : "",
        "itn" : "",
        "lexical" : ""
    }

def create_transcription(user_config : helper.Read_Only_Dict) -> str :
    uri = f"https://{user_config['speech_endpoint']}{SPEECH_TRANSCRIPTION_PATH}"

    # Create Transcription API JSON request sample and schema:
    # https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
    # Notes:
    # - locale and displayName are required.
    # - diarizationEnabled should only be used with mono audio input.
    content = {
        "contentUrls" : [user_config["input_audio_url"]],
        "properties" : {
            "diarizationEnabled" : not user_config["use_stereo_audio"],
            "timeToLive" : "PT30M"
        },
        "locale" : user_config["locale"],
        "displayName" : f"call_center_{datetime.now()}",
    }

    response = rest_helper.send_post(uri=uri, content=content, key=user_config["speech_subscription_key"], expected_status_codes=[HTTPStatus.CREATED])
    
    # Create Transcription API JSON response sample and schema:
    # https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
    transcription_uri = response["json"]["self"]
    # The transcription ID is at the end of the transcription URI.
    transcription_id = transcription_uri.split("/")[-1]
    # Verify the transcription ID is a valid GUID.
    try :
        uuid.UUID(transcription_id)
        return transcription_id
    except ValueError:
        raise Exception(f"Unable to parse response from Create Transcription API:{linesep}{response['text']}")

def get_transcription_status(transcription_id : str, user_config : helper.Read_Only_Dict) -> bool :
    uri = f"https://{user_config['speech_endpoint']}{SPEECH_TRANSCRIPTION_PATH}/{transcription_id}"
    response = rest_helper.send_get(uri=uri, key=user_config["speech_subscription_key"], expected_status_codes=[HTTPStatus.OK])
    if "failed" == response["json"]["status"].lower() :
        raise Exception(f"Unable to transcribe audio input. Response:{linesep}{response['text']}")
    else :
        return "succeeded" == response["json"]["status"].lower()

def wait_for_transcription(transcription_id : str, user_config : helper.Read_Only_Dict) -> None :
    done = False
    while not done :
        print(f"Waiting {WAIT_SECONDS} seconds for transcription to complete.")
        sleep(WAIT_SECONDS)
        done = get_transcription_status(transcription_id, user_config=user_config)

def get_transcription_files(transcription_id : str, user_config : helper.Read_Only_Dict) -> Dict :
    uri = f"https://{user_config['speech_endpoint']}{SPEECH_TRANSCRIPTION_PATH}/{transcription_id}/files"
    response = rest_helper.send_get(uri=uri, key=user_config["speech_subscription_key"], expected_status_codes=[HTTPStatus.OK])
    return response["json"]

def get_transcription_uri(transcription_files : Dict, user_config : helper.Read_Only_Dict) -> str :
    # Get Transcription Files JSON response sample and schema:
    # https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
    value = next(filter(lambda value: "transcription" == value["kind"].lower(), transcription_files["values"]), None)    
    if value is None :
        raise Exception (f"Unable to parse response from Get Transcription Files API:{linesep}{transcription_files['text']}")
    return value["links"]["contentUrl"]

def get_transcription(transcription_uri : str) -> Dict :
    response = rest_helper.send_get(uri=transcription_uri, key="", expected_status_codes=[HTTPStatus.OK])
    return response["json"]

def get_transcription_phrases(transcription : Dict, user_config : helper.Read_Only_Dict) -> List[TranscriptionPhrase] :
    def helper(id_and_phrase : Tuple[int, Dict]) -> TranscriptionPhrase :
        (id, phrase) = id_and_phrase
        best = phrase["nBest"][0]
        speaker_number : int
        # If the user specified stereo audio, and therefore we turned off diarization,
        # only the channel property is present.
        # Note: Channels are numbered from 0. Speakers are numbered from 1.
        if "speaker" in phrase :
            speaker_number = phrase["speaker"] - 1
        elif "channel" in phrase :
            speaker_number = phrase["channel"]
        else :
            raise Exception(f"nBest item contains neither channel nor speaker attribute.{linesep}{best}")
        return TranscriptionPhrase(id, best["display"], best["itn"], best["lexical"], speaker_number, phrase["offset"], phrase["offsetInTicks"])
    # For stereo audio, the phrases are sorted by channel number, so resort them by offset.
    return list(map(helper, enumerate(transcription["recognizedPhrases"])))

def delete_transcription(transcription_id : str, user_config : helper.Read_Only_Dict) -> None :
    uri = f"https://{user_config['speech_endpoint']}{SPEECH_TRANSCRIPTION_PATH}/{transcription_id}"
    rest_helper.send_delete(uri=uri, key=user_config["speech_subscription_key"], expected_status_codes=[HTTPStatus.NO_CONTENT])

def get_sentiments_helper(documents : List[Dict], user_config : helper.Read_Only_Dict) -> Dict :
    uri = f"https://{user_config['language_endpoint']}{SENTIMENT_ANALYSIS_PATH}{SENTIMENT_ANALYSIS_QUERY}"
    content = {
        "kind" : "SentimentAnalysis",
        "analysisInput" : { "documents" : documents },
    }
    response = rest_helper.send_post(uri = uri, content=content, key=user_config["language_subscription_key"], expected_status_codes=[HTTPStatus.OK])
    return response["json"]["results"]["documents"]

def get_sentiment_analysis(phrases : List[TranscriptionPhrase], user_config : helper.Read_Only_Dict) -> List[SentimentAnalysisResult] :
    retval : List[SentimentAnalysisResult] = []
    # Create a map of phrase ID to phrase data so we can retrieve it later.
    phrase_data : Dict = {}
    # Convert each transcription phrase to a "document" as expected by the sentiment analysis REST API.
    # Include a counter to use as a document ID.
    documents : List[Dict] = []
    for phrase in phrases :
        phrase_data[phrase.id] = (phrase.speaker_number, phrase.offset_in_ticks)
        documents.append({
            "id" : phrase.id,
            "language" : user_config["language"],
            "text" : phrase.text,
        })
    # We can only analyze sentiment for 10 documents per request.
    # Get the sentiments for each chunk of documents.
    result_chunks = list(map(lambda xs : get_sentiments_helper(xs, user_config), helper.chunk (documents, 10)))
    for result_chunk in result_chunks :
        for document in result_chunk :
            retval.append(SentimentAnalysisResult(phrase_data[int(document["id"])][0], phrase_data[int(document["id"])][1], document))
    return retval

def get_sentiments_for_simple_output(sentiment_analysis_results : List[SentimentAnalysisResult]) -> List[str] :
    sorted_by_offset = sorted(sentiment_analysis_results, key=lambda x : x.offset_in_ticks)
    return list(map(lambda result : result.document["sentiment"], sorted_by_offset))

def get_sentiment_confidence_scores(sentiment_analysis_results : List[SentimentAnalysisResult]) -> List[Dict] :
    sorted_by_offset = sorted(sentiment_analysis_results, key=lambda x : x.offset_in_ticks)
    return list(map(lambda result : result.document["confidenceScores"], sorted_by_offset))

def merge_sentiment_confidence_scores_into_transcription(transcription : Dict, sentiment_confidence_scores : List[Dict]) -> Dict :
    for id, phrase in enumerate(transcription["recognizedPhrases"]) :
        for best_item in phrase["nBest"] :
            best_item["sentiment"] = sentiment_confidence_scores[id]
    return transcription

def transcription_phrases_to_conversation_items(phrases : List[TranscriptionPhrase]) -> List[Dict] :
    return [{
        "id" : phrase.id,
        "text" : phrase.text,
        "itn" : phrase.itn,
        "lexical" : phrase.lexical,
        # The first person to speak is probably the agent.
        "role" : "Agent" if 0 == phrase.speaker_number else "Customer",
        "participantId" : phrase.speaker_number
    } for phrase in phrases]

def request_conversation_analysis(conversation_items : List[Dict], user_config : helper.Read_Only_Dict) -> str :
    uri = f"https://{user_config['language_endpoint']}{CONVERSATION_ANALYSIS_PATH}{CONVERSATION_ANALYSIS_QUERY}"
    content = {
        "displayName" : f"call_center_{datetime.now()}",
        "analysisInput" : {
            "conversations" : [{
                "id" : "conversation1",
                "language" : user_config["language"],
                "modality" : "transcript",
                "conversationItems" : conversation_items,
            }],
        },
        "tasks" : [
            {
                "taskName" : "summary_1",
                "kind" : "ConversationalSummarizationTask",
                "parameters" : {
                   "modelVersion" : CONVERSATION_SUMMARY_MODEL_VERSION,
                    "summaryAspects" : [
                        "Issue",
                        "Resolution"
                    ],
                }
            },
            {
                "taskName" : "PII_1",
                "kind" : "ConversationalPIITask",
                "parameters" : {
                    "piiCategories" : [
                        "All",
                    ],
                    "includeAudioRedaction" : False,
                    "redactionSource" : "text",
                    "modelVersion" : CONVERSATION_SUMMARY_MODEL_VERSION,
                    "loggingOptOut" : False
                }
            }
        ]
    }
    response = rest_helper.send_post(uri=uri, content=content, key=user_config["language_subscription_key"], expected_status_codes=[HTTPStatus.ACCEPTED])
    return response["headers"]["operation-location"]

def get_conversation_analysis_status(conversation_analysis_url : str, user_config : helper.Read_Only_Dict) -> bool :
    response = rest_helper.send_get(uri=conversation_analysis_url, key=user_config["language_subscription_key"], expected_status_codes=[HTTPStatus.OK])
    if "failed" == response["json"]["status"].lower() :
        raise Exception(f"Unable to analyze conversation. Response:{linesep}{response['text']}")
    else :
        return "succeeded" == response["json"]["status"].lower()

def wait_for_conversation_analysis(conversation_analysis_url : str, user_config : helper.Read_Only_Dict) -> None :
    done = False
    while not done :
        print(f"Waiting {WAIT_SECONDS} seconds for conversation analysis to complete.")
        sleep(WAIT_SECONDS)
        done = get_conversation_analysis_status(conversation_analysis_url, user_config=user_config)

def get_conversation_analysis(conversation_analysis_url : str, user_config : helper.Read_Only_Dict) -> Dict :
    response = rest_helper.send_get(uri=conversation_analysis_url, key=user_config["language_subscription_key"], expected_status_codes=[HTTPStatus.OK])
    return response["json"]

def get_conversation_analysis_for_simple_output(conversation_analysis : Dict, user_config : helper.Read_Only_Dict) -> ConversationAnalysisForSimpleOutput :
    tasks = conversation_analysis["tasks"]["items"]
    
    summary_task = next(filter(lambda task : "summary_1" == task["taskName"], tasks), None)
    if summary_task is None :
        raise Exception (f"Unable to parse response from Get Conversation Analysis API. Summary task missing. Response:{linesep}{conversation_analysis}")
    conversation = summary_task["results"]["conversations"][0]
    summary_items = list(map(lambda summary : ConversationAnalysisSummaryItem(summary["aspect"], summary["text"]), conversation["summaries"]))

    pii_task = next(filter(lambda task : "PII_1" == task["taskName"], tasks), None)
    if pii_task is None :
        raise Exception (f"Unable to parse response from Get Conversation Analysis API. PII task missing. Response:{linesep}{conversation_analysis}")
    conversation = pii_task["results"]["conversations"][0]
    pii_items = [[ConversationAnalysisPiiItem(entity["category"], entity["text"])
        for entity in conversation_item["entities"]]
        for conversation_item in conversation["conversationItems"]]

    return ConversationAnalysisForSimpleOutput(summary_items, pii_items)

def get_simple_output(phrases : List[TranscriptionPhrase], sentiments : List[str], conversation_analysis : ConversationAnalysisForSimpleOutput) -> str :
    result = ""
    for index, phrase in enumerate(phrases) :
        result += f"Phrase: {phrase.text}{linesep}"
        result += f"Speaker: {phrase.speaker_number}{linesep}"
        if index < len(sentiments) :
            result += f"Sentiment: {sentiments[index]}{linesep}"
        if index < len(conversation_analysis.pii_analysis) :
            if len(conversation_analysis.pii_analysis[index]) > 0 :
                entities = reduce(lambda acc, entity : f"{acc}    Category: {entity.category}. Text: {entity.text}.{linesep}", conversation_analysis.pii_analysis[index], "")
                result += f"Recognized entities (PII):{linesep}{entities}"
            else :
                result += f"Recognized entities (PII): none.{linesep}"
        result += linesep
        result += reduce(lambda acc, item : f"{acc}    {item.aspect}: {item.summary}.{linesep}", conversation_analysis.summary, f"Conversation summary:{linesep}")
    return result

def print_simple_output(phrases : List[TranscriptionPhrase], sentiment_analysis_results : List[SentimentAnalysisResult], conversation_analysis : Dict, user_config : helper.Read_Only_Dict) -> None :
    sentiments = get_sentiments_for_simple_output(sentiment_analysis_results)
    conversation = get_conversation_analysis_for_simple_output(conversation_analysis, user_config)
    print(get_simple_output(phrases, sentiments, conversation))

def get_conversation_analysis_for_full_output(phrases : List[TranscriptionPhrase], conversation_analysis : Dict) -> Dict :
    # Get the conversation summary and conversation PII analysis task results.
    tasks = conversation_analysis["tasks"]["items"]
    conversation_summary_results = next(filter(lambda task : "summary_1" == task["taskName"], tasks))["results"]
    conversation_pii_results = next(filter(lambda task : "PII_1" == task["taskName"], tasks))["results"]
    # There should be only one conversation.
    conversation = conversation_pii_results["conversations"][0]
    # Order conversation items by ID so they match the order of the transcription phrases.
    conversation["conversationItems"] = sorted(conversation["conversationItems"], key=lambda item : int(item["id"]))
    combined_redacted_content = [get_combined_redacted_content(0), get_combined_redacted_content(1)]
    for index, conversation_item in enumerate(conversation["conversationItems"]) :
        # Get the channel and offset for this conversation item from the corresponding transcription phrase.
        channel = phrases[index].speaker_number
        # Add channel and offset to conversation item JsonElement.
        conversation_item["channel"] = channel
        conversation_item["offset"] = phrases[index].offset
        # Get the text, lexical, and itn fields from redacted content, and append them to the combined redacted content for this channel.
        redacted_content = conversation_item["redactedContent"]
        combined_redacted_content[channel]["display"] += f"{redacted_content['text']} "
        combined_redacted_content[channel]["lexical"] += f"{redacted_content['lexical']} "
        combined_redacted_content[channel]["itn"] += f"{redacted_content['itn']} "
    return {
        "conversationSummaryResults" : conversation_summary_results,
        "conversationPiiResults" : {
            "combinedRedactedContent" : combined_redacted_content,
            "conversations" : [conversation]
        }
    }

def print_full_output(output_file_path : str, transcription : Dict, sentiment_confidence_scores : List[Dict], phrases : List[TranscriptionPhrase], conversation_analysis : Dict) -> None :
    results = {
        "transcription" : merge_sentiment_confidence_scores_into_transcription(transcription, sentiment_confidence_scores),
        "conversationAnalyticsResults" : get_conversation_analysis_for_full_output(phrases, conversation_analysis)
    }
    with open(output_file_path, mode = "w", newline = "") as f :
        f.write(dumps(results, indent=2))

def run() -> None :
    usage = """python call_center.py [...]

  HELP
    --help                          Show this help and stop.

  CONNECTION
    --speechKey KEY                 Your Azure Speech service subscription key. Required unless --jsonInput is present.
    --speechRegion REGION           Your Azure Speech service region. Required unless --jsonInput is present.
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
    --input URL                     Input audio from URL. Required unless --jsonInput is present.
    --jsonInput FILE                Input JSON Speech batch transcription result from FILE. Overrides --input.
    --stereo                        Use stereo audio format.
                                    If this is not present, mono is assumed.

  OUTPUT
    --output FILE                   Output phrase list and conversation summary to text file.
"""

    if user_config_helper.cmd_option_exists("--help") :
        print(usage)
    else :
        user_config = user_config_helper.user_config_from_args(usage)
        transcription : Dict
        transcription_id : str
        if user_config["input_file_path"] is not None :
            with open(user_config["input_file_path"], mode="r") as f :
                transcription = loads(f.read())
        elif user_config["input_audio_url"] is not None :
            # How to use batch transcription:
            # https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
            transcription_id = create_transcription(user_config)
            wait_for_transcription(transcription_id, user_config)
            print(f"Transcription ID: {transcription_id}")
            transcription_files = get_transcription_files(transcription_id, user_config)
            transcription_uri = get_transcription_uri(transcription_files, user_config)
            print(f"Transcription URI: {transcription_uri}")
            transcription = get_transcription(transcription_uri)
        else :
            raise Exception(f"Missing input audio URL.{linesep}{usage}")
        # For stereo audio, the phrases are sorted by channel number, so resort them by offset.
        transcription["recognizedPhrases"] = sorted(transcription["recognizedPhrases"], key=lambda phrase : phrase["offsetInTicks"])
        phrases = get_transcription_phrases(transcription, user_config)
        sentiment_analysis_results = get_sentiment_analysis(phrases, user_config)
        sentiment_confidence_scores = get_sentiment_confidence_scores(sentiment_analysis_results)
        conversation_items = transcription_phrases_to_conversation_items(phrases)
        # NOTE: Conversation summary is currently in gated public preview. You can sign up here:
        # https://aka.ms/applyforconversationsummarization/
        conversation_analysis_url = request_conversation_analysis(conversation_items, user_config)
        wait_for_conversation_analysis(conversation_analysis_url, user_config)
        conversation_analysis = get_conversation_analysis(conversation_analysis_url, user_config)
        print_simple_output(phrases, sentiment_analysis_results, conversation_analysis, user_config)
        if user_config["output_file_path"] is not None :
            print_full_output(user_config["output_file_path"], transcription, sentiment_confidence_scores, phrases, conversation_analysis)

run()

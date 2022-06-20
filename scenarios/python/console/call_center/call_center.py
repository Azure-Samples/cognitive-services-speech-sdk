#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#

from datetime import datetime
from functools import reduce
from http import HTTPStatus
from json import dumps, loads
from os import linesep
# To install, run:
# python -m pip install requests
import requests
from time import sleep
from typing import Dict, List, Tuple
from urllib.parse import urljoin
import uuid

# Replace this value with the region for your Speech subscription.
speechRegion = "PASTE_YOUR_SPEECH_REGION_HERE"
# Replace this value with the subscription key for your Speech subscription.
speechKey = "PASTE_YOUR_SPEECH_SUBSCRIPTION_KEY_HERE"

# Replace this value with the endpoint for your Text Analytics subscription.
textAnalyticsHost = "PASTE_YOUR_TEXT_ANALYTICS_ENDPOINT_HERE"
# Replace this value with the subscription key for your Text Analytics subscription.
textAnalyticsKey = "PASTE_YOUR_TEXT_ANALYTICS_SUBSCRIPTION_KEY_HERE"

# Replace this value with the URL of the audio file you want to transcribe.
transcriptionUri = "https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/master/sampledata/audiofiles/wikipediaOcelot.wav"

# This should not change unless the Speech REST API changes.
speechHost = f"https://{speechRegion}.api.cognitive.microsoft.com"

# This should not change unless you switch to a new version of the Speech REST API.
speechTranscriptionPath = "speechtotext/v3.0/transcriptions"

# These should not change unless you switch to a new version of the Text Analytics REST API.
detectLanguagePath = "text/analytics/v3.1/languages"
entitiesRecognitionGeneralPath = "text/analytics/v3.1/entities/recognition/general"
entitiesRecognitionPIIPath = "text/analytics/v3.1/entities/recognition/pii"
keyPhrasesPath = "text/analytics/v3.1/keyPhrases"
sentimentAnalysisPath = "text/analytics/v3.1/sentiment"

# How long to wait while polling transcription status.
waitSeconds = 5

# Send GET request with authentication and return the response content.
def send_get(uri : str, key : str, expected_status_codes : List[int]) -> Tuple[str, Dict] :
    headers = {"Ocp-Apim-Subscription-Key": key}
    response = requests.get(uri, headers=headers)
    if response.status_code not in expected_status_codes :
        raise Exception(f"The GET request to {uri} returned a status code {response.status_code} that was not in the expected status codes: {expected_status_codes}")
    else :
        return response.text, response.json()

# Send POST request with authentication and return the response content.
def send_post(uri : str, content : Dict, key : str, expected_status_codes : List[int]) -> Tuple[str, Dict] :
    headers = {"Ocp-Apim-Subscription-Key": key}
    response = requests.post(uri, headers=headers, json=content)
    if response.status_code not in expected_status_codes :
        raise Exception(f"The POST request to {uri} returned a status code {response.status_code} that was not in the expected status codes: {expected_status_codes}")
    else :
        return response.text, response.json()

def send_delete(uri : str, key : str, expected_status_codes : List[int]) -> None :
    headers = {"Ocp-Apim-Subscription-Key": key}
    response = requests.delete(uri, headers=headers)
    if response.status_code not in expected_status_codes :
        raise Exception(f"The DELETE request to {uri} returned a status code {response.status_code} that was not in the expected status codes: {expected_status_codes}")

# Create a transcription of the specified audio.
def create_transcription(transcription_uri : str) -> str :
    uri = urljoin(speechHost, speechTranscriptionPath)

    display_name = f"call_center_{datetime.now()}"

    # Create Transcription API JSON request sample and schema:
    # https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
    # Note: locale and displayName are required.
    content = {
        "contentUrls" : [transcription_uri],
        "locale" : "en-US",
        "displayName" : display_name
    }

    response_text, response_json = send_post(uri=uri, content=content, key=speechKey, expected_status_codes=[HTTPStatus.CREATED])
    
    # Create Transcription API JSON response sample and schema:
    # https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
    transcription_uri = response_json["self"]
    # The transcription ID is at the end of the transcription URI.
    transcription_id = transcription_uri.split("/")[-1];
    # Verify the transcription ID is a valid GUID.
    try :
        uuid.UUID(transcription_id)
        return transcription_id
    except ValueError:
        raise Exception(f"Unable to parse response from Create Transcription API:{linesep}{response_text}")

# Return true if the transcription has status "Succeeded".
def get_transcription_status(transcription_id : str) -> bool :
    uri = urljoin(speechHost, f"{speechTranscriptionPath}/{transcription_id}")
    response_text, response_json = send_get(uri=uri, key=speechKey, expected_status_codes=[HTTPStatus.OK])
    return "Succeeded" == response_json["status"]

# Poll the transcription status until it has status "Succeeded".
def wait_for_transcription(transcription_id : str) -> None :
    done = False
    while not done :
        print(f"Waiting {waitSeconds} seconds for transcription to complete.")
        sleep(waitSeconds)
        done = get_transcription_status(transcription_id)

def get_transcription_uri(transcription_id : str) -> str :
    uri = urljoin(speechHost, f"{speechTranscriptionPath}/{transcription_id}/files")
    response_text, response_json = send_get(uri=uri, key=speechKey, expected_status_codes=[HTTPStatus.OK])
    # Get Transcription Files JSON response sample and schema:
    # https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
    for value in response_json["values"] :
        if "Transcription" == value["kind"] :
            return value["links"]["contentUrl"]
    raise Exception (f"Unable to parse response from Get Transcription Files API:{linesep}{response_text}")

# Return transcription content as a list of phrases.
def get_transcription_phrases(transcription_uri : str) -> List[str] :
    response_text, response_json = send_get(uri=transcription_uri, key="", expected_status_codes=[HTTPStatus.OK]);
    nBests = list(map(lambda phrase : phrase["nBest"], response_json["recognizedPhrases"]))
    return list(map(lambda nBest : nBest[0]["display"], nBests))

def delete_transcription(transcription_id : str) -> None :
    uri = urljoin(speechHost, f"{speechTranscriptionPath}/{transcription_id}")
    send_delete(uri=uri, key=speechKey, expected_status_codes=[HTTPStatus.NO_CONTENT])

# Detect languages for a list of transcription phrases.
def get_transcription_languages(transcription_phrases : List[str]) -> List[str] :
    uri = urljoin(textAnalyticsHost, detectLanguagePath)
# Convert each transcription phrase to a "document" as expected by the Text Analytics Detect Language REST API.
# Include a counter to use as a document ID.
# Detect Language API JSON request and response samples:
# https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/languages/languages
    documents_1 : List[Dict] = []
    for id, phrase in enumerate(transcription_phrases) :
        documents_1.append ({
            "id" : id,
            "text" : phrase
        })
    documents_2 = {
        "documents" : documents_1
    }
    response_text, response_json = send_post(uri=uri, content=documents_2, key=textAnalyticsKey, expected_status_codes=[HTTPStatus.OK])
    return list(map(lambda document : document["detectedLanguage"]["iso6391Name"], response_json["documents"]))

# Convert a list of transcription phrases to "document" JSON elements as expected by various Text Analytics REST APIs.
def transcription_phrases_to_documents(transcription_phrases : List[str], transcription_languages : List[str]) -> Dict :
    # Combine the list of transcription phrases with the corresponding list of transcription languages into a single
    # list of tuples. Convert each (phrase, language) tuple into a "document" as expected by various Text Analytics
    # REST APIs. For example, see the sample request body for the
    # Sentiment REST API:
    # https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/sentiment/sentiment#examples
    # Include a counter to use as a document ID.
    documents_1 : List[Dict] = []
    for id, (text, language) in enumerate(list(zip(transcription_phrases, transcription_languages))) :
        documents_1.append ({
            "id" : id,
            "language" : language,
            "text" : text
        })
    return {
        "documents" : documents_1
    }

# Get sentiments for transcription phrases.
def get_sentiments(transcription_documents : Dict) -> List[str] :
    uri = urljoin(textAnalyticsHost, sentimentAnalysisPath)
    # Sentiment JSON request and response samples:
    # https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/sentiment/sentiment#examples
    response_text, response_json = send_post(uri=uri, content=transcription_documents, key=textAnalyticsKey, expected_status_codes=[HTTPStatus.OK])
    return list(map(lambda document : document["sentiment"], response_json["documents"]))

# Get key phrases for transcription phrases.
def get_key_phrases(transcription_documents : Dict) -> List[List[str]] :
    uri = urljoin(textAnalyticsHost, keyPhrasesPath)
    # Key phrases JSON request and response samples:
    # https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/key-phrases/key-phrases#examples
    response_text, response_json = send_post(uri=uri, content=transcription_documents, key=textAnalyticsKey, expected_status_codes=[HTTPStatus.OK])
    return list(map(lambda document : document["keyPhrases"], response_json["documents"]))

# Get recognized entities (general) for transcription phrases.
def get_recognized_entities_general(transcription_documents : Dict) -> List[List[Tuple[str, str]]] :
    def process_entities(entities : List[Dict]) -> List[Tuple[str, str]] :
        return list(map(lambda entity : (entity["category"], entity["text"]), entities))
    uri = urljoin(textAnalyticsHost, entitiesRecognitionGeneralPath)
    # Entities recognition general JSON request and response samples:
    # https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/entities-recognition-general/entities-recognition-general#examples
    response_text, response_json = send_post(uri=uri, content=transcription_documents, key=textAnalyticsKey, expected_status_codes=[HTTPStatus.OK])
    return list(map(lambda document : process_entities(document["entities"]), response_json["documents"]))

# Get recognized entities (PII) for transcription phrases.
def get_recognized_entities_PII(transcription_documents : Dict) -> List[List[Tuple[str, str]]] :
    def process_entities(entities : List[Dict]) -> List[Tuple[str, str]] :
        return list(map(lambda entity : (entity["category"], entity["text"]), entities))
    uri = urljoin(textAnalyticsHost, entitiesRecognitionPIIPath)
    # Entities recognition PII JSON request and response samples:
    # https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/entities-recognition-pii/entities-recognition-pii#examples
    response_text, response_json = send_post(uri=uri, content=transcription_documents, key=textAnalyticsKey, expected_status_codes=[HTTPStatus.OK])
    return list(map(lambda document : process_entities(document["entities"]), response_json["documents"]))

# Print each transcription phrase, followed by its language, sentiment, and so on.
def print_results(
    transcription_phrases : List[str],
    transcription_languages : List[str],
    transcription_sentiments : List[str],
    key_phrases : List[List[str]],
    transcription_entities_general : List[List[Tuple[str, str]]],
    transcription_entities_pii : List[List[Tuple[str, str]]]) -> None :

    phrases_2 = list(map(lambda item : f"Phrase: {item[0]}{linesep}Language: {item[1]}{linesep}", list(zip(transcription_phrases, transcription_languages))))
    phrases_3 = list(map(lambda item : f"{item[0]}Sentiment: {item[1]}{linesep}", list(zip(phrases_2, transcription_sentiments))))
    def process_key_phrases(key_phrases : List[str]) -> str :
        return reduce(lambda acc, item : f"{acc}    {item}{linesep}", key_phrases, "")
    phrases_4 = list(map(lambda item : f"{item[0]}Key phrases:{linesep}{process_key_phrases(item[1])}", list(zip(phrases_3, key_phrases))))
    def process_entities_general(entities_1 : List[Tuple[str, str]]) -> str :
        if len(entities_1) > 0 :
            entities_2 = reduce(lambda acc, item : f"{acc}    Category: {item[0]}. Text: {item[1]}.{linesep}", entities_1, "")
            return f"Recognized entities (general):{linesep}{entities_2}"
        else :
            return f"Recognized entities (general): none.{linesep}"
    phrases_5 = list(map(lambda item : f"{item[0]}{process_entities_general(item[1])}", list(zip(phrases_4, transcription_entities_general))))
    def process_entities_PII(entities_1 : List[Tuple[str, str]]) -> str :
        if len(entities_1) > 0 :
            entities_2 = reduce(lambda acc, item : f"{acc}    Category: {item[0]}. Text: {item[1]}.{linesep}", entities_1, "")
            return f"Recognized entities (PII):{linesep}{entities_2}"
        else :
            return f"Recognized entities (PII): none.{linesep}"
    phrases_6 = list(map(lambda item : f"{item[0]}{process_entities_PII(item[1])}", list(zip(phrases_5, transcription_entities_pii))))
    
    for phrase in phrases_6 :
        print(phrase)

def run() -> None :
    # How to use batch transcription:
    # https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
    transcription_id = await create_transcription(transcriptionUri);
    WaitForTranscription(transcription_id)
    transcription_url = get_transcription_uri(transcription_id)
    phrases = get_transcription_phrases(transcription_url)
    languages = get_transcription_languages(phrases)
    documents = transcription_phrases_to_documents(phrases, languages)
    sentiments = get_sentiments(documents)
    key_phrases = get_key_phrases(documents)
    entities_general = get_recognized_entities_general(documents)
    entities_pii = get_recognized_entities_PII(documents)
    print_results(phrases, languages, sentiments, key_phrases, entities_general, entities_pii)
    # Clean up resources.
    delete_transcription(transcription_id);
    
run()

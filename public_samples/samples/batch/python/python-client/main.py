from __future__ import print_function
from typing import List

import logging
import sys
import requests
import time
import swagger_client as cris_client


logging.basicConfig(stream=sys.stdout, level=logging.DEBUG, format="%(message)s")

SUBSCRIPTION_KEY = "<your subscription key>"

HOST_NAME = "<your region>.cris.ai"
PORT = 443

NAME = "Simple transcription"
DESCRIPTION = "Simple transcription description"

LOCALE = "en-US"
RECORDINGS_BLOB_URI = "<Your SAS Uri to the recording>"

ADAPTED_ACOUSTIC_ID = None  # guid of a custom acoustic model
ADAPTED_LANGUAGE_ID = None  # guid of a custom language model


def transcribe():
    logging.info("Starting transcription client...")

    # configure API key authorization: subscription_key
    configuration = cris_client.Configuration()
    configuration.api_key['Ocp-Apim-Subscription-Key'] = SUBSCRIPTION_KEY

    # create the client object and authenticate
    client = cris_client.ApiClient(configuration)

    # create an instance of the transcription api class
    transcription_api = cris_client.CustomSpeechTranscriptionsApi(api_client=client)

    # get all transcriptions for the subscription
    transcriptions: List[cris_client.Transcription] = transcription_api.get_transcriptions()

    logging.info("Deleting all existing completed transcriptions.")

    # delete all pre-existing completed transcriptions
    # if transcriptions are still running or not started, they will not be deleted
    for transcription in transcriptions:
        transcription_api.delete_transcription(transcription.id)

    logging.info("Creating transcriptions.")

    # transcription definition using custom models
    transcription_definition = cris_client.TranscriptionDefinition(
        name=NAME, description=DESCRIPTION, locale=LOCALE, recordings_url=RECORDINGS_BLOB_URI,
        models=[cris_client.ModelIdentity(ADAPTED_ACOUSTIC_ID), cris_client.ModelIdentity(ADAPTED_LANGUAGE_ID)]
    )

    # comment out the previous statement and uncomment the following to use base models for transcription
    # transcription_definition = cris_client.TranscriptionDefinition(
    #     name=NAME, description=DESCRIPTION, locale=LOCALE, recordings_url=RECORDINGS_BLOB_URI
    # )

    data, status, headers = transcription_api.create_transcription_with_http_info(transcription_definition)

    # extract transcription location from the headers
    transcription_location: str = headers["location"]

    # get the transcription Id from the location URI
    created_transcriptions = list()
    created_transcriptions.append(transcription_location.split('/')[-1])

    logging.info("Checking status.")

    completed, running, not_started = 0, 0, 0

    while completed < 1:
        # get all transcriptions for the user
        transcriptions: List[cris_client.Transcription] = transcription_api.get_transcriptions()

        # for each transcription in the list we check the status
        for transcription in transcriptions:
            if transcription.status == "Failed" or transcription.status == "Succeeded":
                # we check to see if it was one of the transcriptions we created from this client
                if transcription.id not in created_transcriptions:
                    continue

                completed += 1

                if transcription.status == "Succeeded":
                    results_uri = transcription.results_urls["channel_0"]
                    results = requests.get(results_uri)
                    logging.info("Transcription succeeded. Results: ")
                    logging.info(results.content.decode("utf-8"))
            elif transcription.status == "Running":
                running += 1
            elif transcription.status == "NotStarted":
                not_started += 1

        logging.info(f"Transcriptions status: {completed} completed, {running} running, {not_started} not started yet")
        # wait for 5 seconds
        time.sleep(5)

    input("Press any key...")


def main():
    transcribe()


if __name__ == "__main__":
    main()

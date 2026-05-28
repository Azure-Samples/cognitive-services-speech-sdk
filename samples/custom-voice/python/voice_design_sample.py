#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

"""
Voice design sample.

Provide credentials by editing the constants below, exporting SPEECH_KEY / SPEECH_REGION as
environment variables, or setting them in a VS Code launch.json "env" block.
"""

import json
import os
import sys
import uuid

import azure.cognitiveservices.speech as speechsdk
import requests

# ==================================================================================
# Configuration -- edit these or set the matching environment variables.
# ==================================================================================

SPEECH_KEY = os.environ.get("SPEECH_KEY", "<paste-your-speech-resource-key-here>")
SPEECH_REGION = os.environ.get("SPEECH_REGION", "eastus")

VOICE_PROMPT = "An elderly man in his seventies with a low, bold, strong voice. He speaks slowly and deliberately."
SAMPLE_TEXT = "Hello, I am an AI generated voice. Nice to meet you."
LOCALE = "en-US"
CANDIDATE_COUNT = 3
CANDIDATE_INDEX = 0  # Which candidate to turn into a personal voice (audition the preview WAVs first).

PROJECT_ID = "voice-design-sample-project"
PERSONAL_VOICE_ID = f"voice-design-{uuid.uuid4().hex[:8]}"

SYNTHESIS_TEXT = "This voice was created from a text prompt -- no recording, no consent file, just words."
OUTPUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "voice_design_output")

CUSTOM_VOICE_HOST = f"https://{SPEECH_REGION}.api.cognitive.microsoft.com"
API_VERSION = "2024-02-01-preview"


def _print_response(label: str, response: requests.Response) -> None:
    print(f"--- {label} [{response.status_code}] ---")
    try:
        print(json.dumps(response.json(), indent=2))
    except ValueError:
        print(response.text)
    print()


# Step 1: create the personal-voice project if it doesn't already exist.
def ensure_project(project_id: str) -> None:
    url = f"{CUSTOM_VOICE_HOST}/customvoice/projects/{project_id}?api-version={API_VERSION}"
    response = requests.put(
        url,
        headers={"Ocp-Apim-Subscription-Key": SPEECH_KEY, "Content-Type": "application/json"},
        json={"kind": "PersonalVoice", "description": "voice design sample"},
    )
    response.raise_for_status()
    _print_response(f"PUT project {project_id}", response)


# Step 2: design voice candidates from the prompt.
def design_voice_candidates(voice_prompt: str, sample_text: str, locale: str, candidate_count: int) -> list:
    url = f"{CUSTOM_VOICE_HOST}/customvoice/personalvoices:design?api-version={API_VERSION}"
    body = {
        "VoicePrompt": voice_prompt,
        "SampleText": sample_text,
        "candidateCount": candidate_count,
        "locale": locale,
    }
    response = requests.post(
        url,
        headers={"Ocp-Apim-Subscription-Key": SPEECH_KEY, "Content-Type": "application/json"},
        json=body,
    )
    response.raise_for_status()
    _print_response("POST personalvoices:design", response)
    candidates = response.json().get("candidates", [])
    if not candidates:
        raise RuntimeError("Service returned no candidates. Try a more descriptive VOICE_PROMPT.")
    return candidates


# Step 3: download each candidate's preview WAV. The uri is a short-lived SAS URL.
def download_previews(candidates: list, output_dir: str) -> None:
    os.makedirs(output_dir, exist_ok=True)
    for i, candidate in enumerate(candidates):
        path = os.path.join(output_dir, f"candidate_{i}.wav")
        with requests.get(candidate["uri"], stream=True) as r:
            r.raise_for_status()
            with open(path, "wb") as f:
                for chunk in r.iter_content(chunk_size=64 * 1024):
                    f.write(chunk)
        print(f"  candidate {i}: id={candidate['id']}  preview -> {path}")
    print()


# Step 4: turn the chosen candidate into a personal voice and return its speakerProfileId.
def create_personal_voice(project_id: str, personal_voice_id: str, candidate_id: str) -> str:
    url = f"{CUSTOM_VOICE_HOST}/customvoice/personalvoices/{personal_voice_id}?api-version={API_VERSION}"
    response = requests.put(
        url,
        headers={"Ocp-Apim-Subscription-Key": SPEECH_KEY, "Content-Type": "application/json"},
        json={"projectId": project_id, "candidateId": candidate_id},
    )
    response.raise_for_status()
    _print_response(f"PUT personalvoices/{personal_voice_id}", response)
    return response.json()["speakerProfileId"]


# Step 5: synthesize speech using the new personal voice. Identical to any other personal voice.
def synthesize(speaker_profile_id: str, text: str, output_path: str) -> None:
    ssml = (
        "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' "
        "xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'>"
        "<voice name='DragonLatestNeural'>"
        f"<mstts:ttsembedding speakerProfileId='{speaker_profile_id}'>"
        f"<lang xml:lang='{LOCALE}'>{text}</lang>"
        "</mstts:ttsembedding></voice></speak>"
    )

    speech_config = speechsdk.SpeechConfig(subscription=SPEECH_KEY, region=SPEECH_REGION)
    speech_config.set_speech_synthesis_output_format(speechsdk.SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm)
    audio_config = speechsdk.audio.AudioOutputConfig(filename=output_path)
    synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config, audio_config=audio_config)

    result = synthesizer.speak_ssml_async(ssml).get()
    if result.reason == speechsdk.ResultReason.SynthesizingAudioCompleted:
        print(f"Synthesized audio -> {output_path}")
    elif result.reason == speechsdk.ResultReason.Canceled:
        details = result.cancellation_details
        raise RuntimeError(f"Synthesis canceled: {details.reason}. {details.error_details}")


def main() -> int:
    if SPEECH_KEY.startswith("<"):
        print("ERROR: SPEECH_KEY is not set. Edit the constant at the top of this file or set the SPEECH_KEY env var.")
        return 1

    ensure_project(PROJECT_ID)

    candidates = design_voice_candidates(VOICE_PROMPT, SAMPLE_TEXT, LOCALE, CANDIDATE_COUNT)
    download_previews(candidates, OUTPUT_DIR)

    if not 0 <= CANDIDATE_INDEX < len(candidates):
        print(f"ERROR: CANDIDATE_INDEX={CANDIDATE_INDEX} is out of range (got {len(candidates)} candidates).")
        return 1
    chosen = candidates[CANDIDATE_INDEX]

    speaker_profile_id = create_personal_voice(PROJECT_ID, PERSONAL_VOICE_ID, chosen["id"])

    output_path = os.path.join(OUTPUT_DIR, "voice_design_output.wav")
    synthesize(speaker_profile_id, SYNTHESIS_TEXT, output_path)
    return 0


if __name__ == "__main__":
    sys.exit(main())

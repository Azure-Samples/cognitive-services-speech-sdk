# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import logging
import uuid
from multiprocessing.pool import ThreadPool
from pathlib import Path
from typing import List, Tuple

import azure.cognitiveservices.speech as speechsdk
from azure.storage.blob import ContainerClient
from tqdm import tqdm

LOG = logging.getLogger(__name__)


class VoiceChanger:
    def __init__(
        self,
        subscription: str,
        region: str,
        container_url_with_sas: str,
        language: str = "zh-CN",
        name: str = "zh-CN-XiaoxiaoNeural",
        parallel_threads: int = 8,
    ) -> None:
        self.subscription = subscription
        self.region = region
        self.language = language
        self.name = name
        self.parallel_threads = parallel_threads
        self._create_ssml_string()
        self._create_container_client(container_url_with_sas)

    def _create_ssml_string(self) -> None:
        self.ssml_string = (
            "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis'"
            " xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='{language}'>"
            "<voice name='{name}'><mstts:voiceconversion url='{url}'/></voice></speak> "
        )
        LOG.debug(
            f"SSML related config: language={self.language}, name={self.name}\n\t" f"ssml_string={self.ssml_string}"
        )

    def _create_container_client(self, container_url_with_sas: str) -> None:
        logging.getLogger("azure.core").setLevel(logging.WARNING)
        # Instantiate a new ContainerClient using a sas url
        self.container_client: ContainerClient = ContainerClient.from_container_url(container_url_with_sas)
        self.container_url = container_url_with_sas[: container_url_with_sas.find("?")]
        LOG.debug(f"Azure Storage ContainerClient created: url = {self.container_url} ")

    def _upload_blob(self, local_path: Path, blob_path: str) -> None:
        with local_path.open("rb") as f:
            self.container_client.upload_blob(name=blob_path, data=f, length=local_path.stat().st_size)

    def _delete_blob(self, blob_path: str) -> None:
        self.container_client.delete_blob(blob_path)

    def _create_synthesizer(self) -> speechsdk.SpeechSynthesizer:
        # Create an instance of a speech config with specified subscription key and service region.
        speech_config = speechsdk.SpeechConfig(subscription=self.subscription, region=self.region)
        speech_config.set_speech_synthesis_output_format(speechsdk.SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm)
        # Instantiate a new SpeechSynthesizer
        synthesizer = speechsdk.SpeechSynthesizer(speech_config=speech_config, audio_config=None)
        LOG.debug("SpeechSynthesizer created")
        return synthesizer

    def convert_one_audio(self, audio_pair: Tuple[Path, Path]) -> None:
        input_audio, output_audio = audio_pair
        synthesizer = self._create_synthesizer()
        blob_path = f"{uuid.uuid4()}.wav"
        ssml_args = {"language": self.language, "name": self.name, "url": f"{self.container_url}/{blob_path}"}
        ssml_string = self.ssml_string.format(**ssml_args)
        self._upload_blob(input_audio, blob_path)
        vc_succeed = False
        for _ in range(3):  # retry count
            result = synthesizer.speak_ssml(ssml_string)
            if result.reason == speechsdk.ResultReason.SynthesizingAudioCompleted:
                LOG.info("VC succeed on wav: {}".format(input_audio))
                stream = speechsdk.AudioDataStream(result)
                stream.save_to_wav_file(str(output_audio))
                vc_succeed = True
                break
            elif result.reason == speechsdk.ResultReason.Canceled:
                cancellation_details = result.cancellation_details
                LOG.warning(
                    "VC canceled on wav: {}, error details: {}".format(input_audio, cancellation_details.error_details)
                )
                if cancellation_details.error_code in [
                    speechsdk.CancellationErrorCode.ConnectionFailure,
                    speechsdk.CancellationErrorCode.ServiceUnavailable,
                    speechsdk.CancellationErrorCode.ServiceTimeout,
                ]:
                    LOG.info("Synthesis canceled with connection failure, retrying for wav({}).".format(input_audio))
                    continue
                break
        if not vc_succeed:
            LOG.error("VC failed on wav: {}".format(input_audio))
        self._delete_blob(blob_path)

    def convert_audios(self, input_audio_list: List[Path], output_audio_list: List[Path]) -> None:
        assert len(input_audio_list) == len(output_audio_list)
        with ThreadPool(processes=self.parallel_threads) as pool:
            list(
                tqdm(
                    pool.imap_unordered(
                        self.convert_one_audio,
                        zip(input_audio_list, output_audio_list),
                    ),
                    total=len(input_audio_list),
                )
            )

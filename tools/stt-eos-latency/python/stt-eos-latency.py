"""Measure end-of-speech latency for multi-segment WAV recordings.

The tool streams PCM WAV audio to Azure AI Speech at real-time pace and writes
one ``<wav>.gen1.csv`` file per input. Empty and NoMatch results are omitted.

Prerequisites:
    * Python 3.10 or later.
        * ``pip install azure-cognitiveservices-speech``.
        * An Azure Speech resource key and region.
    * Input audio must be an uncompressed PCM WAV file. The WAV header supplies
      the sample rate, sample width, and channel count.

Configure cloud credentials (PowerShell):
    $env:SPEECH_KEY = "<speech-resource-key>"
    $env:SPEECH_REGION = "<speech-resource-region>"

Configure cloud credentials (bash):
    export SPEECH_KEY="<speech-resource-key>"
    export SPEECH_REGION="<speech-resource-region>"

Examples:
    # Display all options.
    python stt-eos-latency.py --help

    # Evaluate one file with fixed-language recognition.
    python stt-eos-latency.py --file "C:\\audio\\call.wav" --locale en-IN

    # Evaluate every WAV directly in a folder with four concurrent streams.
    python stt-eos-latency.py --folder "C:\\audio" --workers 4

    # Use continuous language identification.
    python stt-eos-latency.py --file "C:\\audio\\call.wav" \
        --locale en-IN hi-IN

Output columns:
    offset_seconds      Recognized segment start in the audio.
    text                Display-form recognition text.
    confidence_score    Confidence of the top hypothesis.
    transcription_time  Audio position when the final result arrived.
    latency             transcription_time - offset_seconds.
    eos_seconds         End of the last recognized word.
    eos_latency         transcription_time - eos_seconds.
    eos_source          ``word`` or ``fallback`` when words are unavailable.

Times are wall-clock seconds from session start while the audio is paced in
real time.
"""

from __future__ import annotations

import argparse
import csv
import glob
import json
import os
import threading
import time
import wave
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    import azure.cognitiveservices.speech as speechsdk

# --- Defaults (override with env vars) --------------------------------------
DEFAULT_REGION = os.environ.get("SPEECH_REGION")
DEFAULT_KEY = os.environ.get("SPEECH_KEY")
DEFAULT_LOCALE = "en-IN"
CHUNK_MS = 100  # real-time pacing chunk size
TRAILING_SILENCE_SEC = 3.0  # flush the final segment

TICKS_PER_SEC = 1e7  # SDK offsets/durations are in 100-ns ticks

CSV_COLUMNS = [
    "offset_seconds",
    "text",
    "confidence_score",
    "transcription_time",
    "latency",
    "eos_seconds",
    "eos_latency",
    "eos_source",
]


def _segment_eos_seconds(result: speechsdk.SpeechRecognitionResult) -> tuple[float, float, str]:
    """Return (eos_seconds, confidence, eos_source) from the detailed JSON result.

    EOS = last word Offset+Duration (true end of speech, eos_source="word").
    Falls back to result.offset+result.duration when no word list is available
    (eos_source="fallback").
    """
    confidence = 0.0
    eos_ticks = result.offset + result.duration  # fallback
    eos_source = "fallback"
    raw = result.properties.get(
        speechsdk.PropertyId.SpeechServiceResponse_JsonResult
    )
    if raw:
        try:
            js = json.loads(raw)
            nbest = js.get("NBest") or []
            if nbest:
                confidence = float(nbest[0].get("Confidence", 0.0))
                words = nbest[0].get("Words") or []
                if words:
                    last = words[-1]
                    eos_ticks = last["Offset"] + last["Duration"]
                    eos_source = "word"
        except (ValueError, KeyError, TypeError):
            pass
    return eos_ticks / TICKS_PER_SEC, confidence, eos_source


def run_file(
    wav_path: str,
    locales: list[str],
    region: str,
    key: str,
    silence_timeout_ms: int | None = None,
    post_refinement: bool = False,
    segmentation_strategy: str | None = None,
) -> list[dict]:
    """Stream one WAV in real time and return the per-segment rows.

    ``locales`` may hold a single locale (fixed language) or several, in which
    case continuous language identification auto-detects among them.
    ``silence_timeout_ms`` (when set) overrides the segmentation silence
    timeout used for endpointing. ``post_refinement`` (when True) sets
    PostProcessingOption=PostRefinement on the speech config.
    """
    global speechsdk
    try:
        import azure.cognitiveservices.speech as speechsdk
    except ModuleNotFoundError as exc:
        raise RuntimeError(
            "Cloud mode requires the Azure Speech SDK. Install it with "
            "'pip install azure-cognitiveservices-speech'."
        ) from exc

    wf = wave.open(wav_path, "rb")
    sample_rate = wf.getframerate()
    channels = wf.getnchannels()
    sample_width = wf.getsampwidth()

    speech_config = speechsdk.SpeechConfig(subscription=key, region=region)
    # --- word-level timestamps + detailed output (the key change) ---
    speech_config.request_word_level_timestamps()
    speech_config.output_format = speechsdk.OutputFormat.Detailed

    # Single locale -> fixed language; multiple -> continuous language ID.
    auto_cfg = None
    if len(locales) == 1:
        speech_config.speech_recognition_language = locales[0]
    else:
        # Continuous language identification for speech-to-text requires the
        # Speech v2 endpoint (the region/key constructor does not support it).
        v2_endpoint = (
            f"wss://{region}.stt.speech.microsoft.com/speech/universal/v2"
        )
        speech_config = speechsdk.SpeechConfig(subscription=key, endpoint=v2_endpoint)
        speech_config.request_word_level_timestamps()
        speech_config.output_format = speechsdk.OutputFormat.Detailed
        speech_config.set_property(
            speechsdk.PropertyId.SpeechServiceConnection_LanguageIdMode,
            "Continuous",
        )
        auto_cfg = speechsdk.languageconfig.AutoDetectSourceLanguageConfig(
            languages=locales
        )

    # Optional endpointing silence timeout (only applied when provided).
    if silence_timeout_ms is not None:
        speech_config.set_property(
            speechsdk.PropertyId.Speech_SegmentationSilenceTimeoutMs,
            str(silence_timeout_ms),
        )

    # Optional post-processing refinement (only applied when requested).
    if post_refinement:
        speech_config.set_property(
            speechsdk.PropertyId.SpeechServiceResponse_PostProcessingOption,
            "PostRefinement",
        )

    # Optional segmentation strategy (e.g. "Semantic"); only applied when set.
    if segmentation_strategy is not None:
        speech_config.set_property(
            speechsdk.PropertyId.Speech_SegmentationStrategy,
            segmentation_strategy,
        )

    stream_format = speechsdk.audio.AudioStreamFormat(
        samples_per_second=sample_rate,
        bits_per_sample=sample_width * 8,
        channels=channels,
    )
    push_stream = speechsdk.audio.PushAudioInputStream(stream_format)
    audio_config = speechsdk.audio.AudioConfig(stream=push_stream)
    if auto_cfg is not None:
        recognizer = speechsdk.SpeechRecognizer(
            speech_config=speech_config,
            auto_detect_source_language_config=auto_cfg,
            audio_config=audio_config,
        )
    else:
        recognizer = speechsdk.SpeechRecognizer(
            speech_config=speech_config, audio_config=audio_config
        )

    rows: list[dict] = []
    state = {"start_time": time.perf_counter()}
    done = threading.Event()

    def session_started(evt):
        state["start_time"] = time.perf_counter()

    def recognized(evt):
        r = evt.result
        if r.reason != speechsdk.ResultReason.RecognizedSpeech or not r.text.strip():
            return  # skip empty / NoMatch segments
        transcription_time = time.perf_counter() - state["start_time"]
        offset_s = r.offset / TICKS_PER_SEC
        eos_s, confidence, eos_source = _segment_eos_seconds(r)
        rows.append(
            {
                "offset_seconds": round(offset_s, 2),
                "text": r.text,
                "confidence_score": confidence,
                "transcription_time": round(transcription_time, 2),
                "latency": round(transcription_time - offset_s, 2),
                "eos_seconds": round(eos_s, 2),
                "eos_latency": round(transcription_time - eos_s, 2),
                "eos_source": eos_source,
            }
        )

    def stop_cb(evt):
        done.set()

    recognizer.recognized.connect(recognized)
    recognizer.session_started.connect(session_started)
    recognizer.session_stopped.connect(stop_cb)
    recognizer.canceled.connect(stop_cb)

    recognizer.start_continuous_recognition()

    # --- stream at real-time pace using an ABSOLUTE schedule ---
    # Each chunk is released at t0 + (frames_sent / sample_rate). Sleeping to an
    # absolute target (instead of a fixed per-chunk sleep) prevents write
    # overhead and OS timer coarseness from accumulating into cumulative drift.
    chunk_frames = int(sample_rate * CHUNK_MS / 1000)
    frame_bytes = sample_width * channels
    audio_duration = wf.getnframes() / sample_rate
    t0 = time.perf_counter()
    frames_sent = 0
    while True:
        data = wf.readframes(chunk_frames)
        if not data:
            break
        push_stream.write(data)
        frames_sent += len(data) // frame_bytes
        dt = (t0 + frames_sent / sample_rate) - time.perf_counter()
        if dt > 0:
            time.sleep(dt)
    wf.close()

    stream_wall = time.perf_counter() - t0
    drift = stream_wall - audio_duration
    print(
        f"    [pacing] {os.path.basename(wav_path)}: audio={audio_duration:.1f}s "
        f"wall={stream_wall:.1f}s drift={drift:+.2f}s "
        f"({drift / audio_duration * 100:+.2f}%)",
        flush=True,
    )

    # --- trailing silence so the last segment finalizes (absolute pace) ---
    silence = b"\x00" * chunk_frames * frame_bytes
    silence_frames = int(TRAILING_SILENCE_SEC * sample_rate)
    sent_silence = 0
    ts0 = time.perf_counter()
    while sent_silence < silence_frames:
        push_stream.write(silence)
        sent_silence += chunk_frames
        dt = (ts0 + sent_silence / sample_rate) - time.perf_counter()
        if dt > 0:
            time.sleep(dt)

    push_stream.close()
    done.wait(timeout=30)
    recognizer.stop_continuous_recognition()
    return rows


def write_csv(rows: list[dict], out_path: str) -> None:
    with open(out_path, "w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=CSV_COLUMNS, extrasaction="ignore")
        writer.writeheader()
        writer.writerows(rows)


def main() -> None:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        allow_abbrev=False,
    )
    g = parser.add_mutually_exclusive_group(required=True)
    g.add_argument("--file", help="Run a single WAV file.")
    g.add_argument("--folder", help="Run every *.wav directly in a folder.")
    g.add_argument("--files", nargs="+", help="Run an explicit list of WAV files.")
    parser.add_argument(
        "--locale", nargs="+", default=[DEFAULT_LOCALE], metavar="LOCALE",
        help="One locale for a fixed language (e.g. en-IN), or several "
             "(e.g. --locale en-IN hi-IN) to auto-detect via continuous "
             "language identification.",
    )
    parser.add_argument(
        "--region", default=DEFAULT_REGION,
        help="Azure Speech region (default: SPEECH_REGION environment variable).",
    )
    parser.add_argument(
        "--key", default=DEFAULT_KEY,
        help="Azure Speech key (default: SPEECH_KEY environment variable).",
    )
    parser.add_argument(
        "--workers", type=int, default=1,
        help="Number of files to stream concurrently (each real-time).",
    )
    parser.add_argument(
        "--silence-timeout-ms", type=int, default=None,
        help="Segmentation silence timeout in ms (endpointing). Lower values "
             "finalize segments sooner. Only applied when provided.",
    )
    parser.add_argument(
        "--post-refinement", action="store_true",
        help='Set PostProcessingOption=PostRefinement on the speech config.',
    )
    parser.add_argument(
        "--segmentation-strategy", default=None,
        help='Set Speech_SegmentationStrategy (e.g. "Semantic"). Only applied when set.',
    )
    parser.add_argument(
        "--suffix",
        help="Override the output CSV suffix (e.g. '.gen2.csv'). "
             "Default: .gen1.csv.",
    )
    args = parser.parse_args()

    if args.workers < 1:
        parser.error("--workers must be at least 1")
    if args.silence_timeout_ms is not None and args.silence_timeout_ms < 1:
        parser.error("--silence-timeout-ms must be at least 1")
    if not args.key:
        parser.error("requires --key or the SPEECH_KEY environment variable")
    if not args.region:
        parser.error(
            "requires --region or the SPEECH_REGION environment variable"
        )

    if args.file:
        wavs = [args.file]
    elif args.files:
        wavs = args.files
    else:
        wavs = sorted(glob.glob(os.path.join(args.folder, "*.wav")))

    if not wavs:
        parser.error("no WAV files found")

    failures: list[str] = []

    def process(idx_wav):
        i, wav_path = idx_wav
        name = os.path.basename(wav_path)
        print(f"[{i}/{len(wavs)}] start {name}", flush=True)
        try:
            rows = run_file(
                wav_path, args.locale, args.region, args.key,
                args.silence_timeout_ms, args.post_refinement,
                args.segmentation_strategy,
            )
        except Exception as exc:  # noqa: BLE001 - keep the batch going
            print(f"    ERROR {name}: {exc}", flush=True)
            failures.append(name)
            return
        out_path = wav_path + (args.suffix or ".gen1.csv")
        write_csv(rows, out_path)
        if rows:
            lat = [
                r["eos_latency"]
                for r in rows
                if isinstance(r["eos_latency"], (int, float))
            ]
            if lat:
                avg = sum(lat) / len(lat)
                print(
                    f"    done {name}: {len(rows)} segments, "
                    f"mean eos_latency {avg:.2f}s",
                    flush=True,
                )
        else:
            print(f"    done {name}: 0 segments", flush=True)

    items = list(enumerate(wavs, 1))
    if args.workers <= 1:
        for item in items:
            process(item)
    else:
        with ThreadPoolExecutor(max_workers=args.workers) as pool:
            futures = [pool.submit(process, item) for item in items]
            for _ in as_completed(futures):
                pass

    if failures:
        print(f"Failed to process {len(failures)} of {len(wavs)} file(s).")
        raise SystemExit(1)


if __name__ == "__main__":
    main()

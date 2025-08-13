import json
import sys
import time
import wave

try:
    import azure.cognitiveservices.speech as speechsdk
except ImportError:
    print(
        """
    Importing the Speech SDK for Python failed.
    Refer to
    https://docs.microsoft.com/azure/cognitive-services/speech-service/quickstart-python for
    installation instructions.
    """
    )
    sys.exit(1)


def read_wave_header(file_path):
    with wave.open(file_path, "rb") as audio_file:
        framerate = audio_file.getframerate()
        bits_per_sample = audio_file.getsampwidth() * 8
        num_channels = audio_file.getnchannels()
        return framerate, bits_per_sample, num_channels


def push_stream_writer(stream, file_name):
    # The number of bytes to push per buffer
    n_bytes = 3200
    wav_fh = wave.open(file_name, "rb")
    # Start pushing data until all data has been read from the file
    try:
        while True:
            frames = wav_fh.readframes(n_bytes // 2)
            print(f"read {len(frames)} bytes")
            if not frames:
                break
            stream.write(frames)
            time.sleep(0.1)
    finally:
        wav_fh.close()
        stream.close()  # must be done to signal the end of stream


def convert_reference_words(reference_text, reference_words):
    dictionary = list(set(reference_words))
    max_length = max([len(word) for word in dictionary])

    # From left to right to do the longest matching to get the word segmentation
    def left_to_right_segmentation(text, dictionary):
        result = list()
        while len(text) > 0:
            sub_text = ""
            # If the length of the text is less than the max_length, then the sub_text is the text itself
            if len(text) < max_length:
                sub_text = text
            else:
                sub_text = text[:max_length]
            while len(sub_text) > 0:
                # If the sub_text is in the dictionary or the length of the sub_text is 1, then add it to the result
                if (sub_text in dictionary) or (len(sub_text) == 1):
                    result.append(sub_text)
                    # Remove the sub_text from the text
                    text = text[len(sub_text):]
                    break
                else:
                    # If the sub_text is not in the dictionary, then remove the last character of the sub_text
                    sub_text = sub_text[: len(sub_text) - 1]
        return result

    # From right to left to do the longest matching to get the word segmentation
    def right_to_left_segmentation(text, dictionary):
        result = list()
        while len(text) > 0:
            sub_text = ""
            # If the length of the text is less than the max_length, then the sub_text is the text itself
            if len(text) < max_length:
                sub_text = text
            else:
                sub_text = text[-max_length:]
            while len(sub_text) > 0:
                # If the sub_text is in the dictionary or the length of the sub_text is 1, then add it to the result
                if (sub_text in dictionary) or (len(sub_text) == 1):
                    result.append(sub_text)
                    # Remove the sub_text from the text
                    text = text[: -len(sub_text)]
                    break
                else:
                    # If the sub_text is not in the dictionary, then remove the first character of the sub_text
                    sub_text = sub_text[1:]
        # Reverse the result to get the correct order
        result = result[::-1]
        return result

    def segment_word(reference_text, dictionary):
        left_to_right = left_to_right_segmentation(reference_text, dictionary)
        right_to_left = right_to_left_segmentation(reference_text, dictionary)
        if "".join(left_to_right) == reference_text:
            return left_to_right
        elif "".join(right_to_left) == reference_text:
            return right_to_left
        else:
            print("WW failed to segment the text with the dictionary")
            if len(left_to_right) < len(right_to_left):
                return left_to_right
            elif len(left_to_right) > len(right_to_left):
                return right_to_left
            else:
                # If the word number is the same, then return the one with the smallest single word
                left_to_right_single = len([word for word in left_to_right if len(word) == 1])
                right_to_left_single = len([word for word in right_to_left if len(word) == 1])
                if left_to_right_single < right_to_left_single:
                    return left_to_right
                else:
                    return right_to_left

    # Remove punctuation from the reference text
    reference_text = "".join([char for char in reference_text if char.isalnum() or char.isspace()])
    return segment_word(reference_text, dictionary)


def get_reference_words(wave_filename, reference_text, language, speech_key, speech_endpoint):
    audio_config = speechsdk.audio.AudioConfig(filename=wave_filename)
    speech_config = speechsdk.SpeechConfig(subscription=speech_key, endpoint=speech_endpoint)
    speech_recognizer = speechsdk.SpeechRecognizer(
        speech_config=speech_config, language=language, audio_config=audio_config
    )

    pronunciation_config = speechsdk.PronunciationAssessmentConfig(
        reference_text=reference_text,
        grading_system=speechsdk.PronunciationAssessmentGradingSystem.FivePoint,
        granularity=speechsdk.PronunciationAssessmentGranularity.Word,
        enable_miscue=True,
    )

    # Apply pronunciation assessment config to speech recognizer
    pronunciation_config.apply_to(speech_recognizer)
    result = speech_recognizer.recognize_once_async().get()

    if result.reason == speechsdk.ResultReason.RecognizedSpeech:
        pronunciation_result = json.loads(result.properties.get(speechsdk.PropertyId.SpeechServiceResponse_JsonResult))
        reference_words = []
        nb = pronunciation_result["NBest"][0]
        for idx, word in enumerate(nb["Words"]):
            if word["PronunciationAssessment"]["ErrorType"] != "Insertion":
                reference_words.append(word["Word"])
        return convert_reference_words(reference_text, reference_words)
    elif result.reason == speechsdk.ResultReason.NoMatch:
        print("No speech could be recognized")
        return None
    elif result.reason == speechsdk.ResultReason.Canceled:
        cancellation_details = result.cancellation_details
        print(f"Speech Recognition canceled: {cancellation_details.reason}")
        if cancellation_details.reason == speechsdk.CancellationReason.Error:
            print(f"Error details: {cancellation_details.error_details}")
        return None


# Aligns two token lists using SequenceMatcher and handles differences.
# Equal segments are copied directly.
# 'Replace' segments are aligned strictly if identical after joining,
# otherwise aligned using align_raw_tokens_by_ref().
# 'Delete' segments from raw are preserved.
def align_lists_with_diff_handling(raw, ref):
    from difflib import SequenceMatcher

    aligned_raw = []

    sm = SequenceMatcher(None, raw, ref)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == 'equal':
            aligned_raw.extend(raw[i1:i2])
        elif tag == 'replace':
            # Strict comparison
            if "".join(raw[i1:i2]) == "".join(ref[j1:j2]):
                aligned_raw.extend(ref[j1:j2])
            else:
                aligned_part = align_raw_tokens_by_ref(raw[i1:i2], ref[j1:j2])
                aligned_raw.extend(aligned_part)
        elif tag == 'delete':
            aligned_raw.extend(raw[i1:i2])
    return aligned_raw


# Aligns raw_list tokens to ref_list
# by merging consecutive tokens and splitting them
# when a reference word is found inside the merged string.
def align_raw_tokens_by_ref(raw_list, ref_list):
    ref_idx = 0
    raw_idx = 0
    ref_len = len(ref_list)
    aligned_raw = []

    # Use a copy to avoid modifying the original list.
    raw_copy = list(raw_list)

    while raw_idx < len(raw_copy) and ref_idx < ref_len:
        merged_split_done = False
        for length in range(1, len(raw_copy) + 1):
            if raw_idx + length > len(raw_copy):
                break
            merged_raw = ''.join(raw_copy[raw_idx:raw_idx + length])
            ref_word = ref_list[ref_idx]

            if ref_word in merged_raw:
                parts = merged_raw.split(ref_word, 1)

                # Handle prefix part before ref_word
                if parts[0]:
                    aligned_raw.append(parts[0])

                # Append the matched ref_word
                aligned_raw.append(ref_word)

                # Handle suffix part after ref_word
                if parts[1]:
                    raw_copy[raw_idx] = parts[1]
                    # Remove the extra merged tokens
                    for _ in range(1, length):
                        raw_copy.pop(raw_idx + 1)
                else:
                    # No suffix: remove all merged tokens
                    for _ in range(length):
                        raw_copy.pop(raw_idx)

                ref_idx += 1
                merged_split_done = True

            if merged_split_done:
                break

            # If no match after merging all tokens,
            # align current token directly
            if length == len(raw_copy):
                aligned_raw.append(raw_copy[raw_idx])
                raw_idx += 1
                ref_idx += 1

    # Append any remaining raw tokens
    while raw_idx < len(raw_copy):
        aligned_raw.append(raw_copy[raw_idx])
        raw_idx += 1

    return aligned_raw


# A common wave header, with zero audio length
# Since stream data doesn't contain header, but the API requires header to fetch format information,
# so you need post this header as first chunk for each query
WaveHeader16K16BitMono = bytes(
    [
        82,
        73,
        70,
        70,
        78,
        128,
        0,
        0,
        87,
        65,
        86,
        69,
        102,
        109,
        116,
        32,
        18,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        128,
        62,
        0,
        0,
        0,
        125,
        0,
        0,
        2,
        0,
        16,
        0,
        0,
        0,
        100,
        97,
        116,
        97,
        0,
        0,
        0,
        0,
    ]
)

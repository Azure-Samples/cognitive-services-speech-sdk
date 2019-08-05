//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream> // cin, cout
#include <speechapi_cxx.h>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

static void* OpenCompressedFile(const std::string& compressedFileName)
{
    FILE *filep = NULL;
    filep = fopen(compressedFileName.c_str(), "rb");
    return filep;
}

static void closeStream(void* fp)
{
    if (fp != NULL)
    {
        fclose((FILE*)fp);
    }
}

static int ReadCompressedBinaryData(void *stream, uint8_t *ptr, uint32_t bufSize)
{
    FILE* compressedStreamfptr = (FILE*)stream;
    if (compressedStreamfptr != NULL && !feof(compressedStreamfptr))
    {
        return fread(ptr, 1, bufSize, compressedStreamfptr);
    }
    else
    {
        return 0;
    }
}

void recognizeSpeech(const std::string& compressedFileName)
{
    std::shared_ptr<SpeechRecognizer> recognizer;
    std::shared_ptr<PullAudioInputStream> pullAudioStream;

    void *compressedFilePtr = OpenCompressedFile(compressedFileName);

    if (compressedFilePtr == NULL)
    {
        std::cout << "Error: Input file doesn't exist" << std::endl;
        return;
    }

    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    AudioStreamContainerFormat inputFormat;

    if (compressedFileName.find(".mp3") == (compressedFileName.size() - 4))
    {
        inputFormat = AudioStreamContainerFormat::MP3;
    }
    else if (compressedFileName.find(".opus") == (compressedFileName.size() - 5))
    {
        inputFormat = AudioStreamContainerFormat::OGG_OPUS;
    }
    else if (compressedFileName.find(".alaw") == (compressedFileName.size() - 5))
    {
        inputFormat = AudioStreamContainerFormat::ALAW;
    }
    else if (compressedFileName.find(".mulaw") == (compressedFileName.size() - 6))
    {
        inputFormat = AudioStreamContainerFormat::MULAW;
    }
    else if (compressedFileName.find(".flac") == (compressedFileName.size() - 5))
    {
        inputFormat = AudioStreamContainerFormat::FLAC;
    }
    else
    {
        std::cout << "Only Opus and MP3 input files are currently supported" << std::endl;
        return;
    }

    pullAudioStream = AudioInputStream::CreatePullStream(
        AudioStreamFormat::GetCompressedFormat(inputFormat),
        compressedFilePtr,
        ReadCompressedBinaryData,
        closeStream
    );
    recognizer = SpeechRecognizer::FromConfig(config, AudioConfig::FromStreamInput(pullAudioStream));

    std::cout << "Recognizing ..." << std::endl;

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of 15
    // seconds of audio is processed.  The task returns the recognition text as result. 
    // Note: Since RecognizeOnceAsync() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query. 
    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeOnceAsync().get();

    // Checks result.
    if (result->Reason == ResultReason::RecognizedSpeech) {
        std::cout << "We recognized: " << result->Text << std::endl;
    }
    else if (result->Reason == ResultReason::NoMatch) {
        std::cout << "NOMATCH: Speech could not be recognized." << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled) {
        auto cancellation = CancellationDetails::FromResult(result);
        std::cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

        if (cancellation->Reason == CancellationReason::Error) {
            std::cout << "CANCELED: ErrorCode= " << (int)cancellation->ErrorCode << std::endl;
            std::cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            std::cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        std::cout << "Usage: ./compressed-audio-input <filename>" << std::endl;
        return 0;
    }
    setlocale(LC_ALL, "");
    recognizeSpeech(argv[1]);
    return 0;
}

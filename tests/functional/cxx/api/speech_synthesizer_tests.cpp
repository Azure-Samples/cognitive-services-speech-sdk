//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <memory.h>
#include "test_utils.h"
#include "file_utils.h"
#include "synthesizer_utils.h"

using namespace TTS;

//   Audio can come out of a synthesizer at several different "scopes", and can be used in several
//   different scenarios. The text and samples below are my attempt to illustrate both scopes and
//   scenarios.
//
//       (1) SYNTH OUTPUT - You can obtain all the audio data from all SPEAK requests,
//           all together, in a single data set, concatenated, using AudioConfig when
//           creating the SpeechSynthesizer via ::FromConfig.
//
//           Similar to Recognition, the output AudioConfig can be of several types (you must pick one of a-e):
//
//           (a) Output device rendering (e.g. AudioConfig::FromDefaultSpeakerOutput())
//               NOTE: The audio data isn't "technically" available to the developer customer
//                     in this "output device" configuration, but it is "available" to the
//                     end user customer by being rendering automatically on behalf of the 
//                     developer customer.
//
//           (b) Audio file output (e.g. AudioConfig::FromWavFileOutput(filename))
//
//           (c) Audio stream output (e.g. AudioConfig::FromStreamOutput(stream))
//               (i)  stream = AudioOutputStream::CreatePullStream(optional format)
//               (ii) stream = AudioOutputStream::CreatePushStream(..., optional format), where
//                    Push callbacks can be:
//                    - "C" functions with context
//                    - std::functions (w/capturing lambdas, etc.)
//                    - custom class derived from PushAudioOutputStreamCallback
//
//           (d) NULL AudioConfig (e.g. std::shared_ptr<AudioConfig>(nullptr))
//               NOTE: In this case we'll do nothing with aggregated SYNTH output. Audio data
//                     will still be available via the mechanisms below (#2, #3, #4), as they are
//                     available for all scenarios (1a - 1e)
//
//           (e) No AudioConfig specified when calling SpeechSynthesizer::FromConfig
//               NOTE: We can treat this in one of two ways:
//                     (i) Same as AudioConfig::FromDefaultSpeakerOutput(); this is symmetric with speech recognition
//                     (ii) Do nothing with aggregated synthesizer output
//
//       (2) SPEAK OUTPUT - You can obtain all the audio data for a single SPEAK via a "completed"
//           SpeechSynthesisResult (e.g. result->Reason == ResultReason::SynthesisStopped)
//           NOTE: The audio data isn't availble to the developer customer until AFTER the synthesis
//                 completed successfully. This is appropriate for many scenarios, but for interactive
//                 scenarios it's best to use a different pattern (#1 above, #3 or #4 below).
//
//           SpeechSynthesisResults of this kind can be obtained 3 ways:
//           (a) synthesizer->SynthesisStopped += [](const SpeechSynthesisEventArgs& e), via e.Result
//           (b) result = synthesizer->SpeakTextAsync("{{{text}}}").get()
//           (c) result = synthesizer->SpeakSsmlAsync("{{{ssml}}}").get()
//
//           After obtaining a result of this kind, result->GetAudioLength() bytes of data will be available 
//           using result->GetAudioData() (std::shared_ptr<std::vector<uint8_t>>.
//
//       (3) AUDIO CHUNK - You can obtain audio data for each individual chunk of synthesized audio data via
//           an "in progress" SpeechSynthesisResult (e.g. result->Reason == ResultReason::Synthesizing)
//
//           SpeechSynthesisResults of this kind can be obtained by connecting to the Synthesizing event:
//           synthesizer->Synthesizing += [](const SpeechSynthesisEventArgs& e), via e.Result
//
//           After obtaining a result of this kind, result->GetAudioLength() bytes of data will be available 
//           using result->GetAudioData() (which is of type std::shared_ptr<std::vector<uint8_t>>).
//
//       (4) AUDIO DATA STREAM - You can obtain an AudioDataStream object representing a single SPEAK
//           request BEFORE or AFTER synthesis has completed by passing an SpeechSynthesisResult object to
//           the AudioDataStream::FromResult(result) method.
//
//           SpeechSynthesisResults can be obtained in many ways:
//
//           (a) result = synthesizer->StartSpeakingTextAsync("{{{text}}}").get()
//               NOTE: For these result->Reason == ResultReason::SynthesisStarted results, the
//                     AudioDataStream obtained via AudioDataStream::FromResult(result) represents
//                     ALL audio data for the SPEAK request associated with the result.
//
//           (b) result = synthesizer->StartSpeakingSsmlAsync("{{{ssml}}}").get()
//               NOTE: For these result->Reason == ResultReason::SynthesisStarted results, the
//                     AudioDataStream obtained via AudioDataStream::FromResult(result) represents
//                     ALL audio data for the SPEAK request associated with the result.
//
//           (c) synthesizer->SynthesisStarted += [](const SpeechSynthesisEventArgs& e), via e.Result
//               NOTE: For these e.Result->Reason == ResultReason::SynthesisStarted results, the
//                     AudioDataStream obtained via AudioDataStream::FromResult(e.Result) represents
//                     ALL audio data for the SPEAK request associated with the result.
//
//           (d) synthesizer->Synthesizing += [](const SpeechSynthesisEventArgs& e), via e.Result
//               NOTE: For these e.Result->Reason == ResultReason::Synthesizing results, the
//                     AudioDataStream obtained via AudioDataStream::FromResult(e.Result) represents
//                     audio data for the SPEAK request associated with the result starting
//                     with this audio data chunk.
//
//                     IMPORTANT: It does NOT contain the audio data for any audio chunks already
//                     provided via Synthesizing events for this SPEAK request prior to this event.
//
//           (e) synthesizer->SynthesisStopped += [](const SpeechSynthesisEventArgs& e), via e.Result
//               NOTE: For these e.Result->Reason == ResultReason::SynthesisStopped results, the
//                     AudioDataStream obtained via AudioDataStream::FromResult(e.Result) represents
//                     ALL audio data for the SPEAK request.
//
//                     IMPORTANT: e.Result->GetAudioData() is an alternative method to obtain the same
//                     audio data as AudioDataStream::FromResult(e.Result) in this scenario.
//
//           (f) result = synthesizer->SpeakTextAsync("{{{text}}}").get()
//               NOTE: For these e.Result->Reason == ResultReason::SynthesisStopped results, the
//                     AudioDataStream obtained via AudioDataStream::FromResult(e.Result) represents
//                     ALL audio data for the SPEAK request.
//
//                     IMPORTANT: e.Result->GetAudioData() is an alternative method to obtain the same
//                     audio data as AudioDataStream::FromResult(e.Result) in this scenario.
//
//           (g) result = synthesizer->SpeakSsmlAsync("{{{ssml}}}").get()
//               NOTE: For these e.Result->Reason == ResultReason::SynthesisStopped results, the
//                     AudioDataStream obtained via AudioDataStream::FromResult(e.Result) represents
//                     ALL audio data for the SPEAK request.
//
//                     IMPORTANT: e.Result->GetAudioData() is an alternative method to obtain the same
//                     audio data as AudioDataStream::FromResult(e.Result) in this scenario.
//
//           Once an AudioDataStream object is obtained, developer customers can access the audio data
//           and the state of the stream via several methods and properties:
//           (i)   audioDataStream->Status (e.g. NoData, PartialData, AllData, Canceled)
//           (ii)  bytesAvailable = audioDataStream->CanReadData(bytesRequested)
//           (iii) bytesAvailable = audioDataStream->CanReadData(pos, bytesRequested)
//           (iv)  bytesRead = audioDataStream->ReadData(destinationBuffer, bytesRequested)
//           (v)   bytesRead = audioDataStream->ReadData(pos, destinationBuffer, bytesRequested)
//           (vi)  pos = audioDataStream->GetPosition()
//           (vii) audioDataStream->SetPosition(pos)

/* Test cases based on cognitive service TTS, without output audio content check */

TEST_CASE("Defaults", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has now completed rendering to default speakers */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has now completed rendering to default speakers */
}

TEST_CASE("Explicitly use default speakers", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto deviceConfig = AudioConfig::FromDefaultSpeakerOutput();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, deviceConfig);

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has now completed rendering to default speakers */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has now completed rendering to default speakers */
}

TEST_CASE("Pick language", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    config->SetSpeechSynthesisLanguage("en-GB");
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has now completed rendering to default speakers */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has now completed rendering to default speakers */
}

TEST_CASE("Pick voice", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    config->SetSpeechSynthesisVoiceName("Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has now completed rendering to default speakers */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has now completed rendering to default speakers */
}

TEST_CASE("Synthesizer output to file", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto fileConfig = AudioConfig::FromWavFileOutput("wavefile.wav");

    auto synthesizer = SpeechSynthesizer::FromConfig(config, fileConfig);
    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"" */
    synthesizer = nullptr; /* "{{{wavefile.wav}}}" is now closed */

    auto waveSize1 = GetFileSize("wavefile.wav");
    if (result1->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(waveSize1 > EMPTY_WAVE_FILE_SIZE);
        ifstream file("wavefile.wav", ios::in | ios::binary | ios::ate);
        SPXTEST_REQUIRE(file.is_open());

        char headerBlock[EMPTY_WAVE_FILE_SIZE];
        file.seekg(0, ios::beg);
        file.read(headerBlock, EMPTY_WAVE_FILE_SIZE);
        file.close();
        SPXTEST_REQUIRE(0 == std::memcmp(headerBlock, "RIFF", 4));
    }

    synthesizer = SpeechSynthesizer::FromConfig(config, fileConfig);
    result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"" */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{wavefile.wav}}}" now contains synthesized audio for both "{{{text1}}}"" and "{{{text2}}}" */
    synthesizer = nullptr; /* "{{{wavefile.wav}}}" is now closed */

    auto waveSize2 = GetFileSize("wavefile.wav");
    if (result1->Reason != ResultReason::Canceled && result2->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(waveSize2 > waveSize1);
    }
}

TEST_CASE("Synthesizer output to push stream", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();

    auto callback = std::make_shared<PushAudioOutputStreamTestCallback>();
    auto stream = AudioOutputStream::CreatePushStream(callback);

    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering to pushstream */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering to pushstream */

    bool canceled = result1->Reason == ResultReason::Canceled && result2->Reason == ResultReason::Canceled;

    result1 = nullptr;
    result2 = nullptr;
    synthesizer = nullptr;

    if (!canceled)
    {
        SPXTEST_REQUIRE(callback->GetAudioSize() > 0);
        SPXTEST_REQUIRE(callback->IsClosed());
    }
    else
    {
        SPXTEST_REQUIRE(callback->GetAudioSize() == 0);
        SPXTEST_REQUIRE(callback->IsClosed());
    }
}

TEST_CASE("Synthesizer output to pull stream use after synthesis completed", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto stream = AudioOutputStream::CreatePullStream();
    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering to pullstream */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering to pullstream */

    bool canceled = result1->Reason == ResultReason::Canceled && result2->Reason == ResultReason::Canceled;

    result1 = nullptr;
    result2 = nullptr;
    synthesizer = nullptr;

    DoSomethingWithAudioInPullStream(stream, canceled);
}

TEST_CASE("Synthesizer output to pull stream start using before done synthesizing", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto stream = AudioOutputStream::CreatePullStream();

    bool canceled = false;
    auto future = DoSomethingWithAudioInPullStreamInBackground(stream, canceled);

    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering to pullstream */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering to pullstream */

    canceled = result1->Reason == ResultReason::Canceled && result2->Reason == ResultReason::Canceled;

    result1 = nullptr;
    result2 = nullptr;
    synthesizer = nullptr;
}

TEST_CASE("Speak out in results", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */
                                                                       /* NOTE: FYI ... all non-nullptr "config" scenarios work independently from audio in result scenarios */

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering, and available in result1 */
    SPXTEST_REQUIRE(result1->ResultId.length() == GUID_LENGTH);
    if (result1->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(result1->Reason == ResultReason::SynthesizingAudioCompleted);
        auto audioData = result1->GetAudioData();
        SPXTEST_REQUIRE(audioData->size() > EMPTY_WAVE_FILE_SIZE);
        SPXTEST_REQUIRE(0 == std::memcmp(&audioData->front(), "RIFF", 4));
    }

    auto audioData1 = result1->GetAudioData(); /* of type std::shared_ptr<std::vector<uint8_t>> */
    DoSomethingWithAudioInVector(audioData1, result1->GetAudioLength());

    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering, and available in result2 */
    SPXTEST_REQUIRE(result2->ResultId.length() == GUID_LENGTH);
    if (result2->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioCompleted);
        SPXTEST_REQUIRE(result2->GetAudioLength() > 0);
    }

    auto audioData2 = result2->GetAudioData(); /* of type std::shared_ptr<std::vector<uint8_t>> */
    DoSomethingWithAudioInVector(audioData2, result2->GetAudioLength());
}

TEST_CASE("Speak output in chunks in event synthesizing", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    synthesizer->Synthesizing += [](const SpeechSynthesisEventArgs& e) {

        auto resultReason = e.Result->Reason;
        SPXTEST_REQUIRE(resultReason == ResultReason::SynthesizingAudio);

        auto audioLength = e.Result->GetAudioLength();
        SPXTEST_REQUIRE(audioLength > 0);

        auto audioData = e.Result->GetAudioData();
        DoSomethingWithAudioInVector(audioData, e.Result->GetAudioLength());
    };

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has completed rendering */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has completed rendering */
}

TEST_CASE("Speak output in streams", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering, and available in result1 */
    if (result1->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(result1->Reason == ResultReason::SynthesizingAudioCompleted);
        SPXTEST_REQUIRE(result1->GetAudioLength() > 0);
    }

    auto stream1 = AudioDataStream::FromResult(result1); /* of type std::shared_ptr<AudioDataStream>, does not block */
    DoSomethingWithAudioInDataStream(stream1, true);

    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering, and available in result2 */
    if (result2->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioCompleted);
        SPXTEST_REQUIRE(result2->GetAudioLength() > 0);
    }

    auto stream2 = AudioDataStream::FromResult(result2); /* of type std::shared_ptr<AudioDataStream>, does not block */
    DoSomethingWithAudioInDataStream(stream2, true);

    stream1->SetPosition(0);
    DoSomethingWithAudioInDataStream(stream1, true); /* re-check stream1 to make sure it's not impacted by stream2 */
}

TEST_CASE("Speak output in streams before done from event synthesis started", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    // hook the future here to make sure the callback below is not blocked by DoSomethingWithAudioInDataStreamInBackground
    future<void> futureThread[3];
    int requestOrder = -1;
    synthesizer->SynthesisStarted += [&futureThread, &requestOrder](const SpeechSynthesisEventArgs& e) {

        auto resultReason = e.Result->Reason;
        SPXTEST_REQUIRE(resultReason == ResultReason::SynthesizingAudioStarted);

        auto audioLength = e.Result->GetAudioLength();
        SPXTEST_REQUIRE(audioLength == 0);

        requestOrder++;
        auto stream = AudioDataStream::FromResult(e.Result); /* of type std::shared_ptr<AudioDataStream>, does not block */
        futureThread[requestOrder] = DoSomethingWithAudioInDataStreamInBackground(stream, false);
    };

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has completed rendering */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has completed rendering */

    auto future3 = synthesizer->SpeakTextAsync("{{{text3}}}"); /* "{{{text3}}}" synthesis might have started */
    auto result3 = future3.get(); /* "{{{text3}}}" synthesis has completed */
}

TEST_CASE("Speak output in streams before done from method start speaking text async", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto result1 = synthesizer->StartSpeakingTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" synthesis has started, likely not finished */
    SPXTEST_REQUIRE(result1->Reason == ResultReason::SynthesizingAudioStarted);
    SPXTEST_REQUIRE(result1->GetAudioLength() == 0);

    auto stream1 = AudioDataStream::FromResult(result1); /* of type std::shared_ptr<AudioDataStream>, does not block */
    auto future1 = DoSomethingWithAudioInDataStreamInBackground(stream1, false); /* does not block, just spins a thread up */

    auto result2 = synthesizer->StartSpeakingTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" synthesis has started, likely not finished */
    SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioStarted);
    SPXTEST_REQUIRE(result2->GetAudioLength() == 0);

    auto stream2 = AudioDataStream::FromResult(result2); /* of type std::shared_ptr<AudioDataStream>, does not block */
    auto future2 = DoSomethingWithAudioInDataStreamInBackground(stream2, false); /* does not block, just spins a thread up */
}

TEST_CASE("Speak output in streams before done queued", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); // nullptr indicates to do nothing with synthesizer audio by default

    int startedRequests = 0;
    synthesizer->SynthesisStarted += [&startedRequests](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        startedRequests++;
    };

    int doneRequests = 0;

    synthesizer->SynthesisCompleted += [&startedRequests, &doneRequests](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        doneRequests++;
        if (doneRequests == 1)
        {
            // This is to check the requests is queued
            // When one request is already completed, the other one is still not started
            SPXTEST_REQUIRE(startedRequests == 1);
        }
    };

    synthesizer->SynthesisCanceled += [&startedRequests, &doneRequests](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        doneRequests++;
        if (doneRequests == 1)
        {
            // This is to check the requests is queued
            // When one request is already completed, the other one is still not started
            SPXTEST_REQUIRE(startedRequests == 1);
        }
    };

    auto futureResult1 = synthesizer->StartSpeakingTextAsync("{{{text1}}}"); // "{{{text1}}}" synthesis might have started, likely not finished
    auto futureResult2 = synthesizer->StartSpeakingTextAsync("{{{text2}}}"); // "{{{text2}}}" synthesis might have started (very unlikely)

    auto future1 = DoSomethingWithAudioInResultInBackground(futureResult1, false); // does not block, just spins a thread up
    auto future2 = DoSomethingWithAudioInResultInBackground(futureResult2, false); // does not block, just spins a thread up

    future1.wait();
    future2.wait();
}

TEST_CASE("Speak output in streams with all data get on synthesis started result", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto result = synthesizer->StartSpeakingTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" synthesis has started, likely not finished */
    SPXTEST_REQUIRE(result->Reason == ResultReason::SynthesizingAudioStarted);
    SPXTEST_REQUIRE(result->GetAudioLength() == 0);

    bool synthesisDone = false;
    synthesizer->SynthesisCompleted += [&synthesisDone](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        synthesisDone = true;
    };

    synthesizer->SynthesisCanceled += [&synthesisDone](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        synthesisDone = true;
    };

    while (!synthesisDone)
    {
        std::this_thread::sleep_for(100ms); /* wait for the synthesis to be done */
    }

    auto stream = AudioDataStream::FromResult(result); /* now get the stream from result */
    DoSomethingWithAudioInDataStream(stream, true); /* the stream should be with AllData status */
}

TEST_CASE("Result data should be consistent with output stream data", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto stream = AudioOutputStream::CreatePullStream();
    auto streamConfig = AudioConfig::FromStreamOutput(stream);

    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);
    auto result = synthesizer->SpeakTextAsync("{{{text1}}}").get();

    auto resultData = std::make_shared<std::vector<uint8_t>>();
    // The audio data in result has a riff header while data in output stream dosen't
    resultData->resize(result->GetAudioLength() - EMPTY_WAVE_FILE_SIZE);
    memcpy(resultData->data(), result->GetAudioData()->data() + EMPTY_WAVE_FILE_SIZE, result->GetAudioLength() - EMPTY_WAVE_FILE_SIZE);

    result = nullptr;
    synthesizer = nullptr; // destruct synthesizer in order to close output stream

    bool canceled = false;
    DoSomethingWithAudioInPullStream(stream, canceled, resultData);
}

TEST_CASE("Synthesis with invalide subscription key", "[api][cxx]")
{
    auto config = SpeechConfig::FromSubscription("InvalidKey", Config::Region);

    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);
    bool synthesisCanceled = false;
    synthesizer->SynthesisCanceled += [&synthesisCanceled](const SpeechSynthesisEventArgs& e) {
        SPXTEST_REQUIRE(ResultReason::Canceled == e.Result->Reason);
        SPXTEST_REQUIRE(0 == e.Result->GetAudioData()->size());
        synthesisCanceled = true;
    };
    auto result = synthesizer->SpeakTextAsync("{{{text1}}}").get();
    SPXTEST_REQUIRE(ResultReason::Canceled == result->Reason);
    SPXTEST_REQUIRE(0 == result->GetAudioData()->size());
    SPXTEST_REQUIRE(synthesisCanceled);
}

TEST_CASE("Defaults - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has now completed rendering to default speakers */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has now completed rendering to default speakers */
}

TEST_CASE("Explicitly use default speakers - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto deviceConfig = AudioConfig::FromDefaultSpeakerOutput();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, deviceConfig);

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has now completed rendering to default speakers */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has now completed rendering to default speakers */
}

TEST_CASE("Pick language - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    config->SetSpeechSynthesisLanguage("en-GB");
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has now completed rendering to default speakers */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has now completed rendering to default speakers */
}

TEST_CASE("Pick voice - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    config->SetSpeechSynthesisVoiceName("Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has now completed rendering to default speakers */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has now completed rendering to default speakers */
}

TEST_CASE("Synthesizer output to file - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto fileConfig = AudioConfig::FromWavFileOutput("wavefile.wav");

    auto synthesizer = SpeechSynthesizer::FromConfig(config, fileConfig);
    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"" */
    synthesizer = nullptr; /* "{{{wavefile.wav}}}" is now closed */

    auto waveSize1 = GetFileSize("wavefile.wav");
    if (result1->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(waveSize1 > EMPTY_WAVE_FILE_SIZE);
    }

    synthesizer = SpeechSynthesizer::FromConfig(config, fileConfig);
    result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"" */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{wavefile.wav}}}" now contains synthesized audio for both "{{{text1}}}"" and "{{{text2}}}" */
    synthesizer = nullptr; /* "{{{wavefile.wav}}}" is now closed */

    auto waveSize2 = GetFileSize("wavefile.wav");
    if (result1->Reason != ResultReason::Canceled && result2->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(waveSize2 > waveSize1);
    }
}

TEST_CASE("Synthesizer output to push stream - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();

    auto callback = std::make_shared<PushAudioOutputStreamTestCallback>();
    auto stream = AudioOutputStream::CreatePushStream(callback);

    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering to pushstream */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering to pushstream */

    bool canceled = result1->Reason == ResultReason::Canceled && result2->Reason == ResultReason::Canceled;

    result1 = nullptr;
    result2 = nullptr;
    synthesizer = nullptr;

    if (!canceled)
    {
        SPXTEST_REQUIRE(callback->GetAudioSize() > 0);
        SPXTEST_REQUIRE(callback->IsClosed());
    }
    else
    {
        SPXTEST_REQUIRE(callback->GetAudioSize() == 0);
        SPXTEST_REQUIRE(callback->IsClosed());
    }
}

TEST_CASE("Synthesizer output to pull stream use after synthesis completed - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto stream = AudioOutputStream::CreatePullStream();
    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering to pullstream */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering to pullstream */

    bool canceled = result1->Reason == ResultReason::Canceled && result2->Reason == ResultReason::Canceled;

    result1 = nullptr;
    result2 = nullptr;
    synthesizer = nullptr;

    DoSomethingWithAudioInPullStream(stream, canceled);
}

TEST_CASE("Synthesizer output to pull stream start using before done synthesizing - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto stream = AudioOutputStream::CreatePullStream();

    bool canceled = false;
    auto future = DoSomethingWithAudioInPullStreamInBackground(stream, canceled);

    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering to pullstream */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering to pullstream */

    canceled = result1->Reason == ResultReason::Canceled && result2->Reason == ResultReason::Canceled;

    result1 = nullptr;
    result2 = nullptr;
    synthesizer = nullptr;
}

TEST_CASE("Speak out in results - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */
                                                                       /* NOTE: FYI ... all non-nullptr "config" scenarios work independently from audio in result scenarios */

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering, and available in result1 */
    SPXTEST_REQUIRE(result1->ResultId.length() == GUID_LENGTH);
    if (result1->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(result1->Reason == ResultReason::SynthesizingAudioCompleted);
        SPXTEST_REQUIRE(result1->GetAudioLength() > 0);
    }

    auto audioData1 = result1->GetAudioData(); /* of type std::shared_ptr<std::vector<uint8_t>> */
    DoSomethingWithAudioInVector(audioData1, result1->GetAudioLength());

    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering, and available in result2 */
    SPXTEST_REQUIRE(result2->ResultId.length() == GUID_LENGTH);
    if (result2->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioCompleted);
        SPXTEST_REQUIRE(result2->GetAudioLength() > 0);
    }

    auto audioData2 = result2->GetAudioData(); /* of type std::shared_ptr<std::vector<uint8_t>> */
    DoSomethingWithAudioInVector(audioData2, result2->GetAudioLength());
}

TEST_CASE("Speak output in chunks in event synthesizing - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    synthesizer->Synthesizing += [](const SpeechSynthesisEventArgs& e) {

        auto resultReason = e.Result->Reason;
        SPXTEST_REQUIRE(resultReason == ResultReason::SynthesizingAudio);

        auto audioLength = e.Result->GetAudioLength();
        SPXTEST_REQUIRE(audioLength > 0);

        auto audioData = e.Result->GetAudioData();
        DoSomethingWithAudioInVector(audioData, e.Result->GetAudioLength());
    };

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has completed rendering */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has completed rendering */
}

TEST_CASE("Speak output in streams - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering, and available in result1 */
    if (result1->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(result1->Reason == ResultReason::SynthesizingAudioCompleted);
        SPXTEST_REQUIRE(result1->GetAudioLength() > 0);
    }

    auto stream1 = AudioDataStream::FromResult(result1); /* of type std::shared_ptr<AudioDataStream>, does not block */
    DoSomethingWithAudioInDataStream(stream1, true);

    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering, and available in result2 */
    if (result2->Reason != ResultReason::Canceled)
    {
        SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioCompleted);
        SPXTEST_REQUIRE(result2->GetAudioLength() > 0);
    }

    auto stream2 = AudioDataStream::FromResult(result2); /* of type std::shared_ptr<AudioDataStream>, does not block */
    DoSomethingWithAudioInDataStream(stream2, true);

    stream1->SetPosition(0);
    DoSomethingWithAudioInDataStream(stream1, true); /* re-check stream1 to make sure it's not impacted by stream2 */
}

TEST_CASE("Speak output in streams before done from event synthesis started - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    // hook the future here to make sure the callback below is not blocked by DoSomethingWithAudioInDataStreamInBackground
    future<void> futureThread[3];
    int requestOrder = -1;
    synthesizer->SynthesisStarted += [&futureThread, &requestOrder](const SpeechSynthesisEventArgs& e) {

        auto resultReason = e.Result->Reason;
        SPXTEST_REQUIRE(resultReason == ResultReason::SynthesizingAudioStarted);

        auto audioLength = e.Result->GetAudioLength();
        SPXTEST_REQUIRE(audioLength == 0);

        requestOrder++;
        auto stream = AudioDataStream::FromResult(e.Result); /* of type std::shared_ptr<AudioDataStream>, does not block */
        futureThread[requestOrder] = DoSomethingWithAudioInDataStreamInBackground(stream, false);
    };

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has completed rendering */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has completed rendering */

    auto future3 = synthesizer->SpeakTextAsync("{{{text3}}}"); /* "{{{text3}}}" synthesis might have started */
    auto result3 = future3.get(); /* "{{{text3}}}" synthesis has completed */
}

TEST_CASE("Speak output in streams before done from method start speaking text async - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto result1 = synthesizer->StartSpeakingTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" synthesis has started, likely not finished */
    SPXTEST_REQUIRE(result1->Reason == ResultReason::SynthesizingAudioStarted);
    SPXTEST_REQUIRE(result1->GetAudioLength() == 0);

    auto stream1 = AudioDataStream::FromResult(result1); /* of type std::shared_ptr<AudioDataStream>, does not block */
    auto future1 = DoSomethingWithAudioInDataStreamInBackground(stream1, false); /* does not block, just spins a thread up */

    auto result2 = synthesizer->StartSpeakingTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" synthesis has started, likely not finished */
    SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioStarted);
    SPXTEST_REQUIRE(result2->GetAudioLength() == 0);

    auto stream2 = AudioDataStream::FromResult(result2); /* of type std::shared_ptr<AudioDataStream>, does not block */
    auto future2 = DoSomethingWithAudioInDataStreamInBackground(stream2, false); /* does not block, just spins a thread up */
}

TEST_CASE("Speak output in streams before done queued - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); // nullptr indicates to do nothing with synthesizer audio by default

    int startedRequests = 0;
    synthesizer->SynthesisStarted += [&startedRequests](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        startedRequests++;
    };

    int doneRequests = 0;

    synthesizer->SynthesisCompleted += [&startedRequests, &doneRequests](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        doneRequests++;
        if (doneRequests == 1)
        {
            // This is to check the requests is queued
            // When one request is already completed, the other one is still not started
            SPXTEST_REQUIRE(startedRequests == 1);
        }
    };

    synthesizer->SynthesisCanceled += [&startedRequests, &doneRequests](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        doneRequests++;
        if (doneRequests == 1)
        {
            // This is to check the requests is queued
            // When one request is already completed, the other one is still not started
            SPXTEST_REQUIRE(startedRequests == 1);
        }
    };

    auto futureResult1 = synthesizer->StartSpeakingTextAsync("{{{text1}}}"); // "{{{text1}}}" synthesis might have started, likely not finished
    auto futureResult2 = synthesizer->StartSpeakingTextAsync("{{{text2}}}"); // "{{{text2}}}" synthesis might have started (very unlikely)

    auto future1 = DoSomethingWithAudioInResultInBackground(futureResult1, false); // does not block, just spins a thread up
    auto future2 = DoSomethingWithAudioInResultInBackground(futureResult2, false); // does not block, just spins a thread up

    future1.wait();
    future2.wait();
}

TEST_CASE("Speak output in streams with all data get on synthesis started result - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto result = synthesizer->StartSpeakingTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" synthesis has started, likely not finished */
    SPXTEST_REQUIRE(result->Reason == ResultReason::SynthesizingAudioStarted);
    SPXTEST_REQUIRE(result->GetAudioLength() == 0);

    bool synthesisDone = false;
    synthesizer->SynthesisCompleted += [&synthesisDone](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        synthesisDone = true;
    };

    synthesizer->SynthesisCanceled += [&synthesisDone](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        synthesisDone = true;
    };

    while (!synthesisDone)
    {
        std::this_thread::sleep_for(100ms); /* wait for the synthesis to be done */
    }

    auto stream = AudioDataStream::FromResult(result); /* now get the stream from result */
    DoSomethingWithAudioInDataStream(stream, true); /* the stream should be with AllData status */
}

TEST_CASE("Check word boundary events - USP", "[api][cxx]")
{
    auto config = UspSpeechConfig();
    config->SetSpeechSynthesisVoiceName("Microsoft Server Speech Text to Speech Voice (zh-CN, HuihuiRUS)");
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr);

    std::string plainText = "您好，我是来自Microsoft的中文声音。";
    std::string ssml = "<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='zh-CN'><voice name='Microsoft Server Speech Text to Speech Voice (zh-CN, HuihuiRUS)'>您好，<break time='50ms'/>我是来自Microsoft的中文声音。</voice></speak>";

    uint64_t expectedAudioOffsets[8] = { 500000, 7443750, 9175625, 10585000, 15096875, 22924375, 25213125, 28760000 };
    uint32_t expectedTextOffsets[8] = { 0, 3, 4, 5, 7, 16, 17, 19 };
    uint32_t expectedSsmlOffsets[8] = { 251, 274, 275, 276, 278, 287, 288, 290 };
    uint32_t expectedWordLengths[8] = { 2, 1, 1, 2, 9, 1, 2, 2 };

    int order = 0;
    synthesizer->WordBoundary += [&order, expectedAudioOffsets, expectedTextOffsets, expectedWordLengths](const SpeechSynthesisWordBoundaryEventArgs& e) {
        SPXTEST_REQUIRE(expectedAudioOffsets[order] == e.AudioOffset);
        SPXTEST_REQUIRE(expectedTextOffsets[order] == e.TextOffset);
        SPXTEST_REQUIRE(expectedWordLengths[order] == e.WordLength);
        order++;
    };

    synthesizer->SpeakTextAsync(plainText);
    SPXTEST_REQUIRE(8 == order);

    synthesizer->WordBoundary.DisconnectAll();

    order = 0;
    synthesizer->WordBoundary += [&order, expectedAudioOffsets, expectedSsmlOffsets, expectedWordLengths](const SpeechSynthesisWordBoundaryEventArgs& e) {
        SPXTEST_REQUIRE(e.AudioOffset > 0);
        SPXTEST_REQUIRE(expectedSsmlOffsets[order] == e.TextOffset);
        SPXTEST_REQUIRE(expectedWordLengths[order] == e.WordLength);
        order++;
    };

    synthesizer->SpeakSsmlAsync(ssml);
    SPXTEST_REQUIRE(8 == order);
}

TEST_CASE("Defaults - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has now completed rendering to default speakers */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has now completed rendering to default speakers */

    auto expectedAudioData1 = BuildMockSynthesizedAudioWithHeader("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData2 = BuildMockSynthesizedAudioWithHeader("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);

    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData1, result1->GetAudioData()));
    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData2, result2->GetAudioData()));
}

TEST_CASE("Explicitly use default speakers - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto deviceConfig = AudioConfig::FromDefaultSpeakerOutput();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, deviceConfig);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has now completed rendering to default speakers */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has now completed rendering to default speakers */

    auto expectedAudioData1 = BuildMockSynthesizedAudioWithHeader("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData2 = BuildMockSynthesizedAudioWithHeader("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);

    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData1, result1->GetAudioData()));
    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData2, result2->GetAudioData()));
}

TEST_CASE("Pick language - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    config->SetSpeechSynthesisLanguage("en-GB");
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has now completed rendering to default speakers */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has now completed rendering to default speakers */

    auto expectedAudioData1 = BuildMockSynthesizedAudioWithHeader("{{{text1}}}", "en-GB", "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
    auto expectedAudioData2 = BuildMockSynthesizedAudioWithHeader("{{{text2}}}", "en-GB", "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");

    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData1, result1->GetAudioData()));
    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData2, result2->GetAudioData()));
}

TEST_CASE("Pick voice - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    config->SetSpeechSynthesisVoiceName("Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has now completed rendering to default speakers */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has now completed rendering to default speakers */

    auto expectedAudioData1 = BuildMockSynthesizedAudioWithHeader("{{{text1}}}", DEFAULT_LANGUAGE, "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");
    auto expectedAudioData2 = BuildMockSynthesizedAudioWithHeader("{{{text2}}}", DEFAULT_LANGUAGE, "Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)");

    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData1, result1->GetAudioData()));
    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData2, result2->GetAudioData()));
}

/* Test cases based on mock TTS engine, with output audio content check */

TEST_CASE("Synthesizer output to file - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto fileConfig = AudioConfig::FromWavFileOutput("wavefile.wav");

    auto synthesizer = SpeechSynthesizer::FromConfig(config, fileConfig);
    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"" */
    synthesizer = nullptr; /* "{{{wavefile.wav}}}" is now closed */

    auto waveData1 = LoadWaveFileData("wavefile.wav");
    auto expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData1, waveData1));

    synthesizer = SpeechSynthesizer::FromConfig(config, fileConfig);
    result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{wavefile.wav}}}" now contains synthesized audio for "{{{text1}}}"" */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{wavefile.wav}}}" now contains synthesized audio for both "{{{text1}}}"" and "{{{text2}}}" */
    synthesizer = nullptr; /* "{{{wavefile.wav}}}" is now closed */

    auto waveData12 = LoadWaveFileData("wavefile.wav");
    auto expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);
    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData12, waveData12));
}

TEST_CASE("Synthesizer output to push stream - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();

    auto callback = std::make_shared<PushAudioOutputStreamTestCallback>();
    auto stream = AudioOutputStream::CreatePushStream(callback);

    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering to pushstream */
    auto expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData1, callback->GetAudioData()));

    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering to pushstream */
    auto expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);
    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData12, callback->GetAudioData()));

    result1 = nullptr;
    result2 = nullptr;
    synthesizer = nullptr;

    SPXTEST_REQUIRE(callback->IsClosed());
}

TEST_CASE("Synthesizer output to pull stream use after synthesis completed - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto stream = AudioOutputStream::CreatePullStream();
    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering to pullstream */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering to pullstream */

    result1 = nullptr;
    result2 = nullptr;
    synthesizer = nullptr;

    auto expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);

    bool canceled = false;
    DoSomethingWithAudioInPullStream(stream, canceled, expectedAudioData12);
}

TEST_CASE("Synthesizer output to pull stream start using before done synthesizing - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto stream = AudioOutputStream::CreatePullStream();

    auto expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);

    bool canceled = false;
    auto future = DoSomethingWithAudioInPullStreamInBackground(stream, canceled, expectedAudioData12);

    auto streamConfig = AudioConfig::FromStreamOutput(stream);
    auto synthesizer = SpeechSynthesizer::FromConfig(config, streamConfig);

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering to pullstream */
    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering to pullstream */

    canceled = result1->Reason == ResultReason::Canceled && result2->Reason == ResultReason::Canceled;

    result1 = nullptr;
    result2 = nullptr;
    synthesizer = nullptr;
}

TEST_CASE("Speak out in results - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering, and available in result1 */
    SPXTEST_REQUIRE(result1->ResultId.length() == GUID_LENGTH);
    SPXTEST_REQUIRE(result1->Reason == ResultReason::SynthesizingAudioCompleted);
    auto expectedAudioData1 = BuildMockSynthesizedAudioWithHeader("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData1, result1->GetAudioData()));

    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering, and available in result2 */
    SPXTEST_REQUIRE(result2->ResultId.length() == GUID_LENGTH);
    SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioCompleted);
    auto expectedAudioData2 = BuildMockSynthesizedAudioWithHeader("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    SPXTEST_REQUIRE(AreBinaryEqual(expectedAudioData2, result2->GetAudioData()));
}

TEST_CASE("Speak output in chunks in event synthesizing - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData12 = MergeBinary(expectedAudioData1, expectedAudioData2);

    size_t offset = 0;
    synthesizer->Synthesizing += [expectedAudioData12, &offset](const SpeechSynthesisEventArgs& e) {

        auto resultReason = e.Result->Reason;
        SPXTEST_REQUIRE(resultReason == ResultReason::SynthesizingAudio);

        auto audioLength = e.Result->GetAudioLength() - EMPTY_WAVE_FILE_SIZE;
        auto audioData = e.Result->GetAudioData();
        auto binaryEqual = AreBinaryEqual(expectedAudioData12->data() + offset, audioLength, audioData->data() + EMPTY_WAVE_FILE_SIZE, audioLength);
        SPXTEST_REQUIRE(binaryEqual == true);

        offset += audioLength;
    };

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has completed rendering */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has completed rendering */
}

TEST_CASE("Speak output in streams - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto result1 = synthesizer->SpeakTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" has completed rendering, and available in result1 */
    SPXTEST_REQUIRE(result1->Reason == ResultReason::SynthesizingAudioCompleted);

    auto stream1 = AudioDataStream::FromResult(result1); /* of type std::shared_ptr<AudioDataStream>, does not block */
    auto expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    DoSomethingWithAudioInDataStream(stream1, true, expectedAudioData1);

    auto result2 = synthesizer->SpeakTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" has completed rendering, and available in result2 */
    SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioCompleted);

    auto stream2 = AudioDataStream::FromResult(result2); /* of type std::shared_ptr<AudioDataStream>, does not block */
    auto expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    DoSomethingWithAudioInDataStream(stream2, true, expectedAudioData2);
}

TEST_CASE("Speak output in streams before done from event synthesis started - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    std::shared_ptr<std::vector<uint8_t>> expectedAudioData[3];
    expectedAudioData[0] = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    expectedAudioData[1] = BuildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    expectedAudioData[2] = BuildMockSynthesizedAudio("{{{text3}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);

    // hook the future here to make sure the callback below is not blocked by DoSomethingWithAudioInDataStreamInBackground
    future<void> futureThread[3];
    int requestOrder = -1;
    synthesizer->SynthesisStarted += [expectedAudioData, &futureThread, &requestOrder](const SpeechSynthesisEventArgs& e) {

        auto resultReason = e.Result->Reason;
        SPXTEST_REQUIRE(resultReason == ResultReason::SynthesizingAudioStarted);

        auto audioLength = e.Result->GetAudioLength();
        SPXTEST_REQUIRE(audioLength == 0);

        requestOrder++;

        auto stream = AudioDataStream::FromResult(e.Result); /* of type std::shared_ptr<AudioDataStream>, does not block */
        futureThread[requestOrder] = DoSomethingWithAudioInDataStreamInBackground(stream, false, expectedAudioData[requestOrder]);
    };

    synthesizer->SpeakTextAsync("{{{text1}}}"); /* "{{{text1}}}" has completed rendering */
    synthesizer->SpeakTextAsync("{{{text2}}}"); /* "{{{text2}}}" has completed rendering */

    auto future3 = synthesizer->SpeakTextAsync("{{{text3}}}"); /* "{{{text3}}}" synthesis might have started */
    auto result3 = future3.get(); /* "{{{text3}}}" synthesis has completed */
}

TEST_CASE("Speak output in streams before done from method start speaking text async - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto result1 = synthesizer->StartSpeakingTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" synthesis has started, likely not finished */
    SPXTEST_REQUIRE(result1->Reason == ResultReason::SynthesizingAudioStarted);
    SPXTEST_REQUIRE(result1->GetAudioLength() == 0);

    auto stream1 = AudioDataStream::FromResult(result1); /* of type std::shared_ptr<AudioDataStream>, does not block */
    auto expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto future1 = DoSomethingWithAudioInDataStreamInBackground(stream1, false, expectedAudioData1); /* does not block, just spins a thread up */

    auto result2 = synthesizer->StartSpeakingTextAsync("{{{text2}}}").get(); /* "{{{text2}}}" synthesis has started, likely not finished */
    SPXTEST_REQUIRE(result2->Reason == ResultReason::SynthesizingAudioStarted);
    SPXTEST_REQUIRE(result2->GetAudioLength() == 0);

    auto stream2 = AudioDataStream::FromResult(result2); /* of type std::shared_ptr<AudioDataStream>, does not block */
    auto expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto future2 = DoSomethingWithAudioInDataStreamInBackground(stream2, false, expectedAudioData2); /* does not block, just spins a thread up */
}

TEST_CASE("Speak output in streams before done queued - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    int startedRequests = 0;
    synthesizer->SynthesisStarted += [&startedRequests](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        startedRequests++;
    };

    int completedRequests = 0;
    synthesizer->SynthesisCompleted += [&startedRequests, &completedRequests](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        completedRequests++;
        if (completedRequests == 1)
        {
            // This is to check the requests is queued
            // When one request is already completed, the other one is still not started
            SPXTEST_REQUIRE(startedRequests == 1);
        }
    };

    auto futureResult1 = synthesizer->StartSpeakingTextAsync("{{{text1}}}"); /* "{{{text1}}}" synthesis might have started, likely not finished */
    auto futureResult2 = synthesizer->StartSpeakingTextAsync("{{{text2}}}"); /* "{{{text2}}}" synthesis might have started (very unlikely) */

    auto expectedAudioData1 = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto future1 = DoSomethingWithAudioInResultInBackground(futureResult1, false, expectedAudioData1); /* does not block, just spins a thread up */
    auto expectedAudioData2 = BuildMockSynthesizedAudio("{{{text2}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto future2 = DoSomethingWithAudioInResultInBackground(futureResult2, false, expectedAudioData2); /* does not block, just spins a thread up */

    future1.wait();
    future2.wait();
}

TEST_CASE("Speak output in streams with all data get on synthesis started result - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    auto result = synthesizer->StartSpeakingTextAsync("{{{text1}}}").get(); /* "{{{text1}}}" synthesis has started, likely not finished */
    SPXTEST_REQUIRE(result->Reason == ResultReason::SynthesizingAudioStarted);
    SPXTEST_REQUIRE(result->GetAudioLength() == 0);

    bool synthesisCompleted = false;
    synthesizer->SynthesisCompleted += [&synthesisCompleted](const SpeechSynthesisEventArgs& e) {
        UNUSED(e);
        synthesisCompleted = true;
    };

    while (!synthesisCompleted)
    {
        std::this_thread::sleep_for(100ms); /* wait for the synthesis to be completed */
    }

    auto stream = AudioDataStream::FromResult(result); /* now get the stream from result */
    auto expectedAudioData = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    DoSomethingWithAudioInDataStream(stream, true, expectedAudioData); /* the stream should be with AllData status */
}

TEST_CASE("Speak output in streams with all data get since synthesizing result - Mock", "[api][cxx]")
{
    auto config = MockSpeechConfig();
    auto synthesizer = SpeechSynthesizer::FromConfig(config, nullptr); /* nullptr indicates to do nothing with synthesizer audio by default */

    bool firstChunkSkipped = false;
    std::shared_ptr<AudioDataStream> stream;
    synthesizer->Synthesizing += [&firstChunkSkipped, &stream](const SpeechSynthesisEventArgs& e) {
        if (firstChunkSkipped && stream == nullptr)
        {
            stream = AudioDataStream::FromResult(e.Result);
        }

        firstChunkSkipped = true;
    };

    // Check result
    auto result = synthesizer->SpeakTextAsync("{{{text1}}}").get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::SynthesizingAudioCompleted);

    // Check stream
    SPXTEST_REQUIRE(stream->GetStatus() == StreamStatus::PartialData);

    auto fullAudioData = BuildMockSynthesizedAudio("{{{text1}}}", DEFAULT_LANGUAGE, DEFAULT_VOICE);
    auto expectedAudioData = std::make_shared<std::vector<uint8_t>>();
    expectedAudioData->resize(fullAudioData->size() - MOCK_AUDIO_CHUNK_SIZE);
    memcpy(expectedAudioData->data(), fullAudioData->data() + MOCK_AUDIO_CHUNK_SIZE, expectedAudioData->size());

    CheckAudioInDataStream(stream, expectedAudioData);
}

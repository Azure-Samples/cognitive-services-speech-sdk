//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <map>
#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

template<typename RecogType>
static std::shared_ptr<RecogType> CreateRecognizers(const string& filename)
{
    auto audioInput = AudioConfig::FromWavFileInput(filename);
    return RecogType::FromConfig(CurrentSpeechConfig(), audioInput);
}

static void DoRecoFromCompressedPushStreamHelper(std::string fileName, std::shared_ptr<SpeechRecognizer> recognizer, std::shared_ptr<PushAudioInputStream> pushStream)
{
    auto result = make_shared<RecoPhrases>();

    ConnectCallbacks(recognizer.get(), result);
    recognizer->StartContinuousRecognitionAsync().get();
    PushData(pushStream.get(), fileName, true);
    WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
    recognizer->StopContinuousRecognitionAsync().get();
    SPXTEST_REQUIRE(!result->phrases.empty());
    SPXTEST_REQUIRE(result->phrases[0].Text == AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text);
}

static void DoRecoFromCompressedPushStream(std::string fileName, AudioStreamContainerFormat containerType)
{
#ifndef __linux__
    try
    {
#endif
        auto config = CurrentSpeechConfig();
        // Create the recognizer with the pull stream
        auto pushStream = AudioInputStream::CreatePushStream(AudioStreamFormat::GetCompressedFormat(containerType));
        auto audioConfig = AudioConfig::FromStreamInput(pushStream);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);
        DoRecoFromCompressedPushStreamHelper(fileName, recognizer, pushStream);
#ifndef __linux__
    }
    catch (const std::exception& e)
    {
        std::string str(e.what());
        std::string refException("Exception with an error code: 0x29 (SPXERR_GSTREAMER_NOT_FOUND_ERROR)");
        CAPTURE(e.what());
        SPXTEST_REQUIRE(str.find(refException) != string::npos);
    }
#endif
}

static void DoRecoFromCompressedPullStreamHelper(std::shared_ptr<SpeechRecognizer> recognizer)
{
    auto result = make_shared<RecoPhrases>();

    ConnectCallbacks(recognizer.get(), result);
    recognizer->StartContinuousRecognitionAsync().get();
    WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
    recognizer->StopContinuousRecognitionAsync().get();
    SPXTEST_REQUIRE(!result->phrases.empty());
    SPXTEST_REQUIRE(result->phrases[0].Text == AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text);
}

static void DoRecoFromCompressedPullStream(std::string filename, AudioStreamContainerFormat containerType)
{
#ifndef __linux__
    try
    {
#endif
        promise<string> result;
        auto config = CurrentSpeechConfig();

        // Prepare for the stream to be "Pulled"
        auto fs = OpenFile(filename);

        // Create the "pull stream" object with C++ lambda callbacks
        auto pullStream = AudioInputStream::CreatePullStream(
            AudioStreamFormat::GetCompressedFormat(containerType),
            [&fs](uint8_t* buffer, uint32_t size) -> int { return (int)ReadBuffer(fs, buffer, size); },
            [=]() {});

        // Create the recognizer with the pull stream
        auto audioConfig = AudioConfig::FromStreamInput(pullStream);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioConfig);

        DoRecoFromCompressedPullStreamHelper(recognizer);
#ifndef __linux__
    }
    catch (const std::exception& e)
    {
        std::string str(e.what());
        std::string refException("Exception with an error code: 0x29 (SPXERR_GSTREAMER_NOT_FOUND_ERROR)");
        CAPTURE(e.what());
        SPXTEST_REQUIRE(str.find(refException) != string::npos);
    }
#endif
}

TEST_CASE("compressed stream test", "[api][cxx]")
{
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_MP3)));
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_OPUS)));
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_A_LAW)));
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_MU_LAW)));
    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_FLAC)));

    SPXTEST_SECTION("push stream works mp3")
    {
        DoRecoFromCompressedPushStream(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_MP3), AudioStreamContainerFormat::MP3);
    }

    SPXTEST_SECTION("push stream works opus")
    {
        DoRecoFromCompressedPushStream(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_OPUS), AudioStreamContainerFormat::OGG_OPUS);
    }

    SPXTEST_SECTION("push stream failed with FLAC")
    {
        DoRecoFromCompressedPushStream(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_FLAC), AudioStreamContainerFormat::FLAC);
    }

    SPXTEST_SECTION("push stream failed with ALAW")
    {
        DoRecoFromCompressedPushStream(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_A_LAW), AudioStreamContainerFormat::ALAW);
    }

    SPXTEST_SECTION("push stream failed with MULAW")
    {
        DoRecoFromCompressedPushStream(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_MU_LAW), AudioStreamContainerFormat::MULAW);
    }

    SPXTEST_SECTION("pull stream works mp3")
    {
        DoRecoFromCompressedPullStream(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_MP3), AudioStreamContainerFormat::MP3);
    }

    SPXTEST_SECTION("pull stream works opus")
    {
        DoRecoFromCompressedPullStream(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_OPUS), AudioStreamContainerFormat::OGG_OPUS);
    }

    SPXTEST_SECTION("pull stream failed with FLAC")
    {
        DoRecoFromCompressedPullStream(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_FLAC), AudioStreamContainerFormat::FLAC);
    }

    SPXTEST_SECTION("pull stream failed with ALAW")
    {
        DoRecoFromCompressedPullStream(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_A_LAW), AudioStreamContainerFormat::ALAW);
    }

    SPXTEST_SECTION("pull stream failed with MULAW")
    {
        DoRecoFromCompressedPullStream(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_MU_LAW), AudioStreamContainerFormat::MULAW);
    }
}

TEST_CASE("continuousRecognitionAsync using push stream", "[api][cxx]")
{
    auto config = CurrentSpeechConfig();
    auto pushStream = AudioInputStream::CreatePushStream();
    auto audioConfig = AudioConfig::FromStreamInput(pushStream);

    // Creates a speech recognizer from stream input;
    auto audioInput = AudioConfig::FromStreamInput(pushStream);
    auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
    SpxSetMockParameterBool("CARBON-INTERNAL-MOCK-SdkKwsEngine", true);

    SPXTEST_SECTION("continuous and once")
    {
        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(recognizer.get(), result);
        recognizer->StartContinuousRecognitionAsync().get();
        PushData(pushStream.get(), ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        recognizer->StopContinuousRecognitionAsync().get();
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(result->phrases[0].Text == AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text);

        //todo: we should allow switching recog mode. Fix this later.
        //pushData(pushStream.get(), weather.m_audioFilename);
        //REQUIRE_THROWS(recognizer->RecognizeOnceAsync().get());
    }
    SPXTEST_SECTION("continuous and kws")
    {
        DoContinuousReco(recognizer.get(), pushStream.get());
        DoKWS(recognizer.get(), pushStream.get());
    }
    SPXTEST_SECTION("continuous start stop 3 times")
    {
        auto loop = 3;
        for (int i = 0; i < loop; i++)
        {
            auto result = make_shared<RecoPhrases>();
            ConnectCallbacks(recognizer.get(), result);
            // TODO: move PushData() after StartContinuousRecongtion
            // after fixing the bug Bug 1506194: PushStream Improvement, endOfStream
            PushData(pushStream.get(), ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
            recognizer->StartContinuousRecognitionAsync().get();

            WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
            recognizer->StopContinuousRecognitionAsync().get();
            SPXTEST_REQUIRE(!result->phrases.empty());
            SPXTEST_REQUIRE(result->phrases[0].Text == AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text);
        }
    }
#if 0
    // Created #1502076
    //todo: we need to turn it on after refactor audio session stream
    SPXTEST_SECTION("kws and continuous")
    {
        doKWS(recognizer.get(), pushStream.get());
        // the KWS does not go back to idle, stays as "StoppingPump"
        doContinuousReco(recognizer.get(), pushStream.get());
    }
#endif

}

TEST_CASE("ContinuousRecognitionAsync using file input", "[a0pi][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_ENGLISH)));

    auto recognizer = CreateRecognizers<SpeechRecognizer>(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    // a normal case.
    SPXTEST_SECTION("start and stop once")
    {
        auto result = make_shared<RecoPhrases>();
        recognizer->StartContinuousRecognitionAsync().get();

        ConnectCallbacks(recognizer.get(), result);
        WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        recognizer->StopContinuousRecognitionAsync().get();
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(result->phrases[0].Text == AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text);
    }
    // Another normal case. no stop is a valid user case.
    SPXTEST_SECTION("start without stop")
    {
        auto result = make_shared<RecoPhrases>();
        recognizer->StartContinuousRecognitionAsync().get();

        ConnectCallbacks(recognizer.get(), result);
        WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        SPXTEST_REQUIRE(!result->phrases.empty());
        SPXTEST_REQUIRE(result->phrases[0].Text == AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text);
    }

    SPXTEST_SECTION("two starts in a row")
    {
        auto recognizer2 = CreateRecognizers<SpeechRecognizer>(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_ENGLISH));

        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(recognizer2.get(), result);

        recognizer2->StartContinuousRecognitionAsync().get();
        REQUIRE_THROWS_WITH(recognizer2->StartContinuousRecognitionAsync().get(), Catch::Contains("SPXERR_START_RECOGNIZING_INVALID_STATE_TRANSITION"));
    }
#if 0
    // start and stop immediately
    SPXTEST_SECTION("start and then stop immediately")
    {
        promise<string> result;
        recognizer->StartContinuousRecognitionAsync().get();

        ConnectCallbacks(recognizer.get(), result);
        recognizer->StopContinuousRecognitionAsync().get();

        // the recognizer should be idle after stop.
        auto text = WaitForResult(result.get_future(), WAIT_FOR_RECO_RESULT_TIME);
    }
    // start and stop in the middle
    SPXTEST_SECTION("stop in the middle of reco")
    {
        promise<string> result;
        recognizer->StartContinuousRecognitionAsync().get();

        ConnectCallbacks(recognizer.get(), result);

        // wait for 500 ms and stop recognition.
        // hopefully, we cut the audio session while pumping data.
        this_thread::sleep_for(500ms);

        recognizer->StopContinuousRecognitionAsync().get();
    }
#endif
}

#ifdef SPEECHSDK_USE_OPENSSL
TEST_CASE("Single trusted root", "[.][int][prod]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    // Baltimore CyberTrust Root
    // which is root for the cert on the Speech Service
    // Validity: Not After: May 12 23:59:00 2025 GMT
    static char baltimoreCyberTrustRoot[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
        "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
        "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
        "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
        "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
        "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
        "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
        "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
        "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
        "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
        "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
        "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
        "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
        "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
        "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
        "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
        "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
        "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
        "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
        "-----END CERTIFICATE-----\n";

    // ISRG Root X1 (https://letsencrypt.org/certificates/)
    // Validity: Not After: Jun 4 11:04:38 2035 GMT
    static char isrgRootX1[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
        "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
        "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
        "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
        "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
        "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
        "h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
        "0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
        "A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
        "T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
        "B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
        "B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
        "KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
        "OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
        "jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
        "qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
        "rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
        "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
        "hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
        "ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
        "3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
        "NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
        "ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
        "TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
        "jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
        "oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
        "4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
        "mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
        "emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
        "-----END CERTIFICATE-----\n";

    UseMocks(false);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH("WEATHER")));
    SPXTEST_REQUIRE(!IsUsingMocks());
    auto audio = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH("WEATHER"));

    SPXTEST_SECTION("with Speech Service cert")
    {
        auto config = SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
        SPXTEST_SECTION("fail with invalid single trusted cert")
        {
            config->SetProperty("OPENSSL_SINGLE_TRUSTED_CERT", "bogus");
            auto recognizer = SpeechRecognizer::FromConfig(config, audio);
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
            auto cancellation = CancellationDetails::FromResult(result);
            SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::Error);
            SPXTEST_REQUIRE(cancellation->ErrorCode == CancellationErrorCode::ConnectionFailure);
            CAPTURE(cancellation->ErrorDetails);
            SPXTEST_REQUIRE(cancellation->ErrorDetails.find("Internal error: 1") != std::string::npos);
        }
        SPXTEST_SECTION("pass with Baltimore CyberTrust Root as single trusted cert")
        {
            config->SetProperty("OPENSSL_SINGLE_TRUSTED_CERT", baltimoreCyberTrustRoot);
            auto recognizer = SpeechRecognizer::FromConfig(config, audio);
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        }
    }
    SPXTEST_SECTION("with Let's Encrypt cert")
    {
        auto config = SpeechConfig::FromEndpoint("wss://valid-isrgrootx1.letsencrypt.org/", "invalid-key");

        config->SetProperty("OPENSSL_SINGLE_TRUSTED_CERT", isrgRootX1);

        SPXTEST_SECTION("fails with CRL check")
        {
            auto recognizer = SpeechRecognizer::FromConfig(config, audio);
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
            auto cancellation = CancellationDetails::FromResult(result);
            SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::Error);
            SPXTEST_REQUIRE(cancellation->ErrorCode == CancellationErrorCode::ConnectionFailure);
            CAPTURE(cancellation->ErrorDetails);
            SPXTEST_REQUIRE(cancellation->ErrorDetails.find("Internal error: 1") != std::string::npos);
        }
        SPXTEST_SECTION("passes without CRL check")
        {
            config->SetProperty("OPENSSL_SINGLE_TRUSTED_CERT_CRL_CHECK", "false");
            auto recognizer = SpeechRecognizer::FromConfig(config, audio);
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
            auto cancellation = CancellationDetails::FromResult(result);
            SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::Error);
            SPXTEST_REQUIRE(cancellation->ErrorCode == CancellationErrorCode::ConnectionFailure);
            CAPTURE(cancellation->ErrorDetails);
            SPXTEST_REQUIRE(cancellation->ErrorDetails.find("WebSocket Upgrade failed with HTTP status code:") != std::string::npos);
        }
    }
}
#endif

TEST_CASE("Speech Recognizer basics", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXTEST_SECTION("push stream with continuous reco")
    {
        UseMocks(false);
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

        auto config = CurrentSpeechConfig();
        auto pushStream = AudioInputStream::CreatePushStream();
        auto audioConfig = AudioConfig::FromStreamInput(pushStream);

        // Creates a speech recognizer from stream input;
        auto audioInput = AudioConfig::FromStreamInput(pushStream);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        // promise for synchronization of recognition end.
        promise<void> recognitionEnd;

        string result;
        recognizer->Recognized.Connect([&result](const SpeechRecognitionEventArgs& e)
        {
            if (e.Result->Reason == ResultReason::RecognizedSpeech)
            {
                result = e.Result->Text;
                SPX_TRACE_VERBOSE("RECOGNIZED: Text= %s, Offset= %" PRIu64 ", Duration= %" PRIu64, e.Result->Text.c_str(), e.Result->Offset(), e.Result->Duration());
            }
            else if (e.Result->Reason == ResultReason::NoMatch)
            {
                SPX_TRACE_VERBOSE("NOMATCH: Speech could not be recognized.");
            }
        });
        string error;
        recognizer->Canceled.Connect([&recognitionEnd, &error](const SpeechRecognitionCanceledEventArgs& e)
        {
            switch (e.Reason)
            {
            case CancellationReason::EndOfStream:
                SPX_TRACE_VERBOSE("CANCELED: Reach the end of the file.");
                break;

            case CancellationReason::Error:
                error = !e.ErrorDetails.empty() ? e.ErrorDetails : "Errors!";
                SPX_TRACE_VERBOSE("CANCELED: ErrorCode=%d, ErrorDetails=%s", (int)e.ErrorCode, e.ErrorDetails.c_str());
                recognitionEnd.set_value();
                break;

            default:
                SPX_TRACE_VERBOSE("unknown Reason!");
            }
        });
        recognizer->SessionStopped.Connect([&recognitionEnd](const SessionEventArgs& e)
        {
            SPX_TRACE_VERBOSE("Session stopped. Id = %s", e.SessionId.c_str());
            recognitionEnd.set_value();
        });

        auto fs = OpenWaveFile(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

        std::array<uint8_t, 1000> buffer;
        recognizer->StartContinuousRecognitionAsync().get();
        while (1)
        {
            auto readSamples = ReadBuffer(fs, buffer.data(), (uint32_t)buffer.size());
            if (readSamples == 0)
            {
                break;
            }
            pushStream->Write(buffer.data(), readSamples);
        }

        pushStream->Close();
        recognitionEnd.get_future().get();
        recognizer->StopContinuousRecognitionAsync().get();

        SPXTEST_REQUIRE(result == AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text);
        SPXTEST_REQUIRE(error == "");
    }

    SPXTEST_SECTION("KWS throws exception given 11khz sampling rate")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);
        UseMocks(false);
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(AUDIO_11_KHZ)));

        SPXTEST_REQUIRE(!IsUsingMocks());

        auto config = SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key,
            SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);

        auto audio = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(AUDIO_11_KHZ));
        auto recognizer = SpeechRecognizer::FromConfig(config, audio);
        auto model = KeywordRecognitionModel::FromFile(DefaultSettingsMap[INPUT_DIR] + "/kws/Computer/kws.table");
        REQUIRE_THROWS(recognizer->StartKeywordRecognitionAsync(model).get());
    }
    SPXTEST_SECTION("throw exception when the file does not existing")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);

        SPXTEST_REQUIRE(!IsUsingMocks());
        REQUIRE_THROWS(CreateRecognizers<SpeechRecognizer>("non-existing-file.wav"));
    }
    SPXTEST_SECTION("return an error message in RecognizeOnceAsync given an invalid endpoint")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
        SPXTEST_REQUIRE(!IsUsingMocks());

        auto config = SpeechConfig::FromEndpoint("Invalid-endpoint", SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        auto audio = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        auto recognizer = SpeechRecognizer::FromConfig(config, audio);
        auto result = recognizer->RecognizeOnceAsync().get();

        SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);

        auto cancellation = CancellationDetails::FromResult(result);
        SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::Error);
        SPXTEST_REQUIRE(cancellation->ErrorCode == CancellationErrorCode::ConnectionFailure);
        CAPTURE(cancellation->ErrorDetails);
        SPXTEST_REQUIRE(cancellation->ErrorDetails.find("Failed to create transport request.") != std::string::npos);
    }
    SPXTEST_SECTION("return canceled in StartContinuousRecognitionAsync given an invalid endpoint")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
        SPXTEST_REQUIRE(!IsUsingMocks());

        auto config = SpeechConfig::FromEndpoint("Invalid-endpoint", SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        auto audio = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        auto recognizer = SpeechRecognizer::FromConfig(config, audio);
        std::string errorDetails;
        CancellationErrorCode errorCode;
        recognizer->Canceled += [&](const SpeechRecognitionCanceledEventArgs& e)
        {
            if (e.Reason == CancellationReason::Error)
            {
                errorDetails = e.ErrorDetails;
                errorCode = e.ErrorCode;
            }
        };
        recognizer->StartContinuousRecognitionAsync().get();
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        recognizer->StopContinuousRecognitionAsync().get();

        SPXTEST_REQUIRE(errorCode == CancellationErrorCode::ConnectionFailure);
        CAPTURE(errorDetails);
        SPXTEST_REQUIRE(errorDetails.find("Failed to create transport request.") != std::string::npos);
    }
    SPXTEST_SECTION("Check that recognition can set authorization token")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
        SPXTEST_REQUIRE(!IsUsingMocks());

        auto recognizer = CreateRecognizers<SpeechRecognizer>(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

        std::string token("Thursday");
        recognizer->SetAuthorizationToken(token);
        SPXTEST_REQUIRE(recognizer->GetAuthorizationToken() == token);

        token = "Friday";
        recognizer->SetAuthorizationToken(token);
        SPXTEST_REQUIRE(recognizer->GetAuthorizationToken() == token);
    }

    SPXTEST_SECTION("Check that recognizer does not crash while async op is in progress")
    {
        UseMocks(true);
        SPXTEST_REQUIRE(IsUsingMocks());

        const int numLoops = 10;

        {
            vector<std::future<std::shared_ptr<SpeechRecognitionResult>>> futures;
            for (int i = 0; i < numLoops; i++)
            {
                auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig());
                SPXTEST_REQUIRE(recognizer != nullptr);
                futures.push_back(recognizer->RecognizeOnceAsync());
            }
        }

        auto model = KeywordRecognitionModel::FromFile(DefaultSettingsMap[INPUT_DIR] + "/kws/Computer/kws.table");
        SPXTEST_REQUIRE(model != nullptr);

        {
            vector<std::future<void>> futures;
            int numAsyncMethods = 4;
            for (int i = 0; i < numLoops*numAsyncMethods; i++)
            {
                auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig());
                SPXTEST_REQUIRE(recognizer != nullptr);
                if (i % numAsyncMethods == 0)
                {
                    futures.push_back(recognizer->StartContinuousRecognitionAsync());
                }
                else if (i % numAsyncMethods == 1)
                {
                    futures.push_back(recognizer->StopContinuousRecognitionAsync());
                }
                else if (i % numAsyncMethods == 2)
                {
                    futures.push_back(recognizer->StartKeywordRecognitionAsync(model));
                }
                else
                {
                    futures.push_back(recognizer->StopKeywordRecognitionAsync());
                }
            }
        }
    }

    GIVEN("Mocks for USP, Microphone, WavFilePump and Reader, and then USP ...")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(true);

        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

        mutex mtx;
        condition_variable cv;

        SPXTEST_WHEN("We checking to make sure callback counts are correct (checking multiple times, and multiple speeds times) ...")
        {
            std::map<Callbacks, atomic_int> callbackCounts = createCallbacksMap();

            // We're going to loop thru 11 times... The first 10, we'll use mocks. The last time we'll use the USP
            // NOTE: Please keep this at 11... It tests various "race"/"speed" configurations of the core system...
            // NOTE: When running against the localhost, loop 20 times... Half the time, we'll use mocks, and half - the USP.
            const int numLoops = (DefaultSettingsMap[ENDPOINT].empty()) ? 11 : 20;

            for (int i = 0; i < numLoops; i++)
            {
                auto useMockUsp = true;

                if (!DefaultSettingsMap[ENDPOINT].empty() && i % 2 == 0)
                {
                    useMockUsp = false;
                }
                else
                {
                    useMockUsp = i + 1 < numLoops;
                }

                auto realTimeRate = (i + 1) * 100 / numLoops;
                SetMockRealTimeSpeed(realTimeRate);
                UseMockUsp(useMockUsp);

                SPX_TRACE_VERBOSE("%s: START of loop #%d; mockUsp=%d; realtime=%d", __FUNCTION__, i, useMockUsp, realTimeRate);

                auto recognizer = CreateRecognizers<SpeechRecognizer>(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
                SPXTEST_REQUIRE(recognizer != nullptr);
                SPXTEST_REQUIRE(IsUsingMocks(useMockUsp));

                bool sessionEnded = false;

                recognizer->Recognized.Connect([&](const SpeechRecognitionEventArgs&) {
                    callbackCounts[Callbacks::final_result]++;
                    SPX_TRACE_VERBOSE("callbackCounts[Callbacks::final_result]=%d", callbackCounts[Callbacks::final_result].load());
                });
                recognizer->SessionStarted.Connect([&](const SessionEventArgs&) {
                    callbackCounts[Callbacks::session_started]++;
                    SPX_TRACE_VERBOSE("callbackCounts[Callbacks::session_started]=%d", callbackCounts[Callbacks::session_started].load());
                });
                recognizer->SpeechStartDetected.Connect([&](const RecognitionEventArgs&) {
                    callbackCounts[Callbacks::speech_start_detected]++;
                    SPX_TRACE_VERBOSE("callbackCounts[Callbacks::speech_start_detected]=%d", callbackCounts[Callbacks::speech_start_detected].load());
                });
                recognizer->SpeechEndDetected.Connect([&](const RecognitionEventArgs&) {
                    callbackCounts[Callbacks::speech_end_detected]++;
                    SPX_TRACE_VERBOSE("callbackCounts[Callbacks::speech_end_detected]=%d", callbackCounts[Callbacks::speech_end_detected].load());
                });
                recognizer->SessionStopped.Connect([&](const SessionEventArgs&) {
                    callbackCounts[Callbacks::session_stopped]++;
                    SPX_TRACE_VERBOSE("callbackCounts[Callbacks::session_stopped]=%d", callbackCounts[Callbacks::session_stopped].load());
                    unique_lock<mutex> lock(mtx);
                    sessionEnded = true;
                    cv.notify_one();
                });

                auto result = recognizer->RecognizeOnceAsync().get();
                CHECK(result != nullptr);

                SPX_TRACE_VERBOSE("%s: Wait for session end (loop #%d)", __FUNCTION__, i);
                unique_lock<mutex> lock(mtx);
                cv.wait_for(lock, std::chrono::seconds(30), [&] { return sessionEnded; });
                lock.unlock();

                SPX_TRACE_VERBOSE("%s: Make sure callbacks are invoked correctly; END of loop #%d", __FUNCTION__, i);
                CHECK(callbackCounts[Callbacks::session_started] == i + 1);
                CHECK(callbackCounts[Callbacks::session_stopped] == i + 1);
                CHECK(callbackCounts[Callbacks::final_result] == i + 1);
                CHECK(callbackCounts[Callbacks::speech_start_detected] == i + 1);
                CHECK(callbackCounts[Callbacks::speech_end_detected] == i + 1);
                CHECK(callbackCounts[Callbacks::no_match] == 0);
            }

            SPX_TRACE_VERBOSE("%s: Wait some more", __FUNCTION__);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            SPX_TRACE_VERBOSE("%s: Checking callback counts ...", __FUNCTION__);

            CHECK(callbackCounts[Callbacks::session_started] == numLoops);
            CHECK(callbackCounts[Callbacks::session_stopped] == numLoops);
            CHECK(callbackCounts[Callbacks::final_result] == numLoops);
            CHECK(callbackCounts[Callbacks::speech_start_detected] == numLoops);
            CHECK(callbackCounts[Callbacks::speech_end_detected] == numLoops);
            CHECK(callbackCounts[Callbacks::no_match] == 0);
        }

        UseMocks(false);
    }

    SPXTEST_SECTION("Check that recognition result contains original json payload.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
        SPXTEST_REQUIRE(!IsUsingMocks());

        auto recognizer = CreateRecognizers<SpeechRecognizer>(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(!result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult).empty());
    }

    SPXTEST_SECTION("Check that recognition result contains error details.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(false);
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
        SPXTEST_REQUIRE(!IsUsingMocks());
        auto badKeyConfig = SpeechConfig::FromSubscription("invalid_key", "invalid_region");
        auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        auto recognizer = SpeechRecognizer::FromConfig(badKeyConfig, audioConfig);
        auto result = recognizer->RecognizeOnceAsync().get();

        SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);

        auto cancellation = CancellationDetails::FromResult(result);
        SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::Error);
        SPXTEST_REQUIRE(cancellation->ErrorCode == CancellationErrorCode::ConnectionFailure);
        SPXTEST_REQUIRE(!cancellation->ErrorDetails.empty());

        // NOTE: Looks like we still do need this...
        // TODO: there's a data race in the audio_pump thread when it tries to
        // pISpxAudioProcessor->SetFormat(nullptr); after exiting the loop.
        // Comment out the next line to see for yourself (repros on Linux build machines).
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    SPXTEST_SECTION("Wrong Key triggers Canceled Event")
    {
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
        UseMocks(false);
        std::promise<void> canceledPromise;

        bool connectionReportedError = false;
        bool canceled = false;
        string wrongKey = "wrongKey";

        auto sc = SpeechConfig::FromSubscription(wrongKey, "westus");
        auto a = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        auto recognizer = SpeechRecognizer::FromConfig(sc, a);

        recognizer->Canceled.Connect([&](const SpeechRecognitionCanceledEventArgs& args) {
            canceled = true;
            connectionReportedError =
                args.Reason == CancellationReason::Error &&
                args.ErrorCode == CancellationErrorCode::AuthenticationFailure &&
                !args.ErrorDetails.empty();
            canceledPromise.set_value();
        });

        auto result = recognizer->RecognizeOnceAsync().get();
        CHECK(result->Reason == ResultReason::Canceled);
        REQUIRE(canceledPromise.get_future().wait_for(5s) == future_status::ready);
        REQUIRE(connectionReportedError);
    }

    SPXTEST_SECTION("German Speech Recognition works")
    {
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_GERMAN)));

        auto sc = !DefaultSettingsMap[ENDPOINT].empty() ? SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT],
            SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key) :
            SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key,
                SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);

        auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_GERMAN));
        auto recognizer = SpeechRecognizer::FromConfig(sc, "de-DE", audioConfig);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text));
    }

    SPXTEST_SECTION("Canceled/EndOfStream works")
    {
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

        auto sc = !DefaultSettingsMap[ENDPOINT].empty() ? SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT],
            SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key) :
            SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key,
                SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);

        auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        auto recognizer = SpeechRecognizer::FromConfig(sc, audioConfig);

        WHEN("using RecognizeOnceAsync() result")
        {
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(!result->Text.empty());
            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);

            result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->Text.empty());
            SPXTEST_REQUIRE(result->Reason == ResultReason::NoMatch);

            auto nomatch = NoMatchDetails::FromResult(result);
            SPXTEST_REQUIRE(nomatch != nullptr);
            SPXTEST_REQUIRE(nomatch->Reason == NoMatchReason::InitialSilenceTimeout);

            result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);
            SPXTEST_REQUIRE(result->Text.empty());
            SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);

            auto cancellation = CancellationDetails::FromResult(result);
            SPXTEST_REQUIRE(cancellation != nullptr);
            SPXTEST_REQUIRE(cancellation->ErrorDetails.empty());
            SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::EndOfStream);
        }

        WHEN("using RecognizeOnceAsync() and SpeechRecognitionCanceledEventArgs e.Result")
        {
            mutex mtx;
            condition_variable cv;
            auto sessionStoppedCount = 0;
            std::vector<std::string> events;

            recognizer->Recognized.Connect([&](const SpeechRecognitionEventArgs& e) {
                auto result = e.Result;
                if (result->Reason == ResultReason::RecognizedSpeech)
                {
                    if (!result->Text.empty())
                    {
                        events.push_back("RecognizedSpeech-NonEmpty");
                    }
                    else
                    {
                        events.push_back("RecognizedSpeech-Other");
                    }
                }
                else if (result->Reason == ResultReason::NoMatch)
                {
                    auto nomatch = NoMatchDetails::FromResult(result);
                    if (result->Text.empty() &&
                        nomatch->Reason == NoMatchReason::InitialSilenceTimeout)
                    {
                        events.push_back("NoMatch-InitialSilenceTimeout");
                    }
                    else
                    {
                        events.push_back("NoMatch-Other");
                    }
                }
                else
                {
                    events.push_back("Recognized-Other");
                }
            });

            recognizer->Canceled.Connect([&](const SpeechRecognitionCanceledEventArgs& e) {
                auto result = e.Result;

                if (e.Reason == CancellationReason::EndOfStream)
                {
                    auto cancellation = CancellationDetails::FromResult(result);
                    if (result->Text.empty() &&
                        result->Reason == ResultReason::Canceled &&
                        cancellation->ErrorDetails.empty() &&
                        cancellation->Reason == CancellationReason::EndOfStream)
                    {
                        events.push_back("EndOfStream");
                    }
                    else
                    {
                        events.push_back("EndOfStream-Malformed");
                    }
                }
                else
                {
                    events.push_back("Canceled-Other");
                }
            });

            recognizer->SessionStarted.Connect([&](const SessionEventArgs&) {
                events.push_back("SessionStarted");
                cv.notify_all();
            });

            recognizer->SessionStopped.Connect([&](const SessionEventArgs&) {
                events.push_back("SessionStopped");
                ++sessionStoppedCount;
                cv.notify_all();
            });

            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);

            result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);

            result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result != nullptr);

            std::unique_lock<std::mutex> lock(mtx);
            cv.wait_for(lock, std::chrono::seconds(30), [&] { return sessionStoppedCount == 3; });
            lock.unlock();

            std::vector<std::string> expectedEvents = {
                "SessionStarted", "RecognizedSpeech-NonEmpty", "SessionStopped",
                "SessionStarted", "NoMatch-InitialSilenceTimeout", "SessionStopped",
                "SessionStarted", "EndOfStream", "SessionStopped"
            };

            REQUIRE(expectedEvents == events);
        }
    }
}

TEST_CASE("KWS basics", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    GIVEN("Mocks for USP, KWS, and the Microphone...")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        UseMocks(true);

        mutex mtx;
        condition_variable cv;

        int gotFinalResult = 0;
        int gotSessionStopped = 0;
        int gotKeywordRecognizing = 0;
        int gotKeywordRecognized = 0;
        int gotKeywordNotRecognized = 0;;

        SPXTEST_WHEN("We do a keyword recognition with a speech recognizer")
        {
            SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

            auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig());
            SPXTEST_REQUIRE(recognizer != nullptr);
            SPXTEST_REQUIRE(IsUsingMocks(true));

            recognizer->Recognizing += [&](const SpeechRecognitionEventArgs& event) {
                std::unique_lock<std::mutex> lock(mtx);
                if (event.Result->Reason == ResultReason::RecognizingKeyword) {
                    gotKeywordRecognizing++;
                    SPX_TRACE_VERBOSE("gotKeywordRecognizing=%d", gotKeywordRecognizing);
                }
            };

            recognizer->Recognized += [&](const SpeechRecognitionEventArgs& event) {
                std::unique_lock<std::mutex> lock(mtx);
                if (event.Result->Reason == ResultReason::RecognizedKeyword) {
                    gotKeywordRecognized++;
                    SPX_TRACE_VERBOSE("gotKeywordRecognized=%d", gotKeywordRecognized);
                }
                else if (event.Result->Reason == ResultReason::RecognizedSpeech) {
                    gotFinalResult++;
                    SPX_TRACE_VERBOSE("gotFinalResult=%d", gotFinalResult);
                }
                else if (event.Result->Reason == ResultReason::NoMatch) {
                    // With keyword verification turned off, this should never happen
                    auto details = NoMatchDetails::FromResult(event.Result);
                    if (details->Reason == NoMatchReason::KeywordNotRecognized) {
                        gotKeywordNotRecognized++;
                        SPX_TRACE_VERBOSE("gotKeywordNotRecognized=%d", gotKeywordNotRecognized);
                    }
                }
            };

            recognizer->SessionStopped += [&](const SessionEventArgs&) {
                std::unique_lock<std::mutex> lock(mtx);
                gotSessionStopped++;
                SPX_TRACE_VERBOSE("gotSessionStopped=%d", gotSessionStopped);
                cv.notify_all();
            };

            auto model = KeywordRecognitionModel::FromFile(DefaultSettingsMap[INPUT_DIR] + "/kws/Computer/kws.table");
            recognizer->StartKeywordRecognitionAsync(model);

            THEN("We wait up to 30 seconds for a KwsSingleShot recognition and it's accompanying SessionStopped")
            {
                SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

                std::unique_lock<std::mutex> lock(mtx);
                cv.wait_for(lock, std::chrono::seconds(30), [&] { return gotFinalResult >= 1 && gotSessionStopped >= 1; });

                recognizer->StopKeywordRecognitionAsync().get();

                THEN("We should see that we got at least 1 of each RecognizedSpeech, RecognizingKeyword, RecognizedKeyword, SessionStopped event and 0 KeywordNotRecognized event")
                {
                    CHECK(gotFinalResult >= 1);
                    CHECK(gotKeywordRecognizing >= 1);
                    CHECK(gotKeywordRecognized >= 1);
                    CHECK(gotSessionStopped >= 1);
                    CHECK(gotKeywordNotRecognized == 0);
                }

                lock.unlock();
            }
        }

        UseMocks(false);
    }
}

TEST_CASE("Speech on local server", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXTEST_SECTION("Stress testing against the local server")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        if (DefaultSettingsMap[ENDPOINT].empty())
        {
            return;
        }

        UseMocks(false);
        SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
        SPXTEST_REQUIRE(!IsUsingMocks());

        const int numLoops = 10;

        auto sc = SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT], R"({"max_timeout":"0"})");
        for (int i = 0; i < numLoops; i++)
        {
            auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
            auto recognizer = SpeechRecognizer::FromConfig(sc, audioConfig);
            auto result = recognizer->RecognizeOnceAsync().get();
            SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
            SPXTEST_REQUIRE(result->Text == "Remind me to buy 5 iPhones.");
        }

        // BUGBUG: this currently fails because CSpxAudioStreamSession::WaitForRecognition() returns a nullptr on a timeout.
        /*
        factory = SpeechFactory::FromEndpoint(PAL::ToWString(Config::Endpoint), LR"({"max_timeout":"10000"})");
        for (int i = 0; i < numLoops; i++)
        {
        auto audioConfig = AudioConfig::FromWavFileInput(weather.m_audioFilename);
        auto recognizer = factory->CreateSpeechRecognizerFromConfig(audioConfig);
        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(result->Text == L"Remind me to buy 5 iPhones.");
        }
        */
    }
}

TEST_CASE("Speech Recognizer is thread-safe.", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    mutex mtx;
    condition_variable cv;

    SPXTEST_SECTION("Check for competing or conflicting conditions in destructor.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        bool callback_invoked = false;

        SPXTEST_REQUIRE(!IsUsingMocks());
        auto recognizer = CreateRecognizers<SpeechRecognizer>(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

        auto callback = [&](const SpeechRecognitionEventArgs& args)
        {
            {
                UNUSED(args);
                unique_lock<mutex> lock(mtx);
                callback_invoked = true;
                cv.notify_one();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        };

        auto canceledCallback = [&](const SpeechRecognitionCanceledEventArgs& args) { callback(args); };

        recognizer->Recognized.Connect(callback);
        recognizer->Canceled.Connect(canceledCallback); // Canceled is called if there are connection issues.

        auto result = recognizer->RecognizeOnceAsync().get();

        {
            unique_lock<mutex> lock(mtx);
            cv.wait_for(lock, std::chrono::seconds(10), [&] { return callback_invoked; });
            SPXTEST_REQUIRE(callback_invoked);
        }
        recognizer.reset();
    }

    SPXTEST_SECTION("Check for a deadlock in disconnect.")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        SPXTEST_REQUIRE(!IsUsingMocks());
        auto recognizer = CreateRecognizers<SpeechRecognizer>(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

        auto callback1 = [&](const SpeechRecognitionEventArgs& args)
        {
            if (args.Result->Reason == ResultReason::RecognizedSpeech)
            {
                recognizer->Canceled.DisconnectAll();
            }
            else
            {
                recognizer->Recognized.DisconnectAll();
            }
        };
        auto canceledCallback1 = [&](const SpeechRecognitionCanceledEventArgs& args) { callback1(args); };

        recognizer->Recognized.Connect(callback1);
        recognizer->Canceled.Connect(canceledCallback1);

        auto result = recognizer->RecognizeOnceAsync().get();
        UNUSED(result);

        auto callback2 = [&](const SpeechRecognitionEventArgs&)
        {
            recognizer->Canceled.DisconnectAll();
            recognizer->Recognized.DisconnectAll();
        };
        auto canceledCallback2 = [&](const SpeechRecognitionCanceledEventArgs& args) { callback2(args); };

        recognizer = CreateRecognizers<SpeechRecognizer>(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        recognizer->Recognized.Connect(callback2);
        recognizer->Canceled.Connect(canceledCallback2);

        result = recognizer->RecognizeOnceAsync().get();
        UNUSED(result);

        auto callback3 = [&](const SpeechRecognitionEventArgs&)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // TODO: fix test
            // This captures a reference to stack-memory which is no longer active when the callback is called
            // recognizer.reset();
        };

        auto canceledCallback3 = [&](const SpeechRecognitionEventArgs& args)
        {
            callback3(args);
        };

        recognizer = CreateRecognizers<SpeechRecognizer>(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        recognizer->Recognized.Connect(callback3);
        recognizer->Canceled.Connect(canceledCallback3);
        auto future = recognizer->RecognizeOnceAsync();
        UNUSED(future);
    }
}

TEST_CASE("Speech Recognizer SpeechConfig validations", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXTEST_SECTION("Check that FromEndpoint raises an exception when called with illegal values")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        CHECK_THROWS(SpeechConfig::FromEndpoint("", ""));
        CHECK_THROWS(SpeechConfig::FromEndpoint("", "illegal-subscription"));
        CHECK_NOTHROW(SpeechConfig::FromEndpoint("illegal-endpoint", "illegal-subscription"));
    }

    SPXTEST_SECTION("Check that FromSubscription raises an exception when called with illegal values")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        CHECK_THROWS(SpeechConfig::FromSubscription("", ""));
        CHECK_THROWS(SpeechConfig::FromSubscription("", "illegal-region"));
        CHECK_THROWS(SpeechConfig::FromSubscription("illegal-subscription", ""));
        CHECK_NOTHROW(SpeechConfig::FromSubscription("illegal-subscription", "illegal-region"));
        auto endpointAsRegion = "wss://westus.stt.speech.microsoft.com/speech/recognition/dictation/cognitiveservices/v1";
        CHECK_THROWS(SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, endpointAsRegion));
        CHECK_THROWS(SpeechTranslationConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, endpointAsRegion));
        CHECK_THROWS(SpeechConfig::FromAuthorizationToken("sometoken", endpointAsRegion));
        CHECK_THROWS(SpeechTranslationConfig::FromAuthorizationToken("sometoken", endpointAsRegion));
    }

    SPXTEST_SECTION("Check that FromAuthorizationToken raises an exception when called with illegal values")
    {
        SPX_TRACE_VERBOSE("%s: line=%d", __FUNCTION__, __LINE__);

        CHECK_THROWS(SpeechConfig::FromAuthorizationToken("", ""));
        CHECK_THROWS(SpeechConfig::FromAuthorizationToken("", "illegal-region"));
        CHECK_THROWS(SpeechConfig::FromAuthorizationToken("illegal-token", ""));
        CHECK_NOTHROW(SpeechConfig::FromAuthorizationToken("illegal-token", "illegal-region"));
    }
}

static void ConnectionEventTests(bool forContinuousRecognition)
{
    auto sc = !DefaultSettingsMap[ENDPOINT].empty() ? SpeechConfig::FromEndpoint(DefaultSettingsMap[ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key) : SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    auto recognizer = SpeechRecognizer::FromConfig(sc, audioConfig);
    auto connection = Connection::FromRecognizer(recognizer);

    promise<bool> recoEnd;
    auto connectedCount = 0;
    auto disconnectedCount = 0;
    auto recognizingCount = 0;
    auto recognizedCount = 0;
    auto sessionStoppedCount = 0;
    auto canceledCount = 0;
    bool connectedAfterRecognizing = false;
    bool connectedDisconnectedUnmatch = false;

    connection->Connected.Connect([&](const ConnectionEventArgs&) {
        connectedCount++;
        if (connectedCount == 1)
        {
            if (recognizedCount != 0 || recognizingCount != 0)
            {
                connectedAfterRecognizing = true;
            }
        }
        if (connectedCount != disconnectedCount + 1)
        {
            connectedDisconnectedUnmatch = true;
        }
    });

    connection->Disconnected.Connect([&](const ConnectionEventArgs&) {
        disconnectedCount++;
        if (connectedCount != disconnectedCount)
        {
            connectedDisconnectedUnmatch = true;
        }
    });

    recognizer->Recognizing.Connect([&](const SpeechRecognitionEventArgs&) {
        if (connectedCount != disconnectedCount && connectedCount != disconnectedCount + 1)
        {
            connectedDisconnectedUnmatch = true;
        }
        recognizingCount++;
    });

    recognizer->Recognized.Connect([&](const SpeechRecognitionEventArgs&) {
        if (connectedCount != disconnectedCount && connectedCount != disconnectedCount + 1)
        {
            connectedDisconnectedUnmatch = true;
        }
        recognizedCount++;
    });

    recognizer->SessionStopped.Connect([&](const SessionEventArgs&) {
        ++sessionStoppedCount;
        recoEnd.set_value(true);
    });

    recognizer->Canceled.Connect([&](const SpeechRecognitionCanceledEventArgs& e) {
        if (e.Reason == CancellationReason::Error)
        {
            canceledCount++;
        }
    });

    if (forContinuousRecognition)
    {
        recognizer->StartContinuousRecognitionAsync().get();
    }
    else
    {
        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
    }

    auto status = recoEnd.get_future().wait_for(WAIT_FOR_RECO_RESULT_TIME);
    SPXTEST_REQUIRE(status == future_status::ready);

    if (forContinuousRecognition)
    {
        recognizer->StopContinuousRecognitionAsync().get();
    }

    SPXTEST_REQUIRE(sessionStoppedCount == 1);
    SPXTEST_REQUIRE(connectedCount > 0);
    SPXTEST_REQUIRE((connectedCount == disconnectedCount || connectedCount == disconnectedCount + 1));
    SPXTEST_REQUIRE(recognizedCount == 1);
    SPXTEST_REQUIRE(sessionStoppedCount == 1);
    SPXTEST_REQUIRE(connectedAfterRecognizing == false);
    SPXTEST_REQUIRE(connectedDisconnectedUnmatch == false);
    SPXTEST_REQUIRE(canceledCount == 0);
}

TEST_CASE("ConnectionEventsTest", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    SPXTEST_SECTION("Connected Disconnected Events with RecognizeOnceAsnyc")
    {
        ConnectionEventTests(false);
    }

    SPXTEST_SECTION("Connected Disconnected Events with ContinuousRecognition")
    {
        ConnectionEventTests(true);
    }
}

TEST_CASE("FromEndpoint without key and token", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    SPXTEST_SECTION("SpeechConfig FromEndpoint")
    {
        auto speechEndpoint = "wss://westus.stt.speech.microsoft.com/speech/recognition/dictation/cognitiveservices/v1";
        auto config = SpeechConfig::FromEndpoint(speechEndpoint);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
        // We cannot really test whether recognizer works, since there is no test endpoint available which supports no authentication.
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty(PropertyId::SpeechServiceAuthorization_Token).empty());
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_Key).empty());
    }

    SPXTEST_SECTION("SpeechTranslationConfig FromEndpoint")
    {
        auto translationEndpoint = "wss://westus.s2s.speech.microsoft.com/speech/translation/cognitiveservices/v1";
        auto config = SpeechTranslationConfig::FromEndpoint(translationEndpoint);
        config->SetSpeechRecognitionLanguage("en-us");
        config->AddTargetLanguage("de");
        auto recognizer = TranslationRecognizer::FromConfig(config, audioInput);
        // We cannot really test whether recognizer works, since there is no test endpoint available which supports no authentication.
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty(PropertyId::SpeechServiceAuthorization_Token).empty());
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_Key).empty());
    }
}

TEST_CASE("SetServiceProperty", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_GERMAN)));
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_GERMAN));

    SPXTEST_SECTION("SetServiceProperty single setting")
    {
        auto config = SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
        config->SetServiceProperty("language", "de-DE", ServicePropertyChannel::UriQueryParameter);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text));
    }

    SPXTEST_SECTION("SetServiceProperty property overwrite")
    {
        auto config = SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
        config->SetSpeechRecognitionLanguage("en-US");
        config->SetServiceProperty("language", "de-DE", ServicePropertyChannel::UriQueryParameter);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text));
    }

    SPXTEST_SECTION("SetServiceProperty 2 properties")
    {
        auto config = SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
        config->SetServiceProperty("language", "de-DE", ServicePropertyChannel::UriQueryParameter);
        config->SetServiceProperty("format", "detailed", ServicePropertyChannel::UriQueryParameter);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text));
        auto detailedResult = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
        SPXTEST_REQUIRE(detailedResult.find("NBest") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("ITN") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Lexical") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("MaskedITN") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Display") != string::npos);
    }

    SPXTEST_SECTION("SetServiceProperty FromEndpoint")
    {
        string speechEndpoint = "wss://" + SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region + ".stt.speech.microsoft.com/speech/recognition/interactive/cognitiveservices/v1";
        auto config = SpeechConfig::FromEndpoint(speechEndpoint, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        config->SetServiceProperty("language", "de-DE", ServicePropertyChannel::UriQueryParameter);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text));
    }

    SPXTEST_SECTION("SetServiceProperty FromEndpoint with parameters")
    {
        string speechEndpoint = "wss://" + SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region + ".stt.speech.microsoft.com/speech/recognition/interactive/cognitiveservices/v1?format=detailed";
        auto config = SpeechConfig::FromEndpoint(speechEndpoint, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        config->SetServiceProperty("language", "de-DE", ServicePropertyChannel::UriQueryParameter);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text));
        auto detailedResult = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
        SPXTEST_REQUIRE(detailedResult.find("NBest") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("ITN") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Lexical") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("MaskedITN") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Display") != string::npos);
    }

    SPXTEST_SECTION("SetServiceProperty invalid parameters")
    {
        auto config = SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);
        CHECK_THROWS(config->SetServiceProperty("", "Value", ServicePropertyChannel::UriQueryParameter));
        CHECK_THROWS(config->SetServiceProperty("Name", "", ServicePropertyChannel::UriQueryParameter));
        CHECK_THROWS(config->SetServiceProperty("Name", "Value", (ServicePropertyChannel)2));
    }

    SPXTEST_SECTION("SetServiceProperty SpeechTranslationConfig")
    {
        auto config = SpeechTranslationConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);

        config->SetServiceProperty("from", "de-DE", ServicePropertyChannel::UriQueryParameter);
        config->SetServiceProperty("to", "en", ServicePropertyChannel::UriQueryParameter);

        auto recognizer = TranslationRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::TranslatedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text));
        SPXTEST_REQUIRE(!result->Translations.at("en").compare("Call the first one."));
    }
}

TEST_CASE("SpeechConfig properties", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_GERMAN)));
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_GERMAN));
    auto config = SpeechConfig::FromSubscription(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region);

    SPXTEST_SECTION("property get and set via propertyId")
    {
        int initialSilenceTimeout = 6000;
        int endSilenceTimeout = 10000;
        config->SetProperty(PropertyId::SpeechServiceConnection_InitialSilenceTimeoutMs, std::to_string(initialSilenceTimeout));
        config->SetProperty(PropertyId::SpeechServiceConnection_EndSilenceTimeoutMs, std::to_string(endSilenceTimeout));
        CHECK(initialSilenceTimeout == std::stoi(config->GetProperty(PropertyId::SpeechServiceConnection_InitialSilenceTimeoutMs)));
        CHECK(endSilenceTimeout == std::stoi(config->GetProperty(PropertyId::SpeechServiceConnection_EndSilenceTimeoutMs)));

        int threshold = 5;
        config->SetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold, std::to_string(threshold));
        CHECK(threshold == std::stoi(config->GetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold)));

        std::string valStr = "something";
        config->SetProperty(PropertyId::SpeechServiceResponse_OutputFormatOption, valStr);
        CHECK(valStr == config->GetProperty(PropertyId::SpeechServiceResponse_OutputFormatOption));

        std::string profanity = "removed";
        config->SetProperty(PropertyId::SpeechServiceResponse_ProfanityOption, profanity);
        CHECK(profanity == config->GetProperty(PropertyId::SpeechServiceResponse_ProfanityOption));

        std::string falseStr = "false";
        config->SetProperty(PropertyId::SpeechServiceConnection_EnableAudioLogging, falseStr);
        CHECK(falseStr == config->GetProperty(PropertyId::SpeechServiceConnection_EnableAudioLogging));

        config->SetProperty(PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps, falseStr);
        CHECK(falseStr == config->GetProperty(PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps));

        config->SetProperty(PropertyId::SpeechServiceResponse_TranslationRequestStablePartialResult, falseStr);
        CHECK(falseStr == config->GetProperty(PropertyId::SpeechServiceResponse_TranslationRequestStablePartialResult));

        std::string trueText = "TrueText";
        config->SetProperty(PropertyId::SpeechServiceResponse_PostProcessingOption, trueText);
        CHECK(trueText == config->GetProperty(PropertyId::SpeechServiceResponse_PostProcessingOption));

        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        CHECK(initialSilenceTimeout == std::stoi(recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_InitialSilenceTimeoutMs)));
        CHECK(endSilenceTimeout == std::stoi(recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_EndSilenceTimeoutMs)));
        CHECK(threshold == std::stoi(recognizer->Properties.GetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold)));
        CHECK(valStr == recognizer->Properties.GetProperty(PropertyId::SpeechServiceResponse_OutputFormatOption));
        CHECK(profanity == recognizer->Properties.GetProperty(PropertyId::SpeechServiceResponse_ProfanityOption));
        CHECK(falseStr == recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_EnableAudioLogging));
        CHECK(falseStr == recognizer->Properties.GetProperty(PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps));
        CHECK(falseStr == recognizer->Properties.GetProperty(PropertyId::SpeechServiceResponse_TranslationRequestStablePartialResult));
        CHECK(trueText == recognizer->Properties.GetProperty(PropertyId::SpeechServiceResponse_PostProcessingOption));
    }

    SPXTEST_SECTION("property direct set and get via propertyId")
    {
        std::string profanity = "removed";
        config->SetProfanity(ProfanityOption::Removed);
        config->EnableAudioLogging();
        config->RequestWordLevelTimestamps();
        config->EnableDictation();

        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        CHECK("DICTATION" == config->GetProperty(PropertyId::SpeechServiceConnection_RecoMode));
        CHECK("DICTATION" == recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_RecoMode));
        CHECK(profanity == config->GetProperty(PropertyId::SpeechServiceResponse_ProfanityOption));
        CHECK(profanity == recognizer->Properties.GetProperty(PropertyId::SpeechServiceResponse_ProfanityOption));
        CHECK("true" == config->GetProperty(PropertyId::SpeechServiceConnection_EnableAudioLogging));
        CHECK("true" == recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_EnableAudioLogging));
        CHECK("true" == config->GetProperty(PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps));
        CHECK("true" == recognizer->Properties.GetProperty(PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps));
    }

    SPXTEST_SECTION("Properties set and check URL")
    {
        config->SetProperty(PropertyId::SpeechServiceConnection_InitialSilenceTimeoutMs, "5000");
        config->SetProperty(PropertyId::SpeechServiceConnection_EndSilenceTimeoutMs, "12000");
        config->SetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold, "5");
        config->SetProperty(PropertyId::SpeechServiceResponse_OutputFormatOption, "detailed");
        config->SetProperty(PropertyId::SpeechServiceResponse_ProfanityOption, "removed");
        config->SetProperty(PropertyId::SpeechServiceConnection_EnableAudioLogging, "false");
        config->SetProperty(PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps, "false");
        config->SetProperty(PropertyId::SpeechServiceResponse_PostProcessingOption, "TrueText");

        // This one is for Translation, should not be picked up by SpeechRecognizer.
        config->SetProperty(PropertyId::SpeechServiceResponse_TranslationRequestStablePartialResult, "true");

        auto sourceLanguageConfig = SourceLanguageConfig::FromLanguage("de-DE");
        auto recognizer = SpeechRecognizer::FromConfig(config, sourceLanguageConfig, audioInput);
        auto result = recognizer->RecognizeOnceAsync().get();
        auto connectionUrl = recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_Url);
        CAPTURE(connectionUrl);

        CHECK(ResultReason::RecognizedSpeech == result->Reason);
        CHECK(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text));
        // Check no word-level timestamps included, but only detailed output.
        string jsonResult = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
        CAPTURE(jsonResult);
        CHECK(jsonResult.find("Words") == string::npos);
        CHECK(jsonResult.find("Lexical") != string::npos);

        CHECK(connectionUrl.find("initialSilenceTimeoutMs=5000") != string::npos);
        CHECK(connectionUrl.find("endSilenceTimeoutMs=12000") != string::npos);
        CHECK(connectionUrl.find("stableIntermediateThreshold=5") != string::npos);
        CHECK(connectionUrl.find("format=detailed") != string::npos);
        CHECK(connectionUrl.find("profanity=removed") != string::npos);
        CHECK(connectionUrl.find("storeAudio=false") != string::npos);
        CHECK(connectionUrl.find("wordLevelTimestamps=false") != string::npos);
        CHECK(connectionUrl.find("postprocessing=TrueText") != string::npos);
        CHECK(connectionUrl.find("language=de-DE") != string::npos);
        CHECK(connectionUrl.find("stableTranslation=") == string::npos);
    }

    SPXTEST_SECTION("Properties set and check URL")
    {
        config->SetProfanity(ProfanityOption::Raw);
        config->EnableAudioLogging();
        config->RequestWordLevelTimestamps();
        config->EnableDictation();
        config->SetOutputFormat(OutputFormat::Simple);

        auto recognizer = SpeechRecognizer::FromConfig(config, "de-DE", audioInput);
        auto result = recognizer->RecognizeOnceAsync().get();
        auto connectionUrl = recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_Url);
        CAPTURE(connectionUrl);

        CHECK(ResultReason::RecognizedSpeech == result->Reason);
        CHECK(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text));
        // Check word-level timestamps included, but only detailed output.
        string jsonResult = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
        CAPTURE(jsonResult);
        CHECK(jsonResult.find("Words") != string::npos);
        CHECK(jsonResult.find("Lexical") != string::npos);

        CHECK(connectionUrl.find("speech/recognition/dictation/cognitiveservices") != string::npos);
        // Word-level timestamps will set format to detailed.
        CHECK(connectionUrl.find("format=detailed") != string::npos);
        CHECK(connectionUrl.find("profanity=raw") != string::npos);
        CHECK(connectionUrl.find("storeAudio=true") != string::npos);
        CHECK(connectionUrl.find("wordLevelTimestamps=true") != string::npos);
        CHECK(connectionUrl.find("language=de-DE") != string::npos);

        CHECK(connectionUrl.find("initialSilenceTimeoutMs=") == string::npos);
        CHECK(connectionUrl.find("endSilenceTimeoutMs=") == string::npos);
        CHECK(connectionUrl.find("stableIntermediateThreshold=") == string::npos);
        CHECK(connectionUrl.find("postprocessing=") == string::npos);
        CHECK(connectionUrl.find("stableTranslation=") == string::npos);
    }
}

TEST_CASE("Dictation Corrections", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto pushStream = AudioInputStream::CreatePushStream();
    auto audioInput = AudioConfig::FromStreamInput(pushStream);
    string endpoint{ "wss://officespeech.platform.bing.com/speech/recognition/dictation/office/v1" };

    SPXTEST_SECTION("send_event")
    {
        auto config = SpeechConfig::FromEndpoint(endpoint, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        config->SetServiceProperty("format", "corrections", ServicePropertyChannel::UriQueryParameter);
        config->EnableDictation();

        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);
        recognizer->SetAuthorizationToken("abc");
        string data = R"(
                       {
                "Id": "Corrections",
                "Name": "Telemetry",
                "ClientSessionId": "DADAAAC4-019C-4D23-9301-7FD619BE68AB",
                "CorrectionEvents": [
                    {
                        "PhraseId": "AEDC2194-019C-4D23-9301-7FD619BE68A9",
                        "CorrectionId": 0,
                        "AlternateId": 1,
                        "TreatedInUX": "true",
                        "TriggerType": "click",
                        "EditType": "alternate"
                    },
                    {
                        "PhraseId": "BEDC2194-019C-4D23-9301-7FD619BE68AA",
                        "CorrectionId": 0,
                        "AlternateId": 2,
                        "TriggerType": "hover",
                        "TreatedInUX": "false",
                        "EditType": "alternate"
                    }
                    ] } )";
        auto connection = Connection::FromRecognizer(recognizer);
        REQUIRE_THROWS(connection->SendMessageAsync("event", "asdf\" qweqr").get());

        connection->SendMessageAsync("event", data).get();

        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(recognizer.get(), result);
        recognizer->StartContinuousRecognitionAsync().get();
        PushData(pushStream.get(), ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        WaitForResult(result->ready.get_future(), 60s);
        recognizer->StopContinuousRecognitionAsync().get();

        SPXTEST_REQUIRE(!result->phrases.empty());
        auto detailedResult = result->phrases[0].Json;
        SPXTEST_REQUIRE(detailedResult.find("NBest") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Corrections") != string::npos);
    }

    SPXTEST_SECTION("set_parameters_in_speech_context_and_config")
    {
        auto config = SpeechConfig::FromEndpoint(endpoint, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        config->SetServiceProperty("format", "corrections", ServicePropertyChannel::UriQueryParameter);
        config->EnableDictation();

        //possible options for punctuation are implicit/explicit/intelligent/none.
        config->SetServiceProperty("punctuation", "none", ServicePropertyChannel::UriQueryParameter);

        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        recognizer->SetAuthorizationToken("abc");
        auto connection = Connection::FromRecognizer(recognizer);

        string phraseDetectionPayload = R"( {"mode": "dictation",
                                              "grammarScenario": "Dictation_Office",
                                              "initialSilenceTimeout": 2000,
                                              "trailingSilenceTimeout": 2000} )";
        // The specification of speech.context.phraseDetection is at https://speechwiki.azurewebsites.net/partners/protocol-phrase-detection.html
        connection->SetMessageProperty("Speech.context", "phraseDetection", phraseDetectionPayload);

        string data = R"({"mode": "MicrosoftEyesOn"})";
        //The spec of dataCollection in speech.config is at https://speechwiki.azurewebsites.net/partners/speechsdk.html#datacollection-element
        connection->SetMessageProperty("speech.config", "dataCollection", data);

        data = R"({"name":"Carbonx","version":"12.01"})";
        connection->SetMessageProperty("speech.config", "application", data);

        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(recognizer.get(), result);
        recognizer->StartContinuousRecognitionAsync().get();
        PushData(pushStream.get(), ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        recognizer->StopContinuousRecognitionAsync().get();

        SPXTEST_REQUIRE(!result->phrases.empty());
        auto detailedResult = result->phrases[0].Json;
        SPXTEST_REQUIRE(detailedResult.find("NBest") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Corrections") != string::npos);
    }
    SPXTEST_SECTION("correction_and_left_right_context")
    {
        auto config = SpeechConfig::FromEndpoint(endpoint, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        config->SetServiceProperty("format", "corrections", ServicePropertyChannel::UriQueryParameter);
        config->EnableDictation();

        //possible options for punctuation are implicit/explicit/intelligent/none.
        config->SetServiceProperty("punctuation", "none", ServicePropertyChannel::UriQueryParameter);

        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        recognizer->SetAuthorizationToken("abc");
        recognizer->Properties.SetProperty("DictationInsertionPointLeft", "This is up to 100 characters of text that precedes the insertion \" \n { ] , point.");
        recognizer->Properties.SetProperty("DictationInsertionPointRight", "This is up to 100 characters of text that follows the insertion point.");

        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(recognizer.get(), result);
        recognizer->StartContinuousRecognitionAsync().get();
        PushData(pushStream.get(), ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        recognizer->StopContinuousRecognitionAsync().get();

        SPXTEST_REQUIRE(!result->phrases.empty());
        auto detailedResult = result->phrases[0].Json;
        SPXTEST_REQUIRE(detailedResult.find("NBest") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("ITN") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Lexical") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("MaskedITN") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Display") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Corrections") != string::npos);
    }
    SPXTEST_SECTION("empty_left_right_context")
    {
        auto config = SpeechConfig::FromEndpoint(endpoint, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        config->SetServiceProperty("format", "corrections", ServicePropertyChannel::UriQueryParameter);
        config->EnableDictation();

        //possible options for punctuation are implicit/explicit/intelligent/none.
        config->SetServiceProperty("punctuation", "none", ServicePropertyChannel::UriQueryParameter);

        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        recognizer->SetAuthorizationToken("abc");
        recognizer->Properties.SetProperty("DictationInsertionPointLeft", "");
        recognizer->Properties.SetProperty("DictationInsertionPointRight", "");

        auto result = make_shared<RecoPhrases>();
        ConnectCallbacks(recognizer.get(), result);
        recognizer->StartContinuousRecognitionAsync().get();
        PushData(pushStream.get(), ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
        WaitForResult(result->ready.get_future(), WAIT_FOR_RECO_RESULT_TIME);
        recognizer->StopContinuousRecognitionAsync().get();

        SPXTEST_REQUIRE(!result->phrases.empty());
        auto detailedResult = result->phrases[0].Json;
        SPXTEST_REQUIRE(detailedResult.find("NBest") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("ITN") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Lexical") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("MaskedITN") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Display") != string::npos);
        SPXTEST_REQUIRE(detailedResult.find("Corrections") != string::npos);
    }
}

TEST_CASE("Verify auto detect source language config in SpeechRecognizer", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    SPXTEST_SECTION("auto detect source language config with a vector of string parameters")
    {
        auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "de-DE" });
        auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig(), autoDetectSourceLanguageConfig, audioConfig);
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages) == "en-US,de-DE");
    }

    SPXTEST_SECTION("auto detect source language config with source language config")
    {
        std::vector<std::shared_ptr<SourceLanguageConfig>> sourceLanguageConfigs;
        sourceLanguageConfigs.push_back(SourceLanguageConfig::FromLanguage("en-US"));
        sourceLanguageConfigs.push_back(SourceLanguageConfig::FromLanguage("fr-FR", "CustomId2"));
        auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs(sourceLanguageConfigs);
        auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig(), autoDetectSourceLanguageConfig, audioConfig);
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages) == "en-US,fr-FR");
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty("en-USSPEECH-ModelId") == "");
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty("fr-FRSPEECH-ModelId") == "CustomId2");
    }

    SPXTEST_SECTION("auto detect source language scenario doesn't support single endpointId setting")
    {
        auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "de-DE", "fr-FR" });
        auto speechConfig = CurrentSpeechConfig();
        speechConfig->SetEndpointId("CustomEndpoint1");
        REQUIRE_THROWS_WITH(SpeechRecognizer::FromConfig(speechConfig, autoDetectSourceLanguageConfig, audioConfig),
            Catch::Contains("EndpointId on SpeechConfig is unsupported for auto detection source language scenario."));
    }
}

TEST_CASE("Verify source language config in SpeechRecognizer", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    SPXTEST_SECTION("source language")
    {
        auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig(), "zh-CN", audioConfig);
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_RecoLanguage) == "zh-CN");
    }

    SPXTEST_SECTION("source language config")
    {
        auto sourceLanguageConfig = SourceLanguageConfig::FromLanguage("de-DE");
        auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig(), sourceLanguageConfig, audioConfig);
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_RecoLanguage) == "de-DE");
    }

    SPXTEST_SECTION("source language config with custom endpoint id")
    {
        auto sourceLanguageConfig = SourceLanguageConfig::FromLanguage("de-DE", "CustomId");
        auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig(), sourceLanguageConfig, audioConfig);
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_RecoLanguage) == "de-DE");
        SPXTEST_REQUIRE(recognizer->Properties.GetProperty(PropertyId::SpeechServiceConnection_EndpointId) == "CustomId");
    }
}

TEST_CASE("Verify auto detect source language result from speech recognition result", "[api][cxx]") {
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    auto expectedLanguage = "";
    std::shared_ptr<SpeechRecognitionResult> speechRecognitionResult;
    ResultReason expectedReason = ResultReason::RecognizedSpeech;

    SPXTEST_SECTION("Non Language Id Scenario")
    {
        auto sourceLanguageConfig = SourceLanguageConfig::FromLanguage("en-US");
        auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig(), sourceLanguageConfig, audioConfig);
        speechRecognitionResult = recognizer->RecognizeOnceAsync().get();
    }

    SPXTEST_SECTION("Language Id Scenario")
    {
        auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "de-DE" });
        auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig(), autoDetectSourceLanguageConfig, audioConfig);
        speechRecognitionResult = recognizer->RecognizeOnceAsync().get();
        expectedLanguage = "en-US";
    }

    SPXTEST_SECTION("Language Id with Invalid source languages")
    {
        auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "en-US", "notrecognized" });
        auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig(), autoDetectSourceLanguageConfig, audioConfig);
        speechRecognitionResult = recognizer->RecognizeOnceAsync().get();
        REQUIRE(speechRecognitionResult != nullptr);
        expectedReason = ResultReason::Canceled;
        auto cancellation = CancellationDetails::FromResult(speechRecognitionResult);
        REQUIRE(cancellation->Reason == CancellationReason::Error);
    }

    SPXTEST_SECTION("Language Id Scenario From Multiple SourceLanguageConfig")
    {
        std::vector<std::shared_ptr<SourceLanguageConfig>> sourceLanguageConfigs;
        sourceLanguageConfigs.push_back(SourceLanguageConfig::FromLanguage("zh-CN"));
        sourceLanguageConfigs.push_back(SourceLanguageConfig::FromLanguage("en-US"));
        auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs(move(sourceLanguageConfigs));
        auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig(), autoDetectSourceLanguageConfig, audioConfig);
        speechRecognitionResult = recognizer->RecognizeOnceAsync().get();
        expectedLanguage = "en-US";
    }

    SPXTEST_REQUIRE(speechRecognitionResult != nullptr);
    auto autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult::FromResult(speechRecognitionResult);
    SPXTEST_REQUIRE(autoDetectSourceLanguageResult != nullptr);
    if (speechRecognitionResult->Reason != expectedReason) {
        auto cancellationDetails = CancellationDetails::FromResult(speechRecognitionResult);
        SPX_TRACE_VERBOSE("cxx_api_Test CANCELED: ErrorCode=%d, ErrorDetails=%s", (int)cancellationDetails->ErrorCode, cancellationDetails->ErrorDetails.c_str());
    }
    if (speechRecognitionResult->Reason == expectedReason) {
        SPXTEST_REQUIRE(autoDetectSourceLanguageResult->Language == expectedLanguage);
    }
    else {
        SPX_TRACE_VERBOSE("cxx_api_Test result reson %d is not expected %d", speechRecognitionResult->Reason, expectedReason);
    }
}

TEST_CASE("Verify language id detection for continous speech recognition", "[api][cxx]") {
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_GERMAN)));
    auto audioConfig = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_GERMAN));
    auto autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig::FromLanguages({ "de-DE", "fr-FR" });
    auto recognizer = SpeechRecognizer::FromConfig(CurrentSpeechConfig(), autoDetectSourceLanguageConfig, audioConfig);

    promise<void> complete;
    shared_future<void> readyFuture(complete.get_future());
    vector<string> recognizingResults;
    vector<string> recognizedResults;
    vector<string> translatedResults;
    vector<string> errorResults;
    recognizer->Recognizing.DisconnectAll();
    recognizer->Recognizing.Connect([&recognizingResults, &errorResults, &complete, &readyFuture](const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizingSpeech)
        {
            recognizingResults.push_back(e.Result->Text);
            auto languageDetectionResult = AutoDetectSourceLanguageResult::FromResult(e.Result);
            if (languageDetectionResult->Language != "de-DE")
            {
                errorResults.push_back("Language detection failed, detection result is " + languageDetectionResult->Language);
            }
        }
        else
        {
            SPX_TRACE_VERBOSE("cxx_api_Test Recognizing failed: Reason:%d.", e.Result->Reason);
            errorResults.push_back("Recognizing error. Reason = " + std::to_string((int)e.Result->Reason));
            if (readyFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
            {
                complete.set_value();
            }
        }
    });

    recognizer->Recognized.DisconnectAll();
    recognizer->Recognized.Connect([&recognizedResults, &errorResults, &complete, &readyFuture](const SpeechRecognitionEventArgs& e)
    {
        if (e.Result->Reason == ResultReason::RecognizedSpeech)
        {
            recognizedResults.push_back(e.Result->Text);
            auto languageDetectionResult = AutoDetectSourceLanguageResult::FromResult(e.Result);
            if (languageDetectionResult->Language != "de-DE")
            {
                errorResults.push_back("Language detection failed, detection result is " + languageDetectionResult->Language);
            }
        }
        else
        {
            SPX_TRACE_VERBOSE("cxx_api_Test Recognized failed: Reason:%d.", e.Result->Reason);
            errorResults.push_back("Recognized error. Reason = " + std::to_string((int)e.Result->Reason));

            if (readyFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
            {
                complete.set_value();
            }
        }
    });

    recognizer->Canceled.DisconnectAll();
    recognizer->Canceled.Connect([&errorResults, &complete, &readyFuture](const SpeechRecognitionCanceledEventArgs& e)
    {
        if (e.Reason == CancellationReason::EndOfStream)
        {
            SPX_TRACE_VERBOSE("cxx_api_Test CANCELED: Reach the end of the file.");
        }
        else
        {
            auto error = !e.ErrorDetails.empty() ? e.ErrorDetails : "Errors!";
            SPX_TRACE_VERBOSE("cxx_api_Test CANCELED: ErrorCode=%d, ErrorDetails=%s", (int)e.ErrorCode, e.ErrorDetails.c_str());
            errorResults.push_back(error);
            if (readyFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
            {
                complete.set_value();
            }
        }
    });

    recognizer->SessionStopped.DisconnectAll();
    recognizer->SessionStopped.Connect([&complete, &readyFuture](const SessionEventArgs& e)
    {
        SPX_TRACE_VERBOSE("cxx_api_Test SessionStopped: session id %s", e.SessionId.c_str());
        if (readyFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
        {
            complete.set_value();
        }
    });

    recognizer->StartContinuousRecognitionAsync().get();
    auto status = readyFuture.wait_for(WAIT_FOR_RECO_RESULT_TIME);
    REQUIRE(status == future_status::ready);
    recognizer->StopContinuousRecognitionAsync().get();

    REQUIRE(errorResults.size() == 0);
    REQUIRE(recognizingResults.size() > 0);
    REQUIRE(recognizedResults.size() > 0);
    REQUIRE(recognizedResults[0] == AudioUtterancesMap[SINGLE_UTTERANCE_GERMAN].Utterances["de"][0].Text);
}

TEST_CASE("Custom speech-to-text endpoints", "[api][cxx]")
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    string speechHost = "wss://" + SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region + ".stt.speech.microsoft.com";

    SPXTEST_SECTION("Host only")
    {
        string host = speechHost;
        auto config = SpeechConfig::FromHost(speechHost, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
    }

    SPXTEST_SECTION("Host with root path") // parsed as empty path
    {
        string host = speechHost + "/";
        auto config = SpeechConfig::FromHost(host, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
    }

    SPXTEST_SECTION("Host with parameters") // not allowed
    {
        string host = speechHost + "?format=detailed";
        auto config = SpeechConfig::FromHost(host, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);

        auto cancellation = CancellationDetails::FromResult(result);
        SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::Error);
        SPXTEST_REQUIRE(cancellation->ErrorCode == CancellationErrorCode::ConnectionFailure);
        CAPTURE(cancellation->ErrorDetails);
        SPXTEST_REQUIRE(cancellation->ErrorDetails.find("Invalid argument exception: Query parameters are not allowed in the host URI.") != std::string::npos);
    }

    SPXTEST_SECTION("Host with non-root path") // not allowed
    {
        string host = speechHost + "/speech/recognition/interactive/cognitiveservices/v1";
        auto config = SpeechConfig::FromHost(host, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);

        auto cancellation = CancellationDetails::FromResult(result);
        SPXTEST_REQUIRE(cancellation->Reason == CancellationReason::Error);
        SPXTEST_REQUIRE(cancellation->ErrorCode == CancellationErrorCode::ConnectionFailure);
        CAPTURE(cancellation->ErrorDetails);
        SPXTEST_REQUIRE(cancellation->ErrorDetails.find("Invalid argument exception: Resource path is not allowed in the host URI.") != std::string::npos);
    }

    SPXTEST_SECTION("Invalid url from portal")
    {
        const auto speechEndpoint = "https://" + SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Region + ".api.cognitive.microsoft.com/sts/v1.0/issueToken";
        auto config = SpeechConfig::FromEndpoint(speechEndpoint, SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text));
    }
}

TEST_CASE("Local speech-to-text endpoints", "[.][api][cxx]") // for manual testing of speech service containers
{
    SPX_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    SPXTEST_SECTION("Host only")
    {
        string host = "ws://localhost:5000";
        auto config = SpeechConfig::FromHost(host);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text));
    }

    SPXTEST_SECTION("Host with root path")
    {
        string host = "ws://localhost:5000/";
        auto config = SpeechConfig::FromHost(host);
        auto recognizer = SpeechRecognizer::FromConfig(config, audioInput);

        auto result = recognizer->RecognizeOnceAsync().get();
        SPXTEST_REQUIRE(result != nullptr);
        SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);
        SPXTEST_REQUIRE(StringComparisions::AssertFuzzyMatch(result->Text, AudioUtterancesMap[SINGLE_UTTERANCE_ENGLISH].Utterances["en-US"][0].Text));
    }
}

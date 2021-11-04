//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

// <toplevel>
#include <string>
#include <vector>
#include <speechapi_cxx.h>
#include "wav_file_reader.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Speaker;
// </toplevel>

const string audioDirName{ "..\\..\\..\\..\\..\\SampleData\\audiofiles\\" };

// AudioInputFromFileCallback implements PullAudioInputStreamCallback interface, and uses a wav file as source
class AudioInputFromFileCallback final : public PullAudioInputStreamCallback
{
public:
    // Constructor that creates an input stream from a file.
    AudioInputFromFileCallback(const string& audioFileName)
        : m_reader(audioFileName)
    {
    }

    // Implements AudioInputStream::Read() which is called to get data from the audio stream.
    // It copies data available in the stream to 'dataBuffer', but no more than 'size' bytes.
    // If the data available is less than 'size' bytes, it is allowed to just return the amount of data that is currently available.
    // If there is no data, this function must wait until data is available.
    // It returns the number of bytes that have been copied in 'dataBuffer'.
    // It returns 0 to indicate that the stream reaches end or is closed.
    int Read(uint8_t* dataBuffer, uint32_t size) override
    {
        return m_reader.Read(dataBuffer, size);
    }
    // Implements AudioInputStream::Close() which is called when the stream needs to be closed.
    void Close() override
    {
        m_reader.Close();
    }

private:
    WavFileReader m_reader;
};

// helper functions
shared_ptr<VoiceProfile> VoiceProfileEnrollmentWithMicrophone(const shared_ptr<VoiceProfileClient>& client);
void VerifyVoiceProfileFromMicrophone(const shared_ptr<SpeechConfig>& config, const shared_ptr<VoiceProfile>& profile);
int PushData(const string& filename, shared_ptr<PushAudioInputStream>& pushStream);
void VerifyVoiceProfileWithPushStream(const shared_ptr<SpeechConfig>& config, const shared_ptr<VoiceProfile>& profile);
void VoiceProfileIdentificationWithPullStream(const shared_ptr<SpeechConfig>& config, const vector<shared_ptr<VoiceProfile>>& profiles);
void VoiceProfileIdentificationWithMicrophone(const shared_ptr<SpeechConfig>& config, const vector<shared_ptr<VoiceProfile>>& profiles);
shared_ptr<VoiceProfile> VoiceProfileEnrollmentWithMicrophone(const shared_ptr<VoiceProfileClient>& client);
shared_ptr<VoiceProfile> VoiceProfileEnrollmentWithPullStream(const shared_ptr<VoiceProfileClient>& client, const string& filename);

// Speaker verification with microphone input.
void SpeakerVerificationWithMicrophone()
{
    // <SpeakerVerificationWithMicrophone>
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a VoiceProfileClient to create voice profiles and train voice profiles.
    auto client = VoiceProfileClient::FromConfig(config);

    // Creates a voice profile using the client.
    auto profile = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    cout << "Created a text dependent verification profile " << profile->GetId() << endl;

    uint64_t remainingEnrollmentCount = UINT64_MAX;
    bool enrolled = false;
    while (remainingEnrollmentCount != 0)
    {
        cout << "Say the pass-phrase to enroll...";
        // Enrolls the voice profile using the audio from the default microphone.
        auto result = client->EnrollProfileAsync(profile, AudioConfig::FromDefaultMicrophoneInput()).get();

        // After the voice profile has been successfully enrolled, you can start verifying your voice.
        if (result->Reason == ResultReason::EnrolledVoiceProfile)
        {
            cout << "Enrolled.\n";
            enrolled = true;
            break;
        }
        // More audio are needed to enroll the speaker.
        else if (result->Reason == ResultReason::EnrollingVoiceProfile)
        {
            remainingEnrollmentCount = result->GetEnrollmentInfo(EnrollmentInfoType::RemainingEnrollmentsCount);
            cout << "Accepted " << result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsCount) << " Utterances.\n";
        }
        // Something went wrong while enrolling the speaker.
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = VoiceProfileEnrollmentCancellationDetails::FromResult(result);
            cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            break;
        }
    }

    //  Verify the voice profile.
    if (enrolled)
    {
        VerifyVoiceProfileFromMicrophone(config, profile);
    }
    // </SpeakerVerificationWithMicrophone>
}

// Speaker verification with push stream.
void SpeakerVerificationWithPushStream()
{
    // <SpeakerVerificationWithPushStream>
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a VoiceProfileClient to create voice profiles and train voice profiles.
    auto client = VoiceProfileClient::FromConfig(config);

    // Creates a voice profile using the client.
    auto profile = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    cout << "Created a text dependent verification profile " << profile->GetId() << endl;

    // The source of the push streams is three recorded audio files.
    vector<string> trainingFilenames { audioDirName+"myVoiceIsMyPassportVerifyMe01.wav", audioDirName+"myVoiceIsMyPassportVerifyMe02.wav", audioDirName + "myVoiceIsMyPassportVerifyMe03.wav" };
    bool enrolled = false;

    // for each audio file, create a push stream and feed it to the voice profile client.
    for (auto& trainingFilename: trainingFilenames)
    {
        // Creates a push stream
        auto pushStream = AudioInputStream::CreatePushStream();

        // Creates an audio config object from stream input;
        auto audioInput = AudioConfig::FromStreamInput(pushStream);

        // Pushes audio into the voice profile client.
        auto error = PushData(trainingFilename, pushStream);
        if (error)
        {
            return;
        }

        // Enrolls the voice profile using the push stream
        auto result = client->EnrollProfileAsync(profile, audioInput).get();

        if (result->Reason == ResultReason::EnrolledVoiceProfile)
        {
            cout << "Enrolled.\n";
            enrolled = true;
            break;
        }
        // More audio are needed to enroll the voice profile.
        else if (result->Reason == ResultReason::EnrollingVoiceProfile)
        {
            cout << "RemainingEnrollmentCount " << result->GetEnrollmentInfo(EnrollmentInfoType::RemainingEnrollmentsCount) << endl;
            cout << "Accepted " << result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsCount) << " Utterances.\n";
        }
        // Something went wrong while enrolling the voice profile.
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = VoiceProfileEnrollmentCancellationDetails::FromResult(result);
            cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            break;
        }
    }

    // After the voice profile has been successfully enrolled, you can start verifying your voice.
    if (enrolled)
    {
        VerifyVoiceProfileWithPushStream(config, profile);
    }
    // </SpeakerVerificationWithPushStream>
}

// Speaker identification with audio input from a pull stream.
void SpeakerIdentificationWithPullStream()
{
    // <SpeakerIdentificationWithPullStream>
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a VoiceProfileClient to create voice profiles and train voice profiles.
    auto client = VoiceProfileClient::FromConfig(config);

    // Creates and train two voice profiles.
    auto profile1 = VoiceProfileEnrollmentWithPullStream(client, audioDirName + "TalkForAFewSeconds16.wav");
    auto profile2 = VoiceProfileEnrollmentWithPullStream(client, audioDirName + "neuralActivationPhrase.wav");

    if (!profile1->GetId().empty() && !profile2->GetId().empty())
    {
        vector<shared_ptr<VoiceProfile>> profiles{ profile1, profile2 };
        VoiceProfileIdentificationWithPullStream(config, profiles);
    }
    // </SpeakerIdentificationWithPullStream>
}

// Speaker identification with audio input from microphone.
void SpeakerIdentificationWithMicrophone()
{
    // Creates an instance of a speech config with specified subscription key and service region.
        // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a VoiceProfileClient to create voice profiles and train voice profiles.
    auto client = VoiceProfileClient::FromConfig(config);

    // Creates and train two voice profiles.
    auto profile1 = VoiceProfileEnrollmentWithMicrophone(client);
    auto profile2 = VoiceProfileEnrollmentWithPullStream(client, audioDirName + "speechService.wav");

    if (!profile1->GetId().empty() && !profile2->GetId().empty())
    {
        vector<shared_ptr<VoiceProfile>> profiles{ profile1, profile2 };
        VoiceProfileIdentificationWithMicrophone(config, profiles);
    }
}

// helper function for speaker verification.
void VerifyVoiceProfileFromMicrophone(const shared_ptr<SpeechConfig>& config, const shared_ptr<VoiceProfile>& profile)
{
    // Creates a speaker recognizer using microphone as audio input.
    auto recognizer = SpeakerRecognizer::FromConfig(config, AudioConfig::FromDefaultMicrophoneInput());

    // Creates a model from the voice profile.
    auto model = SpeakerVerificationModel::FromProfile(profile);

    cout << "Verifying your voice by say something..\n";
    // Verify the voice profile using the speaker recognizer.
    auto result = recognizer->RecognizeOnceAsync(model).get();

    // The voice profile is being recognized.
    if (result->Reason == ResultReason::RecognizedSpeaker)
    {
        cout << "Verified the voice profile " << result->ProfileId << ". The score is " << result->GetScore() << endl;
    }
    // The voice profile is being rejected.
    else if (result->Reason == ResultReason::NoMatch )
    {
        cout << "Rejected the voice profile. Its core is " << result->GetScore() << endl;
        cout << "The service response is " << result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult) << endl;
    }
    // something went wrong while verifying the voice profile.
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = SpeakerRecognitionCancellationDetails::FromResult(result);
        cout << "CANCELED " << profile->GetId() << "ErrorCode= " << (int)cancellation->ErrorCode << endl;
        cout << "CANCELED " << profile->GetId() << "ErrorDetails= " << cancellation->ErrorDetails << endl;
    }
}

// helper function for push data.
int PushData(const string& filename, shared_ptr<PushAudioInputStream>& pushStream)
{
    try
    {
        WavFileReader reader(filename);

        vector<uint8_t> buffer(1000);
        // Read data and push them into the stream
        int readSamples = 0;
        while ((readSamples = reader.Read(buffer.data(), (uint32_t)buffer.size())) != 0)
        {
            // Push a buffer into the stream
            pushStream->Write(buffer.data(), readSamples);
        }

        // Close the push stream.
        pushStream->Close();
    }
    catch (const exception& e)
    {
        cout << "Error in pushing audio stream. " << e.what() << endl;
        return 1;
    }

    return 0;
}

// helper function for verify voice profile with push stream.
void VerifyVoiceProfileWithPushStream(const shared_ptr<SpeechConfig>& config, const shared_ptr<VoiceProfile>& profile)
{
    // Creates a model from the voice profile.
    auto model = SpeakerVerificationModel::FromProfile(profile);

    // Creates a push stream
    auto pushStream = AudioInputStream::CreatePushStream();

    // Creates an audio config object from stream input;
    auto audioInput = AudioConfig::FromStreamInput(pushStream);

    // Creates a speaker recognizer using microphone as audio input.
    auto recognizer = SpeakerRecognizer::FromConfig(config, audioInput);

    auto error = PushData(audioDirName + "myVoiceIsMyPassportVerifyMe04.wav", pushStream);
    if (error)
    {
        return;
    }

    // Verify the voice profile using the speaker recognizer.
    auto result = recognizer->RecognizeOnceAsync(model).get();

    // The voice profile is being recognized.
    if (result->Reason == ResultReason::RecognizedSpeaker)
    {
        cout << "Verified the voice profile " << result->ProfileId << ". The score is " << result->GetScore() << endl;
    }
    // The voice profile is being rejected.
    else if (result->Reason == ResultReason::NoMatch)
    {
        cout << "Rejected the voice profile. Its core is " << result->GetScore() << endl;
        cout << "The service response is " << result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult) << endl;
    }
    // something went wrong while verifying the voice profile.
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = SpeakerRecognitionCancellationDetails::FromResult(result);
        cout << "CANCELED " << profile->GetId() << "ErrorCode= " << (int)cancellation->ErrorCode << endl;
        cout << "CANCELED " << profile->GetId() << "ErrorDetails= " << cancellation->ErrorDetails << endl;
    }
}

// helper function for voice profile enrollment with a pull stream.
shared_ptr<VoiceProfile> VoiceProfileEnrollmentWithPullStream (const shared_ptr<VoiceProfileClient>& client, const string& filename)
{
    // Creates a voice profile using the client.
    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    cout << "Created a text independent identification profile " << profile->GetId() << endl;

    // Creates a callback that will read audio data from a WAV file.
    // Currently, the only supported WAV format is mono(single channel), 16 kHZ sample rate, 16 bits per sample.
    // Replace with your own audio file name.
    auto callback = make_shared<AudioInputFromFileCallback>(filename);
    auto pullStream = AudioInputStream::CreatePullStream(callback);

    // Creates an audio config object from stream input;
    auto audioInput = AudioConfig::FromStreamInput(pullStream);

    auto reason = ResultReason::EnrollingVoiceProfile;
    while (reason == ResultReason::EnrollingVoiceProfile)
    {
        // Enrolls the voice profile using the push stream
        auto result = client->EnrollProfileAsync(profile, audioInput).get();

        // After the voice profile has been successfully enrolled, you can start verifying your voice.
        if (result->Reason == ResultReason::EnrolledVoiceProfile)
        {
            cout << "Enrolled.\n";
        }
        // More audio are needed to enroll the speaker.
        else if (result->Reason == ResultReason::EnrollingVoiceProfile)
        {
            cout << "RemainingEnrollmentsSpeechLength in hundred nanosecond: " << result->GetEnrollmentInfo(EnrollmentInfoType::RemainingEnrollmentsSpeechLength) << endl;
        }
        // Something went wrong while enrolling the speaker.
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = VoiceProfileEnrollmentCancellationDetails::FromResult(result);
            cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
        }
        reason = result->Reason;
    }

    return profile;
}

// helper function for voice profile identification with a pull stream.
void VoiceProfileIdentificationWithPullStream(const shared_ptr<SpeechConfig>& config, const vector<shared_ptr<VoiceProfile>>& profiles)
{
    // Create a callback that will be called by the Speech SDK during identification, aka SpeakerRecognizer::RecognizeOnceAsync.
    auto callback = make_shared<AudioInputFromFileCallback>(audioDirName + "wikipediaOcelot.wav");
    auto pullStream = AudioInputStream::CreatePullStream(callback);

    // Creates an audio config object from stream input;
    auto audioInput = AudioConfig::FromStreamInput(pullStream);

    // Creates a speaker recognizer.
    auto speakerRecognizer = SpeakerRecognizer::FromConfig(config, audioInput);

    // Creates a speakerIdentificationModel.
    auto model = SpeakerIdentificationModel::FromProfiles(profiles);

    // Recognizing the speaker in the audio input among all the speakers in the model.
    auto result = speakerRecognizer->RecognizeOnceAsync(model).get();

    // Recognized the speaker.
    if (result->Reason == ResultReason::RecognizedSpeakers)
    {
        cout << "The most similar voice profile is " << result->ProfileId << " with similarity score " << result->GetScore() << endl;
        auto raw = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
        cout << "The raw json from the service is " << raw << endl;
    }
    // No speaker recognized.
    else if (result->Reason == ResultReason::NoMatch)
    {
        auto raw = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
        cout << "NoMatch: The raw json from the service is " << raw << endl;
    }
    // Something went wrong while recognizing the speaker.
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = SpeakerRecognitionCancellationDetails::FromResult(result);
        cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
        cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
    }
}

// helper function for voice profile identification with the default microphone.
void VoiceProfileIdentificationWithMicrophone(const shared_ptr<SpeechConfig>& config, const vector<shared_ptr<VoiceProfile>>& profiles)
{
    // Creates a speaker recognizer with audio input from microphone.
    auto speakerRecognizer = SpeakerRecognizer::FromConfig(config, AudioConfig::FromDefaultMicrophoneInput());

    // Creates a speakerIdentificationModel.
    auto model = SpeakerIdentificationModel::FromProfiles(profiles);

    cout << "Say something to identify myself...";
    // Recognizing the speaker in the audio input among all the speakers in the model.
    auto result = speakerRecognizer->RecognizeOnceAsync(model).get();

    // Recognized the speaker.
    if (result->Reason == ResultReason::RecognizedSpeakers)
    {
        cout << "The most similar voice profile is " << result->ProfileId << " with similarity score " << result->GetScore() << endl;
        auto raw = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
        cout << "The raw json from the service is " << raw << endl;
    }
    // Something went wrong while recognizing the speaker.
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = SpeakerRecognitionCancellationDetails::FromResult(result);
        cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
        cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
    }
}

// helper function for voice profile enrollment with the default microphone.
shared_ptr<VoiceProfile> VoiceProfileEnrollmentWithMicrophone(const shared_ptr<VoiceProfileClient>& client)
{
    // Creates a voice profile using the client.
    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    cout << "Created a text independent identification profile " << profile->GetId() << endl;

    // Creates an audio config object from the default microphone.
    auto audioInput = AudioConfig::FromDefaultMicrophoneInput();

    // Get list of accepted activation phrases for enrollment
    auto phrasesResult = client->GetActivationPhrasesAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    auto phrases = phrasesResult->GetPhrases();
    if (phrases->size() <= 0)
    {
        cout << "No activation phrases received. Aborting..." << std::endl;
        return profile;
    }

    auto reason = ResultReason::EnrollingVoiceProfile;
    // speak clearly to microphone. The loop exits when the voice profile is enrolled or something went wrong.
    while (reason == ResultReason::EnrollingVoiceProfile)
    {
       
        cout << "Say '" << phrases->at(0) << "' to train your voice profile...";
        // Enrolls the voice profile using the push stream
        auto result = client->EnrollProfileAsync(profile, audioInput).get();

        // After the voice profile has been successfully enrolled, you can start verifying your voice.
        if (result->Reason == ResultReason::EnrolledVoiceProfile)
        {
            cout << "Enrolled.\n";
            break;
        }
        // More audio are needed to enroll the speaker.
        else if (result->Reason == ResultReason::EnrollingVoiceProfile)
        {
            cout << "RemainingEnrollmentsSpeechLength in hundred nanosecond: " << result->GetEnrollmentInfo(EnrollmentInfoType::RemainingEnrollmentsSpeechLength) << endl;
        }
        // Something went wrong while enrolling the speaker.
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = VoiceProfileEnrollmentCancellationDetails::FromResult(result);
            cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << endl;
            cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << endl;
            break;
        }
        reason = result->Reason;
    }
    return profile;
}

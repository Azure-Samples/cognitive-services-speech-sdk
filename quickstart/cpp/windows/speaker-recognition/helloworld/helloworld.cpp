//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
// <code>
#include <iostream>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Speaker;

// Enroll a given profile, and print the resulting id if successful
void enrollProfile(std::shared_ptr<VoiceProfileClient> client, std::shared_ptr <VoiceProfile> profile, std::shared_ptr <AudioConfig> audioConfig)
{
   ResultReason reason = ResultReason::EnrollingVoiceProfile;
   std::shared_ptr<VoiceProfileEnrollmentResult> result;

   // 20 seconds of spoken audio is required for a valid enrollment, loop until the service completes enrollment
   while (reason == ResultReason::EnrollingVoiceProfile)
   {
       result = client->EnrollProfileAsync(profile, audioConfig).get();
       reason = result->Reason;
   }
   if (reason == ResultReason::EnrolledVoiceProfile)
   {
       cout << "Enrolled profile " << profile->GetId() << endl;
   }
   else if (reason == ResultReason::Canceled)
   {
       cout << "Profile enrollment failed!" << endl;
       auto cancellation = CancellationDetails::FromResult(result);
       cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

       if (cancellation->Reason == CancellationReason::Error) {
          cout << "CANCELED: ErrorCode= " << (int)cancellation->ErrorCode << std::endl;
          cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
       }
   }
   else
   {
       cout << "Profile enrollment failed, result: " << result->Text << endl;
   }
}

// Speaker verification using audio sample file
void verifySpeaker(const shared_ptr<SpeechConfig>& config, const shared_ptr<VoiceProfile>& profile)
{
    auto speakerRecognizer = SpeakerRecognizer::FromConfig(config, AudioConfig::FromWavFileInput("myVoiceIsMyPassportVerifyMe04.wav"));

    // Creates the model from the profile to be verified against
    auto model = SpeakerVerificationModel::FromProfile(profile);

    // Verify the given audio sample
    auto result = speakerRecognizer->RecognizeOnceAsync(model).get();
    if (result->Reason == ResultReason::RecognizedSpeaker)
    {
        cout << "Verified voice profile " << result->ProfileId << " score is " << result->GetScore() << endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = SpeakerRecognitionCancellationDetails::FromResult(result);
        cout << "CANCELED " << profile->GetId() << "ErrorCode= " << (int)cancellation->ErrorCode << endl;
        cout << "CANCELED " << profile->GetId() << "ErrorDetails= " << cancellation->ErrorDetails << endl;
    }
}

// Speaker profile creation, enrollment, and verification using audio sample files
void speakerVerification()
{
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a VoiceProfileClient to enroll your voice profile.
    auto client = VoiceProfileClient::FromConfig(config);

    // Creates a text dependent voice profile in one of the supported locales using the client.
    auto profile = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    auto trainingFiles = vector<string>{ "myVoiceIsMyPassportVerifyMe01.wav", "myVoiceIsMyPassportVerifyMe02.wav", "myVoiceIsMyPassportVerifyMe03.wav" };
    for (auto& trainingFile : trainingFiles)
    {
        auto audioInput = AudioConfig::FromWavFileInput(trainingFile);

        // Enrolls the profile using each training file in turn
        auto result = client->EnrollProfileAsync(profile, audioInput).get();
        if (result->Reason == ResultReason::EnrollingVoiceProfile)
        {
            cout << "Enrolling profile id " << profile->GetId() << endl;
        }
        else if (result->Reason == ResultReason::EnrolledVoiceProfile)
        {
            cout << "Enrolled profile id " << profile->GetId() << endl;

            // Verifies the enrolled profile
            verifySpeaker(config, profile);
            break;
        }
        else if (result->Reason == ResultReason::Canceled)
        {
            auto cancellation = VoiceProfileEnrollmentCancellationDetails::FromResult(result);
            cout << "CANCELED " << profile->GetId() << "ErrorCode= " << (int)cancellation->ErrorCode << endl;
            break;
        }
        cout << "Number of enrollment audios accepted for " << profile->GetId() << " is "<< result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsCount) << endl;
        cout << "Number of enrollment audios needed to complete " << profile->GetId() << " is " << result->GetEnrollmentInfo(EnrollmentInfoType::RemainingEnrollmentsCount) << endl;
    }

    if (!profile->GetId().empty())
    {
        client->DeleteProfileAsync(profile).get();
    }
}

// Speaker identification using audio sample file
void identifySpeakers(const shared_ptr<SpeechConfig>& config, const vector<shared_ptr<VoiceProfile>>& profiles)
{
    auto speakerRecognizer = SpeakerRecognizer::FromConfig(config, AudioConfig::FromWavFileInput("TalkForAFewSeconds16.wav"));

    // Creates the model from the profile to be verified against
    auto model = SpeakerIdentificationModel::FromProfiles(profiles);

    // Identify the given audio sample
    auto result = speakerRecognizer->RecognizeOnceAsync(model).get();
    if (result->Reason == ResultReason::RecognizedSpeakers)
    {
        cout << "The most similar voice profile is " << result->ProfileId << " with similarity score " << result->GetScore() << endl;
        auto raw = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
        cout << "The raw json from the service is " << raw << endl;
    }
}

// Speaker profile creation, enrollment, and identification using audio sample files
void speakerIdentification()
{
   // Creates an instance of a speech config with specified subscription key and service region.
   // Replace with your own subscription key and service region (e.g., "westus").
   auto config = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

   // Creates a VoiceProfileClient to enroll your voice profile.
   auto client = VoiceProfileClient::FromConfig(config);

   // Creates two text independent voice profiles in one of the supported locales.
   auto profile1 = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
   auto profile2 = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
   cout << "Created profiles " << profile1->GetId() << " and " << profile2->GetId() << " for text independent identification." << endl;

   // Enroll the two profiles
   enrollProfile(client, profile1, AudioConfig::FromWavFileInput("TalkForAFewSeconds16.wav"));
   enrollProfile(client, profile2, AudioConfig::FromWavFileInput("neuralActivationPhrase.wav"));

   // Identify the two profiles after successful enrollments.
   vector<shared_ptr<VoiceProfile>> profiles{ profile1, profile2 };
   identifySpeakers(config, profiles);

   // delete the two profiles after we are done.
   if (!profile1->GetId().empty())
   {
       client->DeleteProfileAsync(profile1).get();
   }
   if (!profile2->GetId().empty())
   {
       client->DeleteProfileAsync(profile2).get();
   }
}

int main()
{
    try
    {
        cout << "Speaker Verification:";
        speakerVerification();

        cout << "\nSpeaker Identification:";
        speakerIdentification();
    }
    catch (const exception& e)
    {
        cout << e.what();
    }
    cout << "Please press a key to continue.\n";
    cin.get();
    return 0;
}
// </code>

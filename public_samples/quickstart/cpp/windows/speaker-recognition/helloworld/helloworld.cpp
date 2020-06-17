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

void verifySpeaker(const shared_ptr<SpeechConfig>& config, const shared_ptr<VoiceProfile>& profile)
{
    auto speakerRecognizer = SpeakerRecognizer::FromConfig(config, AudioConfig::FromWavFileInput("myVoiceIsMyPassportVerifyMe04.wav"));
    auto model = SpeakerVerificationModel::FromProfile(profile);
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
        auto result = client->EnrollProfileAsync(profile, audioInput).get();
        if (result->Reason == ResultReason::EnrollingVoiceProfile)
        {
            cout << "Enrolling profile id " << profile->GetId() << endl;
        }
        else if (result->Reason == ResultReason::EnrolledVoiceProfile)
        {
            cout << "Enrolled profile id " << profile->GetId() << endl;
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

void identifySpeakers(const shared_ptr<SpeechConfig>& config, const vector<shared_ptr<VoiceProfile>>& profiles)
{
    auto speakerRecognizer = SpeakerRecognizer::FromConfig(config, AudioConfig::FromWavFileInput("wikipediaOcelot.wav"));
    auto model = SpeakerIdentificationModel::FromProfiles(profiles);
    auto result = speakerRecognizer->RecognizeOnceAsync(model).get();
    if (result->Reason == ResultReason::RecognizedSpeakers)
    {
        cout << "The most similar voice profile is " << result->ProfileId << " with similarity score " << result->GetScore() << endl;
        auto raw = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
        cout << "The raw json from the service is " << raw << endl;
    }
}

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
   auto result1 = client->EnrollProfileAsync(profile1, AudioConfig::FromWavFileInput("aboutSpeechSdk.wav")).get();
   cout << "Enrolled profile " << profile1->GetId() << endl;
   auto result2 = client->EnrollProfileAsync(profile2, AudioConfig::FromWavFileInput("speechService.wav")).get();
   cout << "Enrolled profile " << profile2->GetId() << endl;

   // Identify the two profiles after successful enrollments.
   if (result1->Reason == ResultReason::EnrolledVoiceProfile && result2->Reason == ResultReason::EnrolledVoiceProfile)
   {
       vector<shared_ptr<VoiceProfile>> profiles{ profile1, profile2 };
       identifySpeakers(config, profiles);
   }

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

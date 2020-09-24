//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <chrono>
#include <thread>

#include "test_utils.h"
#include "file_utils.h"
#include "recognizer_utils.h"

#include "speechapi_cxx_speaker_recognizer.h"
#include "speechapi_cxx_voice_profile_client.h"
#include "speechapi_cxx_voice_profile.h"
#include "speechapi_cxx_voice_profile_enrollment_result.h"
#include "speechapi_cxx_speaker_recognizer.h"
#include "speechapi_cxx_speaker_verification_model.h"
#include "speechapi_cxx_speaker_identification_model.h"

using namespace std::chrono_literals;
using namespace std;
using namespace Microsoft::CognitiveServices::Speech;

std::shared_ptr<SpeechConfig> GetSpeakerRecognitionProdSubscriptionConfig()
{
  // create a profile using a speech config
  auto subscriptionKey = SubscriptionsRegionsMap[SPEAKER_RECOGNITION_SUBSCRIPTION].Key;
  auto subscriptionRegion = SubscriptionsRegionsMap[SPEAKER_RECOGNITION_SUBSCRIPTION].Region;
  SPXTEST_REQUIRE(!subscriptionKey.empty());
  SPXTEST_REQUIRE(!subscriptionRegion.empty());
  auto config = SpeechConfig::FromSubscription(subscriptionKey, subscriptionRegion);
  SPXTEST_REQUIRE(config != nullptr);
  return config;
}

SPXTEST_CASE_BEGIN(
      "Speaker recognition::text independent verification enrollment",
      "[api][cxx][speaker_id][enrollment_ti_verification]")
{
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE)));

    // create a profile using a speech config
    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());
    // always delete the profile even when there are exceptions in following code. The lambda is called at when exits this test case.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE));
    auto result = client->EnrollProfileAsync(profile, audioInput).get();
    auto json_string = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
    SPXTEST_REQUIRE(!json_string.empty());
    INFO(result->ProfileId);
    SPXTEST_REQUIRE(result->ProfileId == profile->GetId());
    SPXTEST_REQUIRE(result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsCount) == 1);
    SPXTEST_REQUIRE(result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsSpeechLength) >= 239200000);
    SPXTEST_REQUIRE(result->Reason == ResultReason::EnrolledVoiceProfile);
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::text independent identication enrollment",
    "[api][cxx][speaker_id][enrollment_ti_identication]")
{
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE)));

    // create a profile using a speech config
    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());
    // always delete the profile even when there are exceptions in following code. The lambda is called at when exits this test case.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE));
    auto result = client->EnrollProfileAsync(profile, audioInput).get();
    auto json_string = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
    SPXTEST_REQUIRE(!json_string.empty());
    INFO(result->ProfileId);
    SPXTEST_REQUIRE(result->ProfileId == profile->GetId());
    SPXTEST_REQUIRE(result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsCount) == 1);
    SPXTEST_REQUIRE(result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsSpeechLength) >= 239200000);
    auto length = result->GetEnrollmentInfo(EnrollmentInfoType::RemainingEnrollmentsSpeechLength);
    INFO(length);
    SPXTEST_REQUIRE(result->Reason == ResultReason::EnrolledVoiceProfile);
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::text dependent verification enrollment bad request",
    "[api][cxx][speaker_id][enrollment_td_verfication_bad_request]")
{
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE)));

    // create a profile using a speech config
    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());
    // always delete the profile even when there are exceptions in following code.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE));
    auto result = client->EnrollProfileAsync(profile, audioInput).get();
    INFO(result->ProfileId);
    auto json_string = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
    SPXTEST_REQUIRE(json_string.find("Bad request") != std::string::npos);

    SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
    auto details = VoiceProfileEnrollmentCancellationDetails::FromResult(result);
    SPXTEST_REQUIRE(details->ErrorCode == CancellationErrorCode::BadRequest);
    SPXTEST_REQUIRE(details->ErrorDetails.find("Invalid audio length.") != std::string::npos);
    INFO(details->ErrorDetails);
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::reset voice profile text dependent verification",
    "[api][cxx][speaker_id][reset_td_ver]")
{
    // create a profile using a speech config
    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());
    // always delete the profile even when there are exceptions in following code.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });
    auto result = client->ResetProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::ResetVoiceProfile);
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::reset voice profile text independent verification",
    "[api][cxx][speaker_id][reset_ti_ver]")
{
    // create a profile using a speech config
    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());
    // always delete the profile even when there are exceptions in following code.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });
    auto result = client->ResetProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::ResetVoiceProfile);
    //note: when reset is successful, the content buffer in the response is empty. So, SpeechServiceResponse_JsonResult is empty.
    SPXTEST_REQUIRE(result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult).empty());
    SPXTEST_REQUIRE(!result->ResultId.empty());
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::reset voice profile text independent identification",
    "[api][speaker_id][cxx][reset_ti_iden]")
{
    // create a profile using a speech config
    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());

    auto result = client->ResetProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::ResetVoiceProfile);
    SPXTEST_REQUIRE(!result->ResultId.empty());

    result = client->DeleteProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::DeletedVoiceProfile);

    result = client->DeleteProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
    auto d = VoiceProfileCancellationDetails::FromResult(result);
    SPXTEST_REQUIRE(d->ErrorDetails.find("can't be found") != std::string::npos);

    // can't reset or reset without a voice type. it is a runtime error.
    auto fakeProfile = VoiceProfile::FromId("voice_profile_without_voice_type");
    result = client->ResetProfileAsync(fakeProfile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
    auto details = VoiceProfileCancellationDetails::FromResult(result);
    SPXTEST_REQUIRE(details->ErrorDetails.find("error") != std::string::npos);
    SPXTEST_REQUIRE(details->ErrorCode == CancellationErrorCode::RuntimeError);
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::delete voice profile text independent identification",
    "[api][cxx][speaker_id][delete_ti_iden]")
{
    // create a profile using a speech config
    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());

    auto result = client->DeleteProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::DeletedVoiceProfile);

    result = client->DeleteProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
    //auto raw = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult, "");
    auto d = VoiceProfileCancellationDetails::FromResult(result);
    SPXTEST_REQUIRE(d->ErrorDetails.find("can't be found") != std::string::npos);
    SPXTEST_REQUIRE(d->ErrorCode == CancellationErrorCode::ServiceError);
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::verification accepts and rejects speaker",
    "[api][cxx][speaker_id][verification]")
{
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE)));
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH)));

    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE));

    auto client = VoiceProfileClient::FromConfig(config);
    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());
    // always delete the profile even when there are exceptions in following code.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });

    auto enrollResult = client->EnrollProfileAsync(profile, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrolledVoiceProfile);
    auto model = SpeakerVerificationModel::FromProfile(profile);

    auto recognizer = SpeakerRecognizer::FromConfig(config, audioInput);
    auto result = recognizer->RecognizeOnceAsync(model).get();
    SPXTEST_REQUIRE(result->GetScore() > 0.0);
    SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeaker);
    SPXTEST_REQUIRE(result->ProfileId == profile->GetId());

    auto wrongAudio = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    auto recognizer2 = SpeakerRecognizer::FromConfig(config, wrongAudio);
    auto result2 = recognizer2->RecognizeOnceAsync(model).get();
    SPXTEST_REQUIRE(result2->Reason == ResultReason::NoMatch);

} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::speaker identification",
    "[api][cxx][speaker_id][identification]")
{
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE)));

    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE));
    auto recognizer = SpeakerRecognizer::FromConfig(config, audioInput);

    auto client = VoiceProfileClient::FromConfig(config);
    auto profile1 = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    SPXTEST_REQUIRE(!profile1->GetId().empty());
    // always delete the profile even when there are exceptions in following code.
    auto finish1 = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile1->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile1).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });
    auto enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrolledVoiceProfile);

    auto profile2 = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    SPXTEST_REQUIRE(!profile2->GetId().empty());
    // always delete the profile even when there are exceptions in following code.
    auto finish2 = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile2->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile2).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });
    auto enrollResult2 = client->EnrollProfileAsync(profile2, audioInput).get();
    SPXTEST_REQUIRE(enrollResult2->Reason == ResultReason::EnrolledVoiceProfile);

    auto profiles = std::vector<std::shared_ptr<VoiceProfile>>{ profile1, profile2 };

    auto model = SpeakerIdentificationModel::FromProfiles(profiles);
    auto result = recognizer->RecognizeOnceAsync(model).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeakers);

} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::text dependent verification",
    "[api][cxx][speaker_id][td_good_case]")
{
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SPEAKER_VERIFICATION_ENGLISH)));

    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SPEAKER_VERIFICATION_ENGLISH));

    auto client = VoiceProfileClient::FromConfig(config);
    auto profile1 = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile1->GetId().empty());
    // always delete the profile even when there are exceptions in following code. The lambda is called when exits this test case.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile1->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile1).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });
    // passphrase is my voice is my passport verify me.
    auto enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrollingVoiceProfile);
    SPXTEST_REQUIRE(enrollResult->GetEnrollmentInfo(EnrollmentInfoType::RemainingEnrollmentsCount) == 2);
    SPXTEST_REQUIRE(enrollResult->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsCount) == 1);

    enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrollingVoiceProfile);

    enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrolledVoiceProfile);

    auto model = SpeakerVerificationModel::FromProfile(profile1);
    auto recognizer = SpeakerRecognizer::FromConfig(config, audioInput);
    shared_ptr<SpeakerRecognitionResult> result;
    SPXTEST_CHECK_NOTHROW(result = recognizer->RecognizeOnceAsync(model).get());
    SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeaker);
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::wrong passphrase in text dependent verification",
    "[api][cxx][speaker_id][td_wrong_case]")
{
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SPEAKER_VERIFICATION_ENGLISH)));

    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto verifymeFile = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SPEAKER_VERIFICATION_ENGLISH));

    auto client = VoiceProfileClient::FromConfig(config);
    auto profile1 = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile1->GetId().empty());
    // always delete the profile even when there are exceptions in following code. The lambda is called when exits this test case.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile1->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile1).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });
    // passphrase is my voice is my passport verify me.
    auto enrollResult = client->EnrollProfileAsync(profile1, verifymeFile).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrollingVoiceProfile);

    // feed a audio not containing the passphrase, expecting invalid passphrase in the error detail and BadRequest.
    // this is to test switching audio input between two EnrollProfileAsync.
    auto weatherFile = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    enrollResult = client->EnrollProfileAsync(profile1, weatherFile).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::Canceled);
    auto details = VoiceProfileEnrollmentCancellationDetails::FromResult(enrollResult);
    SPXTEST_REQUIRE(details->ErrorCode == CancellationErrorCode::BadRequest);
    SPXTEST_REQUIRE(PAL::StringUtils::ToLower(details->ErrorDetails).find("invalid passphrase") != string::npos);
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::enroll and delete in parallel",
    "[api][cxx][speaker_id][parallel]")
{
    SPXTEST_REQUIRE(exists(ROOT_RELATIVE_PATH(SPEAKER_VERIFICATION_ENGLISH)));

    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto verifymeFile = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SPEAKER_VERIFICATION_ENGLISH));
    auto weatherFile = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));

    auto client = VoiceProfileClient::FromConfig(config);
    auto profile1 = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile1->GetId().empty());
    // always delete the profile even when there are exceptions in following code. The lambda is called when exits this test case.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile1->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile1).get();
        }});
    auto profile2 = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    // always delete the profile even when there are exceptions in following code. The lambda is called when exits this test case.
    auto finish2 = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile2->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile2).get();
        }});
    // not waiting is an user error, we won't render a result but we should not crash.
    auto enrollFuture1 = client->EnrollProfileAsync(profile1, verifymeFile);
    auto enrollFuture2 = client->EnrollProfileAsync(profile2, weatherFile);
    auto deleteFuture3 = client->DeleteProfileAsync(profile2);
} SPXTEST_CASE_END()

SPXTEST_CASE_BEGIN(
    "Speaker recognition::speaker recognition",
    "[api][cxx][speaker_id][official_endpoint]")
{
    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SPEAKER_VERIFICATION_ENGLISH));
    auto client = VoiceProfileClient::FromConfig(config);
    auto profile1 = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile1->GetId().empty());
    // always delete the profile even when there are exceptions in following code. The lambda is called when exits this test case.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile1->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile1).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });

    auto enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrollingVoiceProfile);

    enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrollingVoiceProfile);

    enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrolledVoiceProfile);
} SPXTEST_CASE_END()

#if 0
SPXTEST_CASE_BEGIN(
    "Speaker recognition::text dependent verification using the microphone",
    "[api][cxx][speaker_id][microphone]")
{
    auto config = GetSpeakerRecognitionProdSubscriptionConfig();
    auto audioInput = AudioConfig::FromDefaultMicrophoneInput();
    //config->SetProperty("SPEECH-MicrophoneTimeoutInSpeakerRecognitionInMilliseconds", "thisis ");
    auto client = VoiceProfileClient::FromConfig(config);
    auto profile1 = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile1->GetId().empty());
    // always delete the profile even when there are exceptions in following code. The lambda is called when exits this test case.
    auto finish = std::shared_ptr<void>(nullptr, [&](void*) {
        if (!profile1->GetId().empty())
        {
            auto deleteResult = client->DeleteProfileAsync(profile1).get();
            SPXTEST_REQUIRE(deleteResult->Reason == ResultReason::DeletedVoiceProfile);
        }
        });
    //REQUIRE_THROWS(client->EnrollProfileAsync(profile1, audioInput).get());

    auto enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrollingVoiceProfile);

    enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrollingVoiceProfile);

    enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrolledVoiceProfile);
} SPXTEST_CASE_END()
#endif

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

TEST_CASE("text independent verification enrollment", "[api][cxx][speaker_id][enrollment_ti_verification]")
{
    // create a profile using a speech config
    SPXTEST_REQUIRE(!DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT].empty());
    auto config = SpeechConfig::FromHost(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());

    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE));
    auto result = client->EnrollProfileAsync(profile, audioInput).get();
    auto json_string = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
    SPXTEST_REQUIRE(!json_string.empty());
    INFO(result->ProfileId);
    SPXTEST_REQUIRE(result->ProfileId == profile->GetId());
    SPXTEST_REQUIRE(result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsCount) == 1);
    SPXTEST_REQUIRE(result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsSpeechLength) >= 239200000);
    SPXTEST_REQUIRE(result->Reason == ResultReason::EnrolledVoiceProfile);
    INFO(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT]);
    INFO(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
}

TEST_CASE("text independent identication enrollment", "[api][cxx][speaker_id][enrollment_ti_identication]")
{
    // create a profile using a speech config
    SPXTEST_REQUIRE(!DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT].empty());
    auto config = SpeechConfig::FromEndpoint(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());

    //auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(SINGLE_UTTERANCE_ENGLISH));
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE));
    auto result = client->EnrollProfileAsync(profile, audioInput).get();
    auto json_string = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
    SPXTEST_REQUIRE(!json_string.empty());
    INFO(result->ProfileId);
    SPXTEST_REQUIRE(result->ProfileId == profile->GetId());
    SPXTEST_REQUIRE(result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsCount) == 1);
    SPXTEST_REQUIRE(result->GetEnrollmentInfo(EnrollmentInfoType::EnrollmentsSpeechLength) >= 239200000);
    auto length= result->GetEnrollmentInfo(EnrollmentInfoType::RemainingEnrollmentsSpeechLength);
    INFO(length);
    SPXTEST_REQUIRE(result->Reason == ResultReason::EnrolledVoiceProfile);
    INFO(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT]);
    INFO(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
}

TEST_CASE("text dependent verification enrollment bad request", "[api][cxx][speaker_id][enrollment_td_verfication_bad_request]")
{
    // create a profile using a speech config
    SPXTEST_REQUIRE(!DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT].empty());
    auto config = SpeechConfig::FromEndpoint(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());

    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE));
    auto result = client->EnrollProfileAsync(profile, audioInput).get();
    INFO(result->ProfileId);
    auto json_string = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult);
    SPXTEST_REQUIRE(json_string.find("error") != std::string::npos);

    SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
    auto details = VoiceProfileEnrollmentCancellationDetails::FromResult(result);
    SPXTEST_REQUIRE(details->ErrorCode == CancellationErrorCode::BadRequest);
    SPXTEST_REQUIRE(details->ErrorDetails.find("Invalid audio length.") != std::string::npos);
    INFO(details->ErrorDetails);
    INFO(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT]);
    INFO(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
}

TEST_CASE("reset voice profile text dependent verification", "[api][cxx][speaker_id][reset_td_ver]")
{
    // create a profile using a speech config
    SPXTEST_REQUIRE(!DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT].empty());
    auto config = SpeechConfig::FromHost(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextDependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());

    auto result = client->ResetProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::ResetVoiceProfile);
    INFO(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT]);
    INFO(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
}

TEST_CASE("reset voice profile text independent verification", "[api][cxx][speaker_id][reset_ti_ver]")
{
    // create a profile using a speech config
    SPXTEST_REQUIRE(!DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT].empty());
    auto config = SpeechConfig::FromEndpoint(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());

    auto result = client->ResetProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::ResetVoiceProfile);
    //note: when reset is successful, the content buffer in the response is empty. So, SpeechServiceResponse_JsonResult is empty.
    SPXTEST_REQUIRE(result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult).empty());
    SPXTEST_REQUIRE(!result->ResultId.empty());
    INFO(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT]);
    INFO(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
}

TEST_CASE("reset voice profile text independent identification", "[api][speaker_id][cxx][reset_ti_iden]")
{
    // create a profile using a speech config
    SPXTEST_REQUIRE(!DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT].empty());
    auto config = SpeechConfig::FromEndpoint(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);

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
    SPXTEST_REQUIRE(d->ErrorDetails.find("profile doesn't exist") != std::string::npos);

    // can't reset or reset without a voice type. it is a runtime error.
    auto fakeProfile = VoiceProfile::FromId("voice_profile_without_voice_type");
    result = client->ResetProfileAsync(fakeProfile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
    auto details = VoiceProfileCancellationDetails::FromResult(result);
    SPXTEST_REQUIRE(details->ErrorDetails.find("error") != std::string::npos);
    SPXTEST_REQUIRE(details->ErrorCode == CancellationErrorCode::RuntimeError);
    INFO(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT]);
    INFO(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
}

TEST_CASE("delete voice profile text independent identification", "[api][cxx][speaker_id][delete_ti_iden]")
{
    // create a profile using a speech config
    SPXTEST_REQUIRE(!DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT].empty());
    auto config = SpeechConfig::FromEndpoint(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
    auto client = VoiceProfileClient::FromConfig(config);

    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());

    auto result = client->DeleteProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::DeletedVoiceProfile);

    result = client->DeleteProfileAsync(profile).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::Canceled);
    //auto raw = result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult, "");
    auto d = VoiceProfileCancellationDetails::FromResult(result);
    SPXTEST_REQUIRE(d->ErrorDetails.find("profile doesn't exist") != std::string::npos);
    SPXTEST_REQUIRE(d->ErrorCode == CancellationErrorCode::ServiceError);
    INFO(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT]);
    INFO(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
}

TEST_CASE("speaker verification", "[api][cxx][speaker_id][verification]")
{
    SPXTEST_REQUIRE(!DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT].empty());
    auto config = SpeechConfig::FromEndpoint(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE));

    auto client = VoiceProfileClient::FromConfig(config);
    auto profile = client->CreateProfileAsync(VoiceProfileType::TextIndependentVerification, "en-us").get();
    SPXTEST_REQUIRE(!profile->GetId().empty());
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
    SPXTEST_REQUIRE(result2->Reason == ResultReason::Canceled);
    auto details = SpeakerRecognitionCancellationDetails::FromResult(result2);

    SPXTEST_REQUIRE(details->Reason == CancellationReason::Error);
    SPXTEST_REQUIRE(PAL::StringUtils::ToLower(details->ErrorDetails).find("reject") != std::string::npos);
    INFO(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT]);
    INFO(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
}

TEST_CASE("speaker identification", "[api][cxx][speaker_id][identification]")
{
    SPXTEST_REQUIRE(!DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT].empty());
    auto config = SpeechConfig::FromEndpoint(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT], SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
    auto audioInput = AudioConfig::FromWavFileInput(ROOT_RELATIVE_PATH(MULTIPLE_UTTERANCE_CHINESE));
    auto recognizer = SpeakerRecognizer::FromConfig(config, audioInput);

    auto client = VoiceProfileClient::FromConfig(config);
    auto profile1 = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    SPXTEST_REQUIRE(!profile1->GetId().empty());
    auto enrollResult = client->EnrollProfileAsync(profile1, audioInput).get();
    SPXTEST_REQUIRE(enrollResult->Reason == ResultReason::EnrolledVoiceProfile);

    auto profile2 = client->CreateProfileAsync(VoiceProfileType::TextIndependentIdentification, "en-us").get();
    SPXTEST_REQUIRE(!profile2->GetId().empty());
    auto enrollResult2 = client->EnrollProfileAsync(profile2, audioInput).get();
    SPXTEST_REQUIRE(enrollResult2->Reason == ResultReason::EnrolledVoiceProfile);

    auto profiles = std::vector<std::shared_ptr<VoiceProfile>>{ profile1, profile2 };

    auto model = SpeakerIdentificationModel::FromProfiles(profiles);
    auto result = recognizer->RecognizeOnceAsync(model).get();
    SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeakers);
    INFO(DefaultSettingsMap[SPEAKER_RECOGNITION_ENDPOINT]);
    INFO(SubscriptionsRegionsMap[UNIFIED_SPEECH_SUBSCRIPTION].Key);
}

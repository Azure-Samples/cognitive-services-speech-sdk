//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//

#pragma once

#include <error_info.h>
#include <http_response.h>
#include <json.h>
#include <memory>
#include <string>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;

namespace {
std::string GetErrorMessageInJson(const std::string& str, int statusCode)
{
  std::string error;
  if (str.empty())
  {
    error = "Error of " + std::to_string(statusCode);
  }
  else
  {
    auto json = nlohmann::json::parse(str);
    if (json.find("error") != json.end())
    {
      error = json["error"]["message"].get<std::string>();
    }
  }
  return error;
}

const std::shared_ptr<ISpxErrorInformation> ErrorFromResponse(
    const std::unique_ptr<HttpResponse>& response)
{
  if (response->IsSuccess())
    return nullptr;
  auto statusCode = (HttpStatusCode)response->GetStatusCode();
  auto errorText
      = GetErrorMessageInJson(response->ReadContentAsString(), response->GetStatusCode());
  return ErrorInfo::FromHttpStatus(statusCode, errorText);
}

ResultReason IdentificationResultReasonFromResponse(const std::unique_ptr<HttpResponse>& response)
{
  if (response == nullptr || !response->IsSuccess())
    return ResultReason::Canceled;

  auto json = nlohmann::json::parse(response->ReadContentAsString());
  auto profileId = json["identifiedProfile"]["profileId"].get<std::string>();

  return !profileId.empty() ? ResultReason::RecognizedSpeakers : ResultReason::Canceled;
}

ResultReason VerificationResultReasonFromResponse(const std::unique_ptr<HttpResponse>& response)
{
  if (response == nullptr || !response->IsSuccess())
    return ResultReason::Canceled;

  auto json = nlohmann::json::parse(response->ReadContentAsString());
  auto recoResult = json["recognitionResult"].get<std::string>();
  auto recoResultLower = PAL::StringUtils::ToLower(recoResult);

  return recoResultLower == "accept"
      ? ResultReason::RecognizedSpeaker
      : recoResultLower == "reject" ? ResultReason::NoMatch : ResultReason::Canceled;
}

ResultReason EnrollmentResultReasonFromResponse(const std::unique_ptr<HttpResponse>& response)
{
  if (response == nullptr || !response->IsSuccess())
    return ResultReason::Canceled;

  auto json = nlohmann::json::parse(response->ReadContentAsString());
  auto recoResult = json["enrollmentStatus"].get<std::string>();
  auto recoResultLower = PAL::StringUtils::ToLower(recoResult);

  return recoResultLower == "enrolling" || recoResultLower == "training"
      ? ResultReason::EnrollingVoiceProfile
      : recoResultLower == "enrolled" ? ResultReason::EnrolledVoiceProfile : ResultReason::Canceled;
}

template <typename T>
bool TryGetJsonValue(
    nlohmann::json& inputJson,
    const std::string& inputKey,
    bool allowNoMatch,
    T* value)
{
  if (inputJson.find(inputKey) != inputJson.end() || !allowNoMatch)
  {
    *value = inputJson[inputKey].get<T>();
    return true;
  }
  return false;
}

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
std::string GetJsonString(nlohmann::json& inputJson, const std::string& inputKey, bool allowNoMatch)
{
  T value;
  return TryGetJsonValue<T>(inputJson, inputKey, allowNoMatch, &value) ? std::to_string(value) : "";
}

std::string GetJsonString(nlohmann::json& inputJson, const std::string& inputKey, bool allowNoMatch)
{
  std::string value;
  return TryGetJsonValue<std::string>(inputJson, inputKey, allowNoMatch, &value) ? value : "";
}

std::string GetJsonStringHNS(
    nlohmann::json& inputJson,
    const std::string& inputKey,
    bool allowNoMatch)
{
  float value;
  if (TryGetJsonValue<float>(inputJson, inputKey, allowNoMatch, &value))
  {
    auto hnsValue = static_cast<uint64_t>(value * 10e7);
    return std::to_string(hnsValue);
  }
  return "";
}

void SetJsonHNSProp(
    nlohmann::json& inputJson,
    const std::string& inputKey,
    const std::shared_ptr<ISpxNamedProperties>& propertyBag,
    const std::string& outputKey)
{
  propertyBag->SetStringValue(
      outputKey.c_str(), GetJsonStringHNS(inputJson, inputKey, false).c_str());
}

void SetJsonHNSProp_NoThrow(
    nlohmann::json& inputJson,
    const std::string& inputKey,
    const std::shared_ptr<ISpxNamedProperties>& propertyBag,
    const std::string& outputKey)
{
  auto input = GetJsonStringHNS(inputJson, inputKey, true);
  if (!input.empty())
  {
    propertyBag->SetStringValue(outputKey.c_str(), input.c_str());
  }
}

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
void SetJsonProp(
    nlohmann::json& inputJson,
    const std::string& inputKey,
    const std::shared_ptr<ISpxNamedProperties>& propertyBag,
    const std::string& outputKey)
{
  propertyBag->SetStringValue(
      outputKey.c_str(), GetJsonString<T>(inputJson, inputKey, false).c_str());
}

void SetJsonProp(
    nlohmann::json& inputJson,
    const std::string& inputKey,
    const std::shared_ptr<ISpxNamedProperties>& propertyBag,
    const std::string& outputKey)
{
  propertyBag->SetStringValue(outputKey.c_str(), GetJsonString(inputJson, inputKey, false).c_str());
}

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
void SetJsonProp_NoThrow(
    nlohmann::json& inputJson,
    const std::string& inputKey,
    const std::shared_ptr<ISpxNamedProperties>& propertyBag,
    const std::string& outputKey)
{
  T value;
  if (TryGetJsonValue<T>(inputJson, inputKey, true, &value))
  {
    propertyBag->SetStringValue(outputKey.c_str(), std::to_string(value).c_str());
  }
}

} // namespace

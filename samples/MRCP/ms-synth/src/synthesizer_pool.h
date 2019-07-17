#pragma once

#include "common.h"
#include "config_manager.h"
#include "speechapi_cxx.h"
#include <Poco/ObjectPool.h>

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::speechlib;

class SpeechSynthesizerFactory
: public Poco::PoolableObjectFactory<SpeechSynthesizer, std::shared_ptr<SpeechSynthesizer>>
{
    public:
    std::shared_ptr<SpeechSynthesizer> createObject()
    {
        std::shared_ptr<SpeechConfig> speechConfig;
        if(ConfigManager::GetBoolValue(Common::SPEECH_SECTION, Common::TTS_USE_LOCAL_CONTAINER))
        {
            static auto localKey =
            ConfigManager::GetStrValue(Common::SPEECH_SECTION, Common::TTS_LOCAL_KEY);
            static auto endpoint =
            ConfigManager::GetStrValue(Common::SPEECH_SECTION, Common::TTS_LOCAL_ENDPOINT);
            speechConfig = SpeechConfig::FromEndpoint(endpoint, localKey);
        }
        else
        {
            static auto subscriptionKey =
            ConfigManager::GetStrValue(Common::SPEECH_SECTION, Common::SPEECH_SDK_KEY);
            static auto region =
            ConfigManager::GetStrValue(Common::SPEECH_SECTION, Common::SPEECH_SDK_REGION);

            speechConfig = SpeechConfig::FromSubscription(subscriptionKey, region); // create from subscription
        }
        
        speechConfig->SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat::Raw16Khz16BitMonoPcm);
        speechConfig->SetSpeechSynthesisVoiceName(
        "Microsoft Server Speech Text to Speech Voice (en-US, JessaNeural)");
        return SpeechSynthesizer::FromConfig(speechConfig, nullptr);
    }

    bool validateObject(std::shared_ptr<SpeechSynthesizer> synthesizer)
    {
        return synthesizer != nullptr;
    }

    void destroyObject(std::shared_ptr<SpeechSynthesizer> synthesizer)
    {
        synthesizer.reset();
    }
};

class SynthesizerPool
{
    public:
    static std::shared_ptr<SpeechSynthesizer> borrowSynthesizer()
    {
        if(pool_ == nullptr)
        {
            const auto factory = SpeechSynthesizerFactory();

            pool_ =
            std::make_unique<Poco::ObjectPool<SpeechSynthesizer, std::shared_ptr<SpeechSynthesizer>, SpeechSynthesizerFactory>>(
            1, ConfigManager::GetIntValue(Common::SPEECH_SECTION, Common::MAX_SYNTHESIZER));
        }
        return pool_->borrowObject();
    }
    static void returnSynthesizer(std::shared_ptr<SpeechSynthesizer>& synthesizer)
    {
        pool_->returnObject(synthesizer);
    }

    private:
    static std::shared_ptr<Poco::ObjectPool<SpeechSynthesizer, std::shared_ptr<SpeechSynthesizer>, SpeechSynthesizerFactory>> pool_;
};

std::shared_ptr<Poco::ObjectPool<SpeechSynthesizer, std::shared_ptr<SpeechSynthesizer>, SpeechSynthesizerFactory>> SynthesizerPool::pool_ =
std::make_unique<Poco::ObjectPool<SpeechSynthesizer, std::shared_ptr<SpeechSynthesizer>, SpeechSynthesizerFactory>>(
1, ConfigManager::GetIntValue(Common::SPEECH_SECTION, Common::MAX_SYNTHESIZER));
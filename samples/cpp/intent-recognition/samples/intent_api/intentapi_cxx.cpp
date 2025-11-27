//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
// intentapi_cxx.cpp: Out-of-line definitions for the standalone intent recognizer API.
//

#include <intentapi_cxx.h>

#include <future>
#include <sstream>
#include <stdexcept>

using namespace Microsoft::SpeechSDK::Standalone::Intent;

IntentRecognitionResult::IntentRecognitionResult(std::string& intentId, std::string& jsonResult, std::string& detailedJsonResult)
	: m_intentId(intentId), IntentId(m_intentId), m_detailedJsonResult(detailedJsonResult)
{
	PopulateIntentFields(jsonResult);
}

const std::map<std::string, std::string>& IntentRecognitionResult::GetEntities() const
{
	return m_entities;
}

const std::string& IntentRecognitionResult::GetDetailedResult() const
{
	return m_detailedJsonResult;
}

IntentRecognitionResult::~IntentRecognitionResult() = default;

void IntentRecognitionResult::PopulateIntentFields(std::string& jsonResult)
{
	auto parser = ajv::json::Parse(jsonResult);
	auto reader = parser.Reader();
	for (auto name = reader.FirstName(); name.IsOk(); name++)
	{
		auto key = name.AsString(false);
		auto item = reader[key.c_str()];
		auto value = item.AsString();
		if (!value.empty())
		{
			key = name.AsString();
			m_entities[key] = value;
		}
	}
}

std::shared_ptr<IntentRecognizer> IntentRecognizer::FromLanguage(const std::string& language)
{
	return std::make_shared<IntentRecognizer>(language);
}

IntentRecognizer::IntentRecognizer(const std::string& language)
	: m_language(language)
{
	m_recognizer = std::make_shared<CSpxIntentRecognizer>(language);
	m_recognizer->Init();
}

IntentRecognizer::~IntentRecognizer()
{
	m_recognizer->Term();
}

std::future<std::shared_ptr<IntentRecognitionResult>> IntentRecognizer::RecognizeOnceAsync(std::string text)
{
	auto keepAlive = this->shared_from_this();
	auto future = std::async(std::launch::async, [keepAlive, this, text]() -> std::shared_ptr<IntentRecognitionResult> {
		std::string intentId;
		std::string jsonResult;
		std::string detailedJsonResult;
		m_recognizer->ProcessText(text, intentId, jsonResult, detailedJsonResult);

		return std::make_shared<IntentRecognitionResult>(intentId, jsonResult, detailedJsonResult);
	});
	return future;
}

void IntentRecognizer::AddIntent(const std::string& simplePhrase)
{
	auto trigger = std::shared_ptr<ISpxTrigger>(new CSpxIntentTrigger());
	trigger->InitPhraseTrigger(simplePhrase);
	AddIntent(trigger, simplePhrase);
}

void IntentRecognizer::AddIntent(const std::string& simplePhrase, const std::string& intentId)
{
	auto trigger = std::shared_ptr<ISpxTrigger>(new CSpxIntentTrigger());
	trigger->InitPhraseTrigger(simplePhrase);
	AddIntent(trigger, intentId);
}

void IntentRecognizer::AddIntent(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentName)
{
	switch (model->GetModelType())
	{
	case LanguageUnderstandingModel::LanguageUnderstandingModelType::PatternMatchingModel:
		m_recognizer->ClearLanguageModels();
		AddPatternMatchingModel(model);
		(void)intentName;
		break;
	default:
		break;
	}
}

void IntentRecognizer::AddIntent(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentName, const std::string& intentId)
{
	switch (model->GetModelType())
	{
	case LanguageUnderstandingModel::LanguageUnderstandingModelType::PatternMatchingModel:
		m_recognizer->ClearLanguageModels();
		AddPatternMatchingModel(model);
		(void)intentName;
		(void)intentId;
		break;
	default:
		break;
	}
}

void IntentRecognizer::AddAllIntents(std::shared_ptr<LanguageUnderstandingModel> model)
{
	switch (model->GetModelType())
	{
	case LanguageUnderstandingModel::LanguageUnderstandingModelType::PatternMatchingModel:
		m_recognizer->ClearLanguageModels();
		AddPatternMatchingModel(model);
		break;
	default:
		break;
	}
}

void IntentRecognizer::AddAllIntents(std::shared_ptr<LanguageUnderstandingModel> model, const std::string& intentId)
{
	switch (model->GetModelType())
	{
	case LanguageUnderstandingModel::LanguageUnderstandingModelType::PatternMatchingModel:
		m_recognizer->ClearLanguageModels();
		AddPatternMatchingModel(model);
		(void)intentId;
		break;
	default:
		break;
	}
}

bool IntentRecognizer::ApplyLanguageModels(const std::vector<std::shared_ptr<LanguageUnderstandingModel>>& collection)
{
	bool result = true;

	m_recognizer->ClearLanguageModels();

	for (auto model : collection)
	{
		switch (model->GetModelType())
		{
		case LanguageUnderstandingModel::LanguageUnderstandingModelType::PatternMatchingModel:
			AddPatternMatchingModel(model);
			break;
		default:
			break;
		}
	}
	return result;
}

void IntentRecognizer::AddIntent(std::shared_ptr<ISpxTrigger> trigger, const std::string& intentId)
{
	m_recognizer->AddIntentTrigger(intentId, trigger, "");
}

void IntentRecognizer::AddPatternMatchingModel(const std::shared_ptr<LanguageUnderstandingModel>& luModel) const
{
	auto model = static_cast<const PatternMatchingModel*>(luModel.get());
	auto modelId = model->GetModelId();

	auto pmModel = std::make_shared<CSpxPatternMatchingModel>(modelId);
	auto pmFactory = std::make_shared<CSpxPatternMatchingFactory>();
	pmFactory->Init(pmModel, m_language);

	typedef PhraseGetterHr(__stdcall* INTENT_PATTERN_MATCHING_MODEL_GET_STR_FROM_INDEX)(void* context, size_t index, const char** str, size_t* size);
	INTENT_PATTERN_MATCHING_MODEL_GET_STR_FROM_INDEX phraseGetter = [](void* context, size_t index, const char** phrase, size_t* phraseLen) -> PhraseGetterHr {
		try
		{
			if (context == nullptr || phrase == nullptr || phraseLen == nullptr)
			{
				return PhraseGetterHr::InvalidArg;
			}

			auto phrases = static_cast<std::vector<std::string>*>(context);
			if (index >= phrases->size())
			{
				return PhraseGetterHr::OutOfRange;
			}

			*phrase = phrases->at(index).c_str();
			*phraseLen = phrases->at(index).length();
			return PhraseGetterHr::NoError;
		}
		catch (...)
		{
			return PhraseGetterHr::UnhandledException;
		}
	};

	for (const auto& entity : model->Entities)
	{
		auto pmEntity = pmFactory->CreateEntity(entity.Type);
		pmEntity->Init(entity.Id, pmModel->GetOrthographyInfo());
		pmEntity->SetMode(entity.Mode);

		auto phraseContext = static_cast<void*>(const_cast<std::vector<std::string>*>(&entity.Phrases));
		for (size_t i = 0; i < entity.Phrases.size(); i++)
		{
			const char* phrase = nullptr;
			size_t phraseLen = 0;
			auto hr = phraseGetter(phraseContext, i, &phrase, &phraseLen);
			if (hr != PhraseGetterHr::NoError)
			{
				std::ostringstream msg;
				msg << "phraseGetter error " << static_cast<int>(hr) << " with modelId " << modelId << " entity.Phrases[" << i << "] " << entity.Phrases[i];
				throw std::runtime_error(msg.str());
			}
			pmEntity->AddPhrase(std::string(phrase, phraseLen));
		}
		pmFactory->AddEntity(pmEntity);
	}

	for (const auto& intent : model->Intents)
	{
		auto pmIntent = pmFactory->CreateIntent();
		auto priority = 0;
		pmIntent->Init(intent.Id, priority, pmModel->GetOrthographyInfo().Name);

		auto phraseContext = static_cast<void*>(const_cast<std::vector<std::string>*>(&intent.Phrases));
		for (size_t i = 0; i < intent.Phrases.size(); i++)
		{
			const char* phrase = nullptr;
			size_t phraseLen = 0;
			auto hr = phraseGetter(phraseContext, i, &phrase, &phraseLen);
			if (hr != PhraseGetterHr::NoError)
			{
				std::ostringstream msg;
				msg << "phraseGetter error " << static_cast<int>(hr) << " with modelId " << modelId << " intent.Phrases[" << i << "] " << intent.Phrases[i];
				throw std::runtime_error(msg.str());
			}
			pmIntent->AddPhrase(std::string(phrase, phraseLen));
		}
		pmFactory->AddIntent(pmIntent);
	}

	auto pmTrigger = std::shared_ptr<ISpxTrigger>(new CSpxIntentTrigger());
	pmTrigger->InitPatternMatchingModelTrigger(pmModel);

	m_recognizer->AddIntentTrigger("", pmTrigger, modelId);
}

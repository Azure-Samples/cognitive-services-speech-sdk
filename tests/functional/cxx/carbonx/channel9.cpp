//      __  __ _                           __ _      ____                  _ _   _             ____                  _                     //
//     |  \/  (_) ___ _ __ ___  ___  ___  / _| |_   / ___|___   __ _ _ __ (_) |_(_)_   _____  / ___|  ___ _ ____   _(_) ___ ___  ___       //
//     | |\/| | |/ __| '__/ _ \/ __|/ _ \| |_| __| | |   / _ \ / _` | '_ \| | __| \ \ / / _ \ \___ \ / _ \ '__\ \ / / |/ __/ _ \/ __|      //
//     | |  | | | (__| | | (_) \__ \ (_) |  _| |_  | |__| (_) | (_| | | | | | |_| |\ V /  __/  ___) |  __/ |   \ V /| | (_|  __/\__ \      //
//     |_|  |_|_|\___|_|  \___/|___/\___/|_|  \__|  \____\___/ \__, |_| |_|_|\__|_| \_/ \___| |____/ \___|_|    \_/ |_|\___\___||___/      //
//                                                             |___/                                                                       //

//       ____ _                            _      ___      ____                             //
//      / ___| |__   __ _ _ __  _ __   ___| |    / _ \    |  _ \  ___ _ __ ___   ___        //
//     | |   | '_ \ / _` | '_ \| '_ \ / _ \ |   | (_) |   | | | |/ _ \ '_ ` _ \ / _ \       //
//     | |___| | | | (_| | | | | | | |  __/ |    \__, |   | |_| |  __/ | | | | | (_) |      //
//      \____|_| |_|\__,_|_| |_|_| |_|\___|_|      /_/    |____/ \___|_| |_| |_|\___/       //
//                                                                                          //


#include <stdafx.h>
#include <stdio.h>
#include <iostream>
using namespace::std;

int CarbonTestConsole::channel9()
{
    printf("Channel 9 - Microsoft Cognitive Services Speech SDK - DEMO !!\n");

    bool showMenu = true;
    while (showMenu)
    {
        printf("\n");

        printf("1 - do_speech()\n");
        printf("2 - do_speech_intermediate()\n");
        printf("3 - do_speech_continuous()\n");
        printf("4 - do_intent()\n");
        printf("5 - do_intent_continuous()\n");
        printf("6 - do_kws_speech()\n");
        printf("7 - do_kws_intent()\n");
        printf("8 - do_translation()\n");
        printf("x - exit\n");

        printf("\nchoice: ");

        auto ch = getchar();
        while (ch != '\n' && getchar() != '\n');

        printf("\n");
        switch (ch)
        {
        case '1': ch9_do_speech(); break;
        case '2': ch9_do_speech_intermediate(); break;
        case '3': ch9_do_speech_continuous(); break;

        case '4': ch9_do_intent(); break;
        case '5': ch9_do_intent_continuous(); break;

        case '6': ch9_do_kws_speech(); break;
        case '7': ch9_do_kws_intent(); break;

        case '8': ch9_do_translation(); break;

        default: showMenu = false; break;
        }
    }

    return 0;
}


//      ____  ____  _  __    ____            _                          //
//     / ___||  _ \| |/ /   |  _ \ __ _  ___| | ____ _  __ _  ___       //
//     \___ \| | | | ' /    | |_) / _` |/ __| |/ / _` |/ _` |/ _ \      //
//      ___) | |_| | . \    |  __/ (_| | (__|   < (_| | (_| |  __/      //
//     |____/|____/|_|\_\   |_|   \__,_|\___|_|\_\__,_|\__, |\___|      //
//                                                     |___/            //

#include "speechapi_cxx.h"


// The Cognitive Services Speech SDK uses strongly typed objects, in scenario specific namespaces...
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Intent;
using namespace Microsoft::CognitiveServices::Speech::Translation;


//      ____        _                   _       _   _                   //
//     / ___| _   _| |__  ___  ___ _ __(_)_ __ | |_(_) ___  _ __        //
//     \___ \| | | | '_ \/ __|/ __| '__| | '_ \| __| |/ _ \| '_ \       //
//      ___) | |_| | |_) \__ \ (__| |  | | |_) | |_| | (_) | | | |      //
//     |____/ \__,_|_.__/|___/\___|_|  |_| .__/ \__|_|\___/|_| |_|      //
//                                       |_|                            //

constexpr auto speechRegion = "westus";
constexpr auto luisRegion = "westus";
constexpr auto translationRegion = "westus";

constexpr auto bingSpeechEndpoint = R"(wss://speech.platform.bing.com/speech/recognition/interactive/cognitiveservices/v1?format=simple&language=en-us)";
constexpr auto luisPpeSpeechEndpoint = R"(wss://speech.platform.bing.com/ppe/speech/uswest2/recognition/interactive/cognitiveservices/v1?format=simple&setflight=cognitiveservicesintent&&language=en-us)";
constexpr auto luisSpeechEndpoint = R"(wss://speech.platform.bing.com/speech/uswest2/recognition/interactive/cognitiveservices/v1?format=simple&language=en-us)";
constexpr auto luisEndpoint = R"(https://REGION.api.cognitive.microsoft.com/luis/v2.0/apps/APP-ID?subscription-key=KEY&verbose=true&timezoneOffset=0&q=)";


void unused()
{
    UNUSED(speechRegion);

    UNUSED(bingSpeechEndpoint);

    UNUSED(luisPpeSpeechEndpoint);

    UNUSED(luisSpeechEndpoint);

    UNUSED(luisEndpoint);
    UNUSED(luisRegion);

    UNUSED(translationRegion);
}

//      ____                       _         ____                            _ _   _                   //
//     / ___| _ __   ___  ___  ___| |__     |  _ \ ___  ___ ___   __ _ _ __ (_) |_(_) ___  _ __        //
//     \___ \| '_ \ / _ \/ _ \/ __| '_ \    | |_) / _ \/ __/ _ \ / _` | '_ \| | __| |/ _ \| '_ \       //
//      ___) | |_) |  __/  __/ (__| | | |   |  _ <  __/ (_| (_) | (_| | | | | | |_| | (_) | | | |      //
//     |____/| .__/ \___|\___|\___|_| |_|   |_| \_\___|\___\___/ \__, |_| |_|_|\__|_|\___/|_| |_|      //
//           |_|                                                 |___/                                 //

void CarbonTestConsole::ch9_do_speech()
{
    auto sc = SpeechConfig::FromSubscription(m_subscriptionKey, speechRegion);
    auto recognizer = SpeechRecognizer::FromConfig(sc, nullptr);

    printf("Say something...\n");
    auto result = recognizer->RecognizeAsync().get();

    printf("FINAL RESULT: '%s'\n", result->Text.c_str());
}

void CarbonTestConsole::ch9_do_speech_intermediate()
{
    auto sc = SpeechConfig::FromSubscription(m_subscriptionKey, speechRegion);
    auto recognizer = SpeechRecognizer::FromConfig(sc, nullptr);

    recognizer->IntermediateResult += [](const SpeechRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %s ...\n", e.Result->Text.c_str());
    };

    printf("Listening...\n");
    auto result = recognizer->RecognizeAsync().get();

    printf("FINAL RESULT: '%s'\n", result->Text.c_str());
}

void CarbonTestConsole::ch9_do_speech_continuous()
{
    auto sc = SpeechConfig::FromSubscription(m_subscriptionKey, speechRegion);
    auto recognizer = SpeechRecognizer::FromConfig(sc, nullptr);

    recognizer->IntermediateResult += [](const SpeechRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %s ...\n", e.Result->Text.c_str());
    };

    recognizer->FinalResult += [](const SpeechRecognitionEventArgs& e) {
        printf("FINAL RESULT: '%s'\n\n", e.Result->Text.c_str());
        printf("Listening... (press ENTER to exit) \n\n");
    };

    recognizer->StartContinuousRecognitionAsync();

    printf("Listening... (press ENTER to exit) \n\n");
    while (getchar() != '\n');

    //recognizer->StopContinuousRecognitionAsync();
}

//      ___       _             _       ____                            _ _   _                   //
//     |_ _|_ __ | |_ ___ _ __ | |_    |  _ \ ___  ___ ___   __ _ _ __ (_) |_(_) ___  _ __        //
//      | || '_ \| __/ _ \ '_ \| __|   | |_) / _ \/ __/ _ \ / _` | '_ \| | __| |/ _ \| '_ \       //
//      | || | | | ||  __/ | | | |_    |  _ <  __/ (_| (_) | (_| | | | | | |_| | (_) | | | |      //
//     |___|_| |_|\__\___|_| |_|\__|   |_| \_\___|\___\___/ \__, |_| |_|_|\__|_|\___/|_| |_|      //
//                                                          |___/                                 //

void CarbonTestConsole::ch9_do_intent()
{
    auto sc = SpeechConfig::FromSubscription(m_subscriptionKey, luisRegion);
    auto recognizer = IntentRecognizer::FromConfig(sc, nullptr);

    recognizer->IntermediateResult += [](const IntentRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %s ...\n", e.Result->Text.c_str());
    };

    auto model = LanguageUnderstandingModel::FromSubscription(m_subscriptionKey, m_intentAppId, luisRegion);
    // auto model = LanguageUnderstandingModel::FromEndpoint(luisEndpoint);
    // auto model = LanguageUnderstandingModel::FromAppId(luisAppId);
    recognizer->AddIntent(model, "TV.ChangeChannel");
    recognizer->AddIntent(model, "TV.WatchTV");
    recognizer->AddIntent(model, "TV.ShowGuide");
    //recognizer->AddAllIntents(model, L"id-representing-all-intents");

    printf("Listening...\n");
    auto result = recognizer->RecognizeAsync().get();

    printf("FINAL RESULT: '%s'\n", result->Text.c_str());
    printf("   INTENT ID: '%s'\n", result->IntentId.c_str());
    printf("   LUIS JSON: '%s'\n\n", result->Properties.GetProperty(SpeechPropertyId::SpeechServiceResponse_JsonResult).c_str());

    //recognizer->StopContinuousRecognitionAsync();
}

void CarbonTestConsole::ch9_do_intent_continuous()
{
    auto sc = SpeechConfig::FromSubscription(m_subscriptionKey, luisRegion);
    auto recognizer = IntentRecognizer::FromConfig(sc, nullptr);

    recognizer->IntermediateResult += [](const IntentRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %s ...\n", e.Result->Text.c_str());
    };

    recognizer->FinalResult += [](const IntentRecognitionEventArgs& e) {
        printf("FINAL RESULT: '%s'\n", e.Result->Text.c_str());
        printf("   INTENT ID: '%s'\n", e.Result->IntentId.c_str());
        printf("   LUIS JSON: '%s'\n\n", e.Result->Properties.GetProperty(SpeechPropertyId::SpeechServiceResponse_JsonResult).c_str());
        printf("Listening... (press ENTER to exit) \n\n");
    };

    auto model = LanguageUnderstandingModel::FromSubscription(m_subscriptionKey, m_intentAppId, luisRegion);
    // auto model = LanguageUnderstandingModel::FromEndpoint(luisEndpoint);
    // auto model = LanguageUnderstandingModel::FromAppId(luisAppId);
    recognizer->AddIntent(model, "TV.ChangeChannel");
    recognizer->AddIntent(model, "TV.WatchTV");
    recognizer->AddIntent(model, "TV.ShowGuide");

    recognizer->StartContinuousRecognitionAsync();

    printf("Listening... (press ENTER to exit) \n\n");
    while (getchar() != '\n');

    //recognizer->StopContinuousRecognitionAsync();
}

//      _  __                                _     ____                            _ _   _                   //
//     | |/ /___ _   ___      _____  _ __ __| |   |  _ \ ___  ___ ___   __ _ _ __ (_) |_(_) ___  _ __        //
//     | ' // _ \ | | \ \ /\ / / _ \| '__/ _` |   | |_) / _ \/ __/ _ \ / _` | '_ \| | __| |/ _ \| '_ \       //
//     | . \  __/ |_| |\ V  V / (_) | | | (_| |   |  _ <  __/ (_| (_) | (_| | | | | | |_| | (_) | | | |      //
//     |_|\_\___|\__, | \_/\_/ \___/|_|  \__,_|   |_| \_\___|\___\___/ \__, |_| |_|_|\__|_|\___/|_| |_|      //
//               |___/                                                 |___/                                 //

void CarbonTestConsole::ch9_do_kws_speech()
{
    auto sc = SpeechConfig::FromSubscription(m_subscriptionKey, speechRegion);
    auto recognizer = SpeechRecognizer::FromConfig(sc, nullptr);

    recognizer->IntermediateResult += [](const SpeechRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %s ...\n", e.Result->Text.c_str());
    };

    recognizer->FinalResult += [](const SpeechRecognitionEventArgs& e) {
        printf("FINAL RESULT: '%s'\n", e.Result->Text.c_str());
        printf("KEYWORD SPOTTING: Say 'Hey Cortana' followed by whatever you want ...  (press ENTER to exit) \n\n");
    };

    auto keywordModel = KeywordRecognitionModel::FromFile("kws.table");
    recognizer->StartKeywordRecognitionAsync(keywordModel);

    printf("KEYWORD SPOTTING: Say 'Hey Cortana' followed by whatever you want ...  (press ENTER to exit) \n\n");
    while (getchar() != '\n');

    // recognizer->StopKeywordRecognitionAsync().get();
}

void CarbonTestConsole::ch9_do_kws_intent()
{
    auto sc = SpeechConfig::FromSubscription(m_subscriptionKey, luisRegion);
    auto recognizer = IntentRecognizer::FromConfig(sc, nullptr);

    recognizer->IntermediateResult += [](const IntentRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %s ...\n", e.Result->Text.c_str());
    };

    recognizer->FinalResult += [](const IntentRecognitionEventArgs& e) {
        printf("FINAL RESULT: '%s'\n", e.Result->Text.c_str());
        printf("   INTENT ID: '%s'\n", e.Result->IntentId.c_str());
        printf("   LUIS JSON: '%s'\n\n", e.Result->Properties.GetProperty(SpeechPropertyId::SpeechServiceResponse_JsonResult).c_str());
        printf("KEYWORD SPOTTING: Say 'Hey Cortana' followed by whatever you want ...  (press ENTER to exit) \n\n");
    };

    auto model = LanguageUnderstandingModel::FromSubscription(m_subscriptionKey, m_intentAppId, luisRegion);
    // auto model = LanguageUnderstandingModel::FromEndpoint(luisEndpoint);
    // auto model = LanguageUnderstandingModel::FromAppId(luisAppId);
    recognizer->AddIntent(model, "TV.ChangeChannel");
    recognizer->AddIntent(model, "TV.WatchTV");
    recognizer->AddIntent(model, "TV.ShowGuide");

    auto keywordModel = KeywordRecognitionModel::FromFile("kws.table");
    recognizer->StartKeywordRecognitionAsync(keywordModel);

    printf("KEYWORD SPOTTING: Say 'Hey Cortana' followed by whatever you want ...  (press ENTER to exit) \n\n");
    while (getchar() != '\n');

    // recognizer->StopKeywordRecognitionAsync().get();
}

//      __  __            _     _                _____                    _       _   _                   //
//     |  \/  | __ _  ___| |__ (_)_ __   ___    |_   _| __ __ _ _ __  ___| | __ _| |_(_) ___  _ __        //
//     | |\/| |/ _` |/ __| '_ \| | '_ \ / _ \     | || '__/ _` | '_ \/ __| |/ _` | __| |/ _ \| '_ \       //
//     | |  | | (_| | (__| | | | | | | |  __/     | || | | (_| | | | \__ \ | (_| | |_| | (_) | | | |      //
//     |_|  |_|\__,_|\___|_| |_|_|_| |_|\___|     |_||_|  \__,_|_| |_|___/_|\__,_|\__|_|\___/|_| |_|      //
//                                                                                                        //

void CarbonTestConsole::ch9_do_translation()
{
    auto sc = SpeechTranslationConfig::FromSubscription(m_subscriptionKey, translationRegion);
    sc->SetSpeechRecognitionLanguage("en-US");
    sc->AddTargetLanguage("de-DE");
    sc->AddTargetLanguage("fr-FR");
    sc->AddTargetLanguage("es-ES");
    auto recognizer = TranslationRecognizer::FromConfig(sc, nullptr);

    recognizer->IntermediateResult += [](const TranslationTextResultEventArgs& e) {
        printf("INTERMEDIATE: %s ...\n", e.Result->Text.c_str());
    };

    recognizer->FinalResult += [](const TranslationTextResultEventArgs& e) {
        printf("FINAL RESULT: '%s'\n", e.Result->Text.c_str());
        for (auto translation : e.Result->Translations) {
            printf(" TRANSLATION: '%s' => '%s'\n", translation.first.c_str(), translation.second.c_str());
        }
        printf("Listening... (press ENTER to exit) \n\n");
    };

    recognizer->StartContinuousRecognitionAsync();

    printf("Listening... (press ENTER to exit) \n\n");
    while (getchar() != '\n');

    //recognizer->StopContinuousRecognitionAsync();
}


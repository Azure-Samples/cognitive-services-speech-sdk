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

int channel9()
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

constexpr auto speechSubscription = L"e8c934dfd8dd43cda89070ffc8fb5eee";
constexpr auto speechRegion = L"westus2";

constexpr auto luisSubscription = L"ee52996d8f814c0aa77f7a415f81bd4c";
constexpr auto luisAppId = L"6ad2c77d180b45a288aa8c442538c090";
constexpr auto luisRegion = L"westus2";

constexpr auto translationSubscription = L"a8ddd80e37dc4c549d9bafd91dadc29a";
constexpr auto translationDeploymentId = L"d4501bd5-a593-45bf-82a6-36ffc59d80a5";
constexpr auto translationRegion = L"westus2";


//      ____                       _         ____                            _ _   _                   //
//     / ___| _ __   ___  ___  ___| |__     |  _ \ ___  ___ ___   __ _ _ __ (_) |_(_) ___  _ __        //
//     \___ \| '_ \ / _ \/ _ \/ __| '_ \    | |_) / _ \/ __/ _ \ / _` | '_ \| | __| |/ _ \| '_ \       //
//      ___) | |_) |  __/  __/ (__| | | |   |  _ <  __/ (_| (_) | (_| | | | | | |_| | (_) | | | |      //
//     |____/| .__/ \___|\___|\___|_| |_|   |_| \_\___|\___\___/ \__, |_| |_|_|\__|_|\___/|_| |_|      //
//           |_|                                                 |___/                                 //

void ch9_do_speech()
{
    auto factory = SpeechFactory::FromSubscription(speechSubscription, speechRegion);
    auto recognizer = factory->CreateSpeechRecognizer();

    printf("Say something...\n");
    auto result = recognizer->RecognizeAsync().get();

    printf("FINAL RESULT: '%ls'\n", result->Text.c_str());
}

void ch9_do_speech_intermediate()
{
    auto factory = SpeechFactory::FromSubscription(speechSubscription, speechRegion);
    auto recognizer = factory->CreateSpeechRecognizer();

    recognizer->IntermediateResult += [](const SpeechRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %ls ...\n", e.Result.Text.c_str());
    };

    printf("Listening...\n");
    auto result = recognizer->RecognizeAsync().get();

    printf("FINAL RESULT: '%ls'\n", result->Text.c_str());
}

void ch9_do_speech_continuous()
{
    auto factory = SpeechFactory::FromSubscription(speechSubscription, speechRegion);
    auto recognizer = factory->CreateSpeechRecognizer();

    recognizer->IntermediateResult += [](const SpeechRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %ls ...\n", e.Result.Text.c_str());
    };

    recognizer->FinalResult += [](const SpeechRecognitionEventArgs& e) {
        printf("FINAL RESULT: '%ls'\n\n", e.Result.Text.c_str());
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

void ch9_do_intent()
{
    auto factory = SpeechFactory::FromSubscription(luisSubscription, luisRegion);
    // factory->Parameters.SetBool(L"CARBON-INTERNAL-USP-NoDGI", true);
    // factory->Parameters.SetBool(L"CARBON-INTERNAL-USP-NoIntentJson", true);

    auto recognizer = factory->CreateIntentRecognizer();

    recognizer->IntermediateResult += [](const IntentRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %ls ...\n", e.Result.Text.c_str());
    };

    //auto model = LanguageUnderstandingModel::FromSubscription(luisSubscription, luisAppId, luisRegion);
    auto model = LanguageUnderstandingModel::FromAppId(luisAppId);
    recognizer->AddIntent(L"1", model, L"TV.ChangeChannel");
    recognizer->AddIntent(L"2", model, L"TV.WatchTV");
    recognizer->AddIntent(L"3", model, L"TV.ShowGuide");
    //recognizer->AddIntent(L"all intents", IntentTrigger::From(model, L""));

    printf("Listening...\n");
    auto result = recognizer->RecognizeAsync().get();

    printf("FINAL RESULT: '%ls'\n", result->Text.c_str());
    printf("   INTENT ID: '%ls'\n", result->IntentId.c_str());
    printf("   LUIS JSON: '%ls'\n\n", result->Properties[ResultProperty::LanguageUnderstandingJson].GetString().c_str());

    //recognizer->StopContinuousRecognitionAsync();
}

void ch9_do_intent_continuous()
{
    auto factory = SpeechFactory::FromSubscription(luisSubscription, luisRegion);
    auto recognizer = factory->CreateIntentRecognizer();

    recognizer->IntermediateResult += [](const IntentRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %ls ...\n", e.Result.Text.c_str());
    };

    recognizer->FinalResult += [](const IntentRecognitionEventArgs& e) {
        printf("FINAL RESULT: '%ls'\n", e.Result.Text.c_str());
        printf("   INTENT ID: '%ls'\n", e.Result.IntentId.c_str());
        printf("   LUIS JSON: '%ls'\n\n", e.Result.Properties[ResultProperty::LanguageUnderstandingJson].GetString().c_str());
        printf("Listening... (press ENTER to exit) \n\n");
    };

    auto model = LanguageUnderstandingModel::FromAppId(luisAppId);
    recognizer->AddIntent(L"1", model, L"TV.ChangeChannel");
    recognizer->AddIntent(L"2", model, L"TV.WatchTV");
    recognizer->AddIntent(L"3", model, L"TV.ShowGuide");

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

void ch9_do_kws_speech()
{
    auto factory = SpeechFactory::FromSubscription(speechSubscription, speechRegion);
    auto recognizer = factory->CreateSpeechRecognizer();

    recognizer->IntermediateResult += [](const SpeechRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %ls ...\n", e.Result.Text.c_str());
    };

    recognizer->FinalResult += [](const SpeechRecognitionEventArgs& e) {
        printf("FINAL RESULT: '%ls'\n", e.Result.Text.c_str());
        printf("KEYWORD SPOTTING: Say 'Hey Cortana' followed by whatever you want ...  (press ENTER to exit) \n\n");
    };

    auto keywordModel = KeywordRecognitionModel::FromFile(L"heycortana_en-US.table");
    recognizer->StartKeywordRecognitionAsync(keywordModel);

    printf("KEYWORD SPOTTING: Say 'Hey Cortana' followed by whatever you want ...  (press ENTER to exit) \n\n");
    while (getchar() != '\n');

    // recognizer->StopKeywordRecognitionAsync().get();
}

void ch9_do_kws_intent()
{
    auto factory = SpeechFactory::FromSubscription(luisSubscription, luisRegion);
    auto recognizer = factory->CreateIntentRecognizer();

    recognizer->IntermediateResult += [](const IntentRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %ls ...\n", e.Result.Text.c_str());
    };

    recognizer->FinalResult += [](const IntentRecognitionEventArgs& e) {
        printf("FINAL RESULT: '%ls'\n", e.Result.Text.c_str());
        printf("   INTENT ID: '%ls'\n", e.Result.IntentId.c_str());
        printf("   LUIS JSON: '%ls'\n\n", e.Result.Properties[ResultProperty::LanguageUnderstandingJson].GetString().c_str());
        printf("KEYWORD SPOTTING: Say 'Hey Cortana' followed by whatever you want ...  (press ENTER to exit) \n\n");
    };

    auto model = LanguageUnderstandingModel::FromAppId(luisAppId);
    recognizer->AddIntent(L"1", model, L"TV.ChangeChannel");
    recognizer->AddIntent(L"2", model, L"TV.WatchTV");
    recognizer->AddIntent(L"3", model, L"TV.ShowGuide");

    auto keywordModel = KeywordRecognitionModel::FromFile(L"heycortana_en-US.table");
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

void ch9_do_translation()
{
    auto factory = SpeechFactory::FromSubscription(translationSubscription, translationRegion);
    auto recognizer = factory->CreateTranslationRecognizer(L"en-US", { L"de-DE", L"fr-FR", L"es-ES" });
    recognizer->Parameters[RecognizerParameter::DeploymentId] = translationDeploymentId;

    recognizer->IntermediateResult += [](const TranslationTextResultEventArgs& e) {
        printf("INTERMEDIATE: %ls ...\n", e.Result.Text.c_str());
    };

    recognizer->FinalResult += [](const TranslationTextResultEventArgs& e) {
        printf("FINAL RESULT: '%ls'\n", e.Result.Text.c_str());
        for (auto translation : e.Result.Translations) {
            printf(" TRANSLATION: '%ls' => '%ls'\n", translation.first.c_str(), translation.second.c_str());
        }
        printf("Listening... (press ENTER to exit) \n\n");
    };

    recognizer->StartContinuousRecognitionAsync();

    printf("Listening... (press ENTER to exit) \n\n");
    while (getchar() != '\n');

    //recognizer->StopContinuousRecognitionAsync();
}


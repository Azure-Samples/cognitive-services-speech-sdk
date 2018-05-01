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

void do_speech();
void do_speech_intermediate();
void do_speech_continuous();

void do_intent();
void do_intent_kws();
void do_translation();

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
        printf("5 - do_intent_kws()\n");
        printf("6 - do_translation()\n");
        printf("x - exit\n");

        printf("\nchoice: ");

        auto ch = getchar();
        while (ch != '\n' && getchar() != '\n');

        printf("\n");
        switch (ch)
        {
        case '1': do_speech(); break;
        case '2': do_speech_intermediate(); break;
        case '3': do_speech_continuous(); break;
        case '4': do_intent(); break;
        case '5': do_intent_kws(); break;
        case '6': do_translation(); break;
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

constexpr auto speechAndLuisRegion = L"westus2";
constexpr auto speechSubscription = L"1f30c291f2474d39acfdf1d3bdf847c3";
// constexpr auto luisSubscription = L"ee52996d8f814c0aa77f7a415f81bd4c";
// constexpr auto luisRegion = L"westus2";
// constexpr auto luisAppId = L"6ad2c77d180b45a288aa8c442538c090";

constexpr auto translationSubscription = L"a8ddd80e37dc4c549d9bafd91dadc29a";
constexpr auto translationDeploymentId = L"d4501bd5-a593-45bf-82a6-36ffc59d80a5";


//      ____                       _         ____                            _ _   _                   //
//     / ___| _ __   ___  ___  ___| |__     |  _ \ ___  ___ ___   __ _ _ __ (_) |_(_) ___  _ __        //
//     \___ \| '_ \ / _ \/ _ \/ __| '_ \    | |_) / _ \/ __/ _ \ / _` | '_ \| | __| |/ _ \| '_ \       //
//      ___) | |_) |  __/  __/ (__| | | |   |  _ <  __/ (_| (_) | (_| | | | | | |_| | (_) | | | |      //
//     |____/| .__/ \___|\___|\___|_| |_|   |_| \_\___|\___\___/ \__, |_| |_|_|\__|_|\___/|_| |_|      //
//           |_|                                                 |___/                                 //

void do_speech()
{
    auto factory = SpeechFactory::FromSubscription(speechSubscription, speechAndLuisRegion);
    auto recognizer = factory->CreateSpeechRecognizer();

    printf("Say something...\n");
    auto result = recognizer->RecognizeAsync().get();

    printf("FINAL RESULT: '%ls'\n", result->Text.c_str());
}

void do_speech_intermediate()
{
    auto factory = SpeechFactory::FromSubscription(speechSubscription, speechAndLuisRegion);
    auto recognizer = factory->CreateSpeechRecognizer();

    recognizer->IntermediateResult += [](const SpeechRecognitionEventArgs& e) {
        printf("INTERMEDIATE: %ls ...\n", e.Result.Text.c_str());
    };

    printf("Listening...\n");
    auto result = recognizer->RecognizeAsync().get();

    printf("FINAL RESULT: '%ls'\n", result->Text.c_str());
}

void do_speech_continuous()
{
    auto factory = SpeechFactory::FromSubscription(speechSubscription, speechAndLuisRegion);
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

void do_intent()
{
    auto factory = SpeechFactory::FromSubscription(speechSubscription, speechAndLuisRegion);
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

    // auto model = LanguageUnderstandingModel::FromSubscription(luisSubscription, luisAppId, luisRegion);
    auto model = LanguageUnderstandingModel::FromEndpoint(LR"(https://westus2.api.cognitive.microsoft.com/luis/v2.0/apps/6ad2c77d180b45a288aa8c442538c090?subscription-key=ee52996d8f814c0aa77f7a415f81bd4c)");

    //recognizer->AddIntent(L"all intents", IntentTrigger::From(model, L""));
    recognizer->AddIntent(L"1", IntentTrigger::From(model, L"TV.ChangeChannel"));
    recognizer->AddIntent(L"2", IntentTrigger::From(model, L"TV.WatchTV"));
    recognizer->AddIntent(L"3", IntentTrigger::From(model, L"TV.ShowGuide"));

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

void do_intent_kws()
{
    auto factory = SpeechFactory::FromSubscription(speechSubscription, speechAndLuisRegion);
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

    // auto model = LanguageUnderstandingModel::FromSubscription(luisSubscription, luisAppId, luisRegion);
    auto model = LanguageUnderstandingModel::FromEndpoint(LR"(https://westus2.api.cognitive.microsoft.com/luis/v2.0/apps/6ad2c77d180b45a288aa8c442538c090?subscription-key=ee52996d8f814c0aa77f7a415f81bd4c)");

    //recognizer->AddIntent(L"all intents", IntentTrigger::From(model, L""));
    recognizer->AddIntent(L"1", IntentTrigger::From(model, L"TV.ChangeChannel"));
    recognizer->AddIntent(L"2", IntentTrigger::From(model, L"TV.WatchTV"));
    recognizer->AddIntent(L"3", IntentTrigger::From(model, L"TV.ShowGuide"));

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

void do_translation()
{
    auto factory = SpeechFactory::FromSubscription(translationSubscription, speechAndLuisRegion);
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


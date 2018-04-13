//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_translation_recognizer.h: Public API declarations for translation recognizer in C++.
//

#pragma once
#include <exception>
#include <future>
#include <memory>
#include <string>
#include <unordered_map>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_recognition_async_recognizer.h>
#include <speechapi_cxx_translation_result.h>
#include <speechapi_cxx_translation_eventargs.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Translation {

// Defines scopes for requesting language resources.
// Clients use the scope define which sets of languages they are interested in.
//    TRANSLATION_LANGUAGE_RESOURCE_SCOPE_SPEECH: retrieves the set of languages available to transcribe speech into text.
//    TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TEXT: retrieves the set of languages available to translate transcribed text.
//    TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TTS: retrieves the set of languages and voices available to synthesize translated text back into speech.
// Clients can retrieve multiple sets simultaneously by setting one or more values via bit-or.
typedef unsigned int LanguageResourceScope;
#define TRANSLATION_LANGUAGE_RESOURCE_SCOPE_SPEECH 0x01
#define TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TEXT 0x02
#define TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TTS 0x04

/*
* Defines the format of resource value for the scope TRANSLATION_LANGUAGE_RESOURCE_SCOPE_SPEECH.
*    name: Display name of the language.
*    language: Language tag of the associated written language.
*/
typedef struct _SpeechScopeResourceValue
{
    ::std::wstring name;
    ::std::wstring language;
} SpeechScopeResourceValue;

/*
* Defines the format of resource value for the scope TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TEXT.
*     name: Display name of the language.
**    dir: Directionality which is "rtl" for right-to-left languages, or "ltr" for left-to-right languages.
*/
typedef struct _TextScopeResourceValue
{
    ::std::wstring name;
    ::std::wstring dir;
} TextScopeResourceValue;

/*
* Defines the format of resource value for the scope TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TTS
*   displayName: Display name for the voice.
*   gender : Gender of the voice(male or female).
*   locale : Language tag of the voice with primary language subtag and region subtag.
*   language : Language tag of the associated written language.
*   languageName : Display name of the language.
*   regionName : Display name of the region for this language.
*/
typedef struct _SynthesisScopeResourceValue
{
    ::std::wstring displayName;
    ::std::wstring gender;
    ::std::wstring locale;
    ::std::wstring languageName;
    ::std::wstring regionName;
    ::std::wstring language;
} SynthesisScopeResourceValue;

/*
* Defines language resources that are supported by the translation service.
* See https://docs.microsofttranslator.com/speech-translate.html for details.
*/
typedef struct _TranslationLanguageResource
{
    // Represents language resources associated with the speech-to-text property. It is a dictionary of (key, value) pairs.
    // Each key identifies a language supported for speech-to- text. The key is the identifier that client passes to the API.
    // The value associated with the key is of type SpeechScopeResourceValue.
    ::std::unordered_map<::std::wstring, SpeechScopeResourceValue> speechResources;

    // Represents language resources associated with the text property. It is also a dictionary where each key identifies a language
    // supported for text translation. The value associated with the key is of type TextScopeResourceValue.
    ::std::unordered_map<::std::wstring, TextScopeResourceValue> textResources;

    // Represents language resources associated with the text-to-speech property. It is a dictionary where each key identifies a
    // supported voice. The value associated with the key is of type SynthesisScopeResourceValue.
    ::std::unordered_map<::std::wstring, SynthesisScopeResourceValue> voiceResources;

} TranslationLanguageResource;

/*
* Gets available language resources supported by the translation service.
* @param scopes: specifies which language scopes to query. Combing each scope (bitwise OR) is supported. See https://docs.microsofttranslator.com/speech-translate.html for details.
* @param acceptLanguage: specifies the language (BCP 47 language tag), in which names of languages and regions are returned.
* @return Supported translation language resources.
*/
inline TranslationLanguageResource GetLanguageResource(LanguageResourceScope scopes, ::std::wstring acceptLanguage)
{
    TranslationLanguageResource discoveredResources{ {},{},{} };
    UNUSED(scopes);
    UNUSED(acceptLanguage);
    SPX_THROW_ON_FAIL(SPXERR_NOT_IMPL);
    return discoveredResources;
}

/*
* The translation recognizer.
* TODO: We might want to have 2 TranslationRecognizer: one is TranslationRecoginizerText for text-only translation result, and the 
* other is TranslationRecognizer for both text and audio results. See work item  <1127978>.
*/
class TranslationRecognizer final : virtual public AsyncRecognizer<TranslationResult, TranslationTextResultEventArgs>
{
public:

    // The AsyncRecognizer only deals with events for translation text result. The audio output event
    // is managed by OnTranslationSynthesisResult.
    using BaseType = AsyncRecognizer<TranslationResult, TranslationTextResultEventArgs>;

    TranslationRecognizer(SPXRECOHANDLE hreco) :
        BaseType(hreco),
        // Todo: OnTranslationError(m_onTranslationError),
        Parameters(hreco),
        OnTranslationSynthesisResult(GetTranslationAudioEventConnectionsChangedCallback(), GetTranslationAudioEventConnectionsChangedCallback())
    {
        SPX_DBG_TRACE_FUNCTION();
    }

    ~TranslationRecognizer()
    {
        SPX_DBG_TRACE_FUNCTION();
    }

    std::future<std::shared_ptr<TranslationResult>> RecognizeAsync() override
    {
        return BaseType::RecognizeAsyncInternal();
    }

    std::future<void> StartContinuousRecognitionAsync() override
    { 
        return BaseType::StartContinuousRecognitionAsyncInternal();
    }
    
    std::future<void> StopContinuousRecognitionAsync() override { return BaseType::StopContinuousRecognitionAsyncInternal(); }

    RecognizerParameterValueCollection Parameters;

    std::future<void> StartKeywordRecognitionAsync(const std::wstring& keyword) override
    {
        UNUSED(keyword);
        auto future = std::async(std::launch::async, [=]() -> void {
            SPX_THROW_ON_FAIL(SPXERR_NOT_IMPL);
        });

        return future;
    };

    std::future<void> StopKeywordRecognitionAsync() override
    {
        auto future = std::async(std::launch::async, [=]() -> void {
            SPX_THROW_ON_FAIL(SPXERR_NOT_IMPL);
        });

        return future;
    };


    /*
    * Defines translation specific events.
    * TODO: Currently, AsyncRecognizer defines events for speech recognitions. Some of events in AsyncRecognizer
    * is indeed not related to translation or other services like TTS. The AsyncRecognizer should be refactored 
    * to only include common events.
    */
    EventSignal<const TranslationSynthesisResultEventArgs&> OnTranslationSynthesisResult;
    // Todo: how to expose the error event of translation. Need to align with what speech does.
    // EventSignal<const TranslationEventArgs<TranslationTextResult>&>& OnTranslationError;

private:

    TranslationRecognizer() = delete;
    TranslationRecognizer(TranslationRecognizer&&) = delete;
    TranslationRecognizer(const TranslationRecognizer&) = delete;
    TranslationRecognizer& operator=(TranslationRecognizer&&) = delete;
    TranslationRecognizer& operator=(const TranslationRecognizer&) = delete;

    friend class CARBON_NAMESPACE_ROOT::Session;

    std::function<void(const EventSignal<const TranslationSynthesisResultEventArgs&>&)> GetTranslationAudioEventConnectionsChangedCallback()
    {
        return [=](const EventSignal<const TranslationSynthesisResultEventArgs&>& audioEvent) {
            if (&audioEvent == &OnTranslationSynthesisResult)
            {
                TranslationRecognizer_TranslationSynthesis_SetEventCallback(m_hreco, OnTranslationSynthesisResult.IsConnected() ? FireEvent_TranslationSynthesisResult : nullptr, this);
            }
        };
    }

    static void FireEvent_TranslationSynthesisResult(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<TranslationSynthesisResultEventArgs> recoEvent{ new TranslationSynthesisResultEventArgs(hevent) };

        auto pThis = static_cast<TranslationRecognizer*>(pvContext);
        pThis->OnTranslationSynthesisResult.Signal(*recoEvent.get());
    }
};


} } } // CARBON_NAMESPACE_ROOT::Recognition::Translation

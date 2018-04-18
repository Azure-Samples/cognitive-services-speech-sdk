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
#include <speechapi_cxx_common.h>
#include <speechapi_c.h>
#include <speechapi_cxx_recognition_async_recognizer.h>
#include <speechapi_cxx_translation_result.h>
#include <speechapi_cxx_translation_eventargs.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Recognition {
namespace Translation {

/*
   Defines scopes for language resources.
   Clients use the scope define which sets of languages they are interested in.
     TRANSLATION_LANGUAGE_RESOURCE_SCOPE_SPEECH: Languages available to transcribe speech into text.
     TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TEXT: languages available to translate transcribed text.
     TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TTS: languages and voices available to synthesize translated text back into speech.
   Scopes can be combined via bit-or.
*/
typedef unsigned int LanguageResourceScope;
#define TRANSLATION_LANGUAGE_RESOURCE_SCOPE_SPEECH 0x01
#define TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TEXT 0x02
#define TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TTS 0x04

/// <summary>
///  Resource value defined for the TRANSLATION_LANGUAGE_RESOURCE_SCOPE_SPEECH scope.
/// </summary>
class SpeechScopeResourceValue
{
public:
    /// <summary> name represents display name of the language.</summary>
    std::wstring name;
    /// <summary> language is the language tag in BCP-47 format of the associated written language.</summary>
    std::wstring language;
};

/// <summary>
/// Resource value defined for the TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TEXT scope.
/// </summary>
class TextScopeResourceValue
{
public:
    /// <summary> name is the display name of the written language.</summary>
    std::wstring name;
    /// <summary> dir represents the irectionality which is "rtl" for right-to-left languages, or "ltr" for left-to-right languages.</summary>
    std::wstring dir;
};

/// <summary>
/// Resource value defined for the TRANSLATION_LANGUAGE_RESOURCE_SCOPE_TTS scope.
/// </summary>

class SynthesisScopeResourceValue
{
public:
    /// <summary>displayName is the display name of the voice.</summary>
    std::wstring displayName;
    /// <summary>gender describes gender of the voice(male or female).</summary>
    std::wstring gender;
    /// <summary>locale is the language tag of the voice with primary language subtag and region subtag. </summary>
    std::wstring locale;
    /// <summary>languageName is the display name of the language.</summary>
    std::wstring languageName;
    /// <summary>regionName is the display name of the region for this language.</summary>
    std::wstring regionName;
    /// <summary>language represents the language tag of the associated written language.</summary>
    std::wstring language;
};

/// <summary>
/// Language resources that are supported by the translation service.
/// See https://docs.microsofttranslator.com/speech-translate.html for details.
/// </summary>
class TranslationLanguageResource
{
public:
    TranslationLanguageResource()
    {
        // Todo: get resource from service.
    }

     /// <summary>
    /// speechResources contains languages supported by speech-to-text. It is a dictionary of (key, value) pairs.
    /// Each key identifies a language supported by speech-to-text. The value associated with the key is of type <see cref="SpeechScopeResourceValue"/>.
    /// </summary>
    std::unordered_map<std::wstring, SpeechScopeResourceValue> speechResources;

    /// <summary>
    /// textResoruces contains languages supported by text translation. It is also a dictionary where each key identifies a language
    /// supported by text translation. The value associated with the key is of type <see cref="TextScopeResourceValue"/>.
    /// </summary>
    std::unordered_map<std::wstring, TextScopeResourceValue> textResources;

    /// <summary>
    /// voiceResources contains languages supported by text-to-speech. It is a dictionary where key is the lagnuage tag that supports voice output. 
    /// The value associated with each key is of type <see cref="SynthesisScopeResourceValue"/>.
    /// </summary>
    std::unordered_map<std::wstring, SynthesisScopeResourceValue> voiceResources;
};

/// <summary>
/// Performs translation on the speech input.
/// </summary>
class TranslationRecognizer final : virtual public AsyncRecognizer<TranslationTextResult, TranslationTextResultEventArgs>
{
public:

    // The AsyncRecognizer only deals with events for translation text result. The audio output event
    // is managed by OnTranslationSynthesisResult.
    using BaseType = AsyncRecognizer<TranslationTextResult, TranslationTextResultEventArgs>;

    /// <summary>
    /// It is intended for internal use only. It creates an instance of <see cref="TranslationRecognizer">. 
    /// </summary>
    /// <remarks>
    /// It is recommended to use RecognizerFactory to create an instance of <see cref="TranslationRecognizer">. This method is mainly
    /// used in case where a recognizer handle has been created by methods via C-API like RecognizerFactory_CreateTranslationRecognizer().
    /// </remarks>
    /// <param name="hreco">The handle of the recognizer that is returned by RecognizerFactory_CreateTranslationRecognizer().</param>
    TranslationRecognizer(SPXRECOHANDLE hreco) :
        BaseType(hreco),
        // Todo: OnTranslationError(m_onTranslationError),
        Parameters(hreco),
        TranslationSynthesisResultEvent(GetTranslationAudioEventConnectionsChangedCallback(), GetTranslationAudioEventConnectionsChangedCallback())
    {
        SPX_DBG_TRACE_FUNCTION();
    }

    /// <summary>
    /// Deconstructs the instance.
    /// </summary>
    ~TranslationRecognizer()
    {
        SPX_DBG_TRACE_FUNCTION();
    }

    /// <summary>
    /// Starts translation recognition as an asynchronous operation, and stops after the first utterance is recognized. The asynchronous operation returns <see creaf="TranslationTextResult"/> as result.
    /// </summary>
    /// <returns>An asynchronous operation representing the recognition. It returns a value of <see cref="TranslationTextResult"/> as result.</returns>
    std::future<std::shared_ptr<TranslationTextResult>> RecognizeAsync() override
    {
        return BaseType::RecognizeAsyncInternal();
    }

    /// <summary>
    /// Starts translation on a continous audio stream, until StopContinuousRecognitionAsync() is called.
    /// User must subscribe to events to receive recognition results.
    /// </summary>
    /// <returns>An asynchronous operation that starts the translation.</returns>
    std::future<void> StartContinuousRecognitionAsync() override
    { 
        return BaseType::StartContinuousRecognitionAsyncInternal();
    }

    /// <summary>
    /// Stops continuous translation.
    /// </summary>
    /// <returns>A task representing the asynchronous operation that stops the translation.</returns>
    std::future<void> StopContinuousRecognitionAsync() override { return BaseType::StopContinuousRecognitionAsyncInternal(); }

    /// <summary>
    /// The collection of parameters and their values defined for this <see cref="TranslationRecognizer"/>.
    /// </summary>
    RecognizerParameterValueCollection Parameters;

    /// <summary>
    /// Note: NOT implemented. Starts keyword recognition on a continous audio stream, until StopKeywordRecognitionAsync() is called.
    /// </summary>
    /// <param name="keyword">Specifies the keyword phrase to be recognized.</param>
    /// <returns>An asynchronous operation that starts the keyword recognition.</returns>
    std::future<void> StartKeywordRecognitionAsync(const std::wstring& keyword) override
    {
        UNUSED(keyword);
        auto future = std::async(std::launch::async, [=]() -> void {
            SPX_THROW_ON_FAIL(SPXERR_NOT_IMPL);
        });

        return future;
    };

    /// <summary>
    /// Note: NOT implemented. Stops continuous keyword recognition.
    /// </summary>
    /// <returns>A task representing the asynchronous operation that stops the keyword recognition.</returns>
    std::future<void> StopKeywordRecognitionAsync() override
    {
        auto future = std::async(std::launch::async, [=]() -> void {
            SPX_THROW_ON_FAIL(SPXERR_NOT_IMPL);
        });

        return future;
    };


    /// <summary>
    /// The event signals that a translation synthesis result is received.
    /// </summary>
    EventSignal<const TranslationSynthesisResultEventArgs&> TranslationSynthesisResultEvent;
    // Todo: how to expose the error event of translation. Need to align with what speech does.
    // EventSignal<const TranslationEventArgs<TranslationTextResult>&>& OnTranslationError;


private:

    TranslationRecognizer() = delete;
    TranslationRecognizer(TranslationRecognizer&&) = delete;
    TranslationRecognizer(const TranslationRecognizer&) = delete;
    TranslationRecognizer& operator=(TranslationRecognizer&&) = delete;
    TranslationRecognizer& operator=(const TranslationRecognizer&) = delete;

    friend class Microsoft::CognitiveServices::Speech::Session;

    std::function<void(const EventSignal<const TranslationSynthesisResultEventArgs&>&)> GetTranslationAudioEventConnectionsChangedCallback()
    {
        return [=](const EventSignal<const TranslationSynthesisResultEventArgs&>& audioEvent) {
            if (&audioEvent == &TranslationSynthesisResultEvent)
            {
                TranslationRecognizer_TranslationSynthesis_SetEventCallback(m_hreco, TranslationSynthesisResultEvent.IsConnected() ? FireEvent_TranslationSynthesisResult : nullptr, this);
            }
        };
    }

    static void FireEvent_TranslationSynthesisResult(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hreco);
        std::unique_ptr<TranslationSynthesisResultEventArgs> recoEvent{ new TranslationSynthesisResultEventArgs(hevent) };

        auto pThis = static_cast<TranslationRecognizer*>(pvContext);
        pThis->TranslationSynthesisResultEvent.Signal(*recoEvent.get());
    }
};


} } } } } // Microsoft::CognitiveServices::Speech::Recognition::Translation

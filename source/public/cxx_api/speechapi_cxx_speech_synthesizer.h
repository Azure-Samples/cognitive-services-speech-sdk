//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_speech_synthesizer.h: Public API declarations for SpeechSynthesizer C++ class
//

#pragma once
#include <future>
#include <memory>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_c.h>
#include <speechapi_cxx_properties.h>
#include <speechapi_cxx_speech_config.h>
#include <speechapi_cxx_auto_detect_source_lang_config.h>
#include <speechapi_cxx_utils.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Class for speech synthesizer.
/// Updated in version 1.13.0
/// </summary>
class SpeechSynthesizer : public std::enable_shared_from_this<SpeechSynthesizer>
{
private:

    /// <summary>
    /// Internal member variable that holds the speech synthesizer handle.
    /// </summary>
    SPXSYNTHHANDLE m_hsynth;

    std::shared_ptr<Audio::AudioConfig> m_audioConfig;

    /*! \cond PRIVATE */

    class PrivatePropertyCollection : public PropertyCollection
    {
    public:
        PrivatePropertyCollection(SPXSYNTHHANDLE hsynth) :
            PropertyCollection(
                [=]() {
            SPXPROPERTYBAGHANDLE hpropbag = SPXHANDLE_INVALID;
            synthesizer_get_property_bag(hsynth, &hpropbag);
            return hpropbag;
        }())
        {
        }
    };

    /// <summary>
    /// Internal member variable that holds the properties of the speech synthesizer
    /// </summary>
    PrivatePropertyCollection m_properties;

    /*! \endcond */

public:

    /// <summary>
    /// Create a speech synthesizer from a speech config.
    /// </summary>
    /// <param name="speechconfig">Speech configuration.</param>
    /// <returns>A smart pointer wrapped speech synthesizer pointer.</returns>
    static std::shared_ptr<SpeechSynthesizer> FromConfig(std::shared_ptr<SpeechConfig> speechconfig, std::nullptr_t)
    {
        SPXSYNTHHANDLE hsynth = SPXHANDLE_INVALID;

        SPX_THROW_ON_FAIL(::synthesizer_create_speech_synthesizer_from_config(
            &hsynth,
            Utils::HandleOrInvalid<SPXSPEECHCONFIGHANDLE, SpeechConfig>(speechconfig),
            SPXHANDLE_INVALID));

        auto ptr = new SpeechSynthesizer(hsynth);
        return std::shared_ptr<SpeechSynthesizer>(ptr);
    }

    /// <summary>
    /// Create a speech synthesizer from a speech config and audio config.
    /// </summary>
    /// <param name="speechconfig">Speech configuration.</param>
    /// <param name="audioconfig">Audio configuration.</param>
    /// <returns>A smart pointer wrapped speech synthesizer pointer.</returns>
    static std::shared_ptr<SpeechSynthesizer> FromConfig(
        std::shared_ptr<SpeechConfig> speechconfig,
        std::shared_ptr<Audio::AudioConfig> audioconfig = Audio::AudioConfig::FromDefaultSpeakerOutput())
    {
        SPXSYNTHHANDLE hsynth = SPXHANDLE_INVALID;

        SPX_THROW_ON_FAIL(::synthesizer_create_speech_synthesizer_from_config(
            &hsynth,
            Utils::HandleOrInvalid<SPXSPEECHCONFIGHANDLE, SpeechConfig>(speechconfig),
            Utils::HandleOrInvalid<SPXAUDIOCONFIGHANDLE, Audio::AudioConfig>(audioconfig)));

        auto ptr = new SpeechSynthesizer(hsynth);
        auto synthesizer = std::shared_ptr<SpeechSynthesizer>(ptr);
        synthesizer->m_audioConfig = audioconfig;
        return synthesizer;
    }

    /// <summary>
    /// Create a speech synthesizer from a speech config, auto detection source language config and audio config
    /// Added in 1.13.0
    /// </summary>
    /// <param name="speechconfig">Speech configuration.</param>
    /// <param name="autoDetectSourceLangConfig">Auto detection source language config.</param>
    /// <param name="audioconfig">Audio configuration.</param>
    /// <returns>A smart pointer wrapped speech synthesizer pointer.</returns>
    static std::shared_ptr<SpeechSynthesizer> FromConfig(
        std::shared_ptr<SpeechConfig> speechconfig,
        std::shared_ptr<AutoDetectSourceLanguageConfig> autoDetectSourceLangConfig,
        std::shared_ptr<Audio::AudioConfig> audioconfig = Audio::AudioConfig::FromDefaultSpeakerOutput())
    {
        SPXSYNTHHANDLE hsynth;

        SPX_THROW_ON_FAIL(::synthesizer_create_speech_synthesizer_from_auto_detect_source_lang_config(
            &hsynth,
            Utils::HandleOrInvalid<SPXSPEECHCONFIGHANDLE, SpeechConfig>(speechconfig),
            Utils::HandleOrInvalid<SPXAUTODETECTSOURCELANGCONFIGHANDLE, AutoDetectSourceLanguageConfig>(autoDetectSourceLangConfig),
            Utils::HandleOrInvalid<SPXAUDIOCONFIGHANDLE, Audio::AudioConfig>(audioconfig)));

        auto ptr = new SpeechSynthesizer(hsynth);
        auto synthesizer = std::shared_ptr<SpeechSynthesizer>(ptr);
        synthesizer->m_audioConfig = audioconfig;
        return synthesizer;
    }

    /// <summary>
    /// Execute the speech synthesis on plain text, synchronously.
    /// </summary>
    /// <param name="text">The plain text for synthesis.</param>
    /// <returns>A smart pointer wrapping a speech synthesis result.</returns>
    std::shared_ptr<SpeechSynthesisResult> SpeakText(const std::string& text)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(::synthesizer_speak_text(m_hsynth, text.data(), static_cast<uint32_t>(text.length()), &hresult));

        return std::make_shared<SpeechSynthesisResult>(hresult);
    }

#ifndef SWIG
    /// <summary>
    /// Execute the speech synthesis on plain text, synchronously.
    /// Added in 1.9.0
    /// </summary>
    /// <param name="text">The plain text for synthesis.</param>
    /// <returns>A smart pointer wrapping a speech synthesis result.</returns>
    std::shared_ptr<SpeechSynthesisResult> SpeakText(const std::wstring& text)
    {
        return SpeakText(Utils::ToUTF8(text));
    }
#endif

    /// <summary>
    /// Execute the speech synthesis on SSML, synchronously.
    /// </summary>
    /// <param name="ssml">The SSML for synthesis.</param>
    /// <returns>A smart pointer wrapping a speech synthesis result.</returns>
    std::shared_ptr<SpeechSynthesisResult> SpeakSsml(const std::string& ssml)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(::synthesizer_speak_ssml(m_hsynth, ssml.data(), static_cast<uint32_t>(ssml.length()), &hresult));

        return std::make_shared<SpeechSynthesisResult>(hresult);
    }

#ifndef SWIG
    /// <summary>
    /// Execute the speech synthesis on SSML, synchronously.
    /// Added in version 1.9.0
    /// </summary>
    /// <param name="ssml">The SSML for synthesis.</param>
    /// <returns>A smart pointer wrapping a speech synthesis result.</returns>
    std::shared_ptr<SpeechSynthesisResult> SpeakSsml(const std::wstring& ssml)
    {
        return SpeakSsml(Utils::ToUTF8(ssml));
    }
#endif

    /// <summary>
    /// Execute the speech synthesis on plain text, asynchronously.
    /// </summary>
    /// <param name="text">The plain text for synthesis.</param>
    /// <returns>An asynchronous operation representing the synthesis. It returns a value of <see cref="SpeechSynthesisResult"/> as result.</returns>
    std::future<std::shared_ptr<SpeechSynthesisResult>> SpeakTextAsync(const std::string& text)
    {
        auto keepAlive = this->shared_from_this();

        auto future = std::async(std::launch::async, [keepAlive, this, text]() -> std::shared_ptr<SpeechSynthesisResult> {
            SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
            SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::synthesizer_speak_text_async(m_hsynth, text.data(), static_cast<uint32_t>(text.length()), &hasync));
            SPX_EXITFN_ON_FAIL(::synthesizer_speak_async_wait_for(hasync, UINT32_MAX, &hresult));

        SPX_EXITFN_CLEANUP:
            auto releaseHr = synthesizer_async_handle_release(hasync);
            SPX_REPORT_ON_FAIL(releaseHr);

            return std::make_shared<SpeechSynthesisResult>(hresult);
        });

        return future;
    }

#ifndef SWIG
    /// <summary>
    /// Execute the speech synthesis on plain text, asynchronously.
    /// Added in version 1.9.0
    /// </summary>
    /// <param name="text">The plain text for synthesis.</param>
    /// <returns>An asynchronous operation representing the synthesis. It returns a value of <see cref="SpeechSynthesisResult"/> as result.</returns>
    std::future<std::shared_ptr<SpeechSynthesisResult>> SpeakTextAsync(const std::wstring& text)
    {
        return SpeakTextAsync(Utils::ToUTF8(text));
    }
#endif

    /// <summary>
    /// Execute the speech synthesis on SSML, asynchronously.
    /// </summary>
    /// <param name="ssml">The SSML for synthesis.</param>
    /// <returns>An asynchronous operation representing the synthesis. It returns a value of <see cref="SpeechSynthesisResult"/> as result.</returns>
    std::future<std::shared_ptr<SpeechSynthesisResult>> SpeakSsmlAsync(const std::string& ssml)
    {
        auto keepAlive = this->shared_from_this();

        auto future = std::async(std::launch::async, [keepAlive, this, ssml]() -> std::shared_ptr<SpeechSynthesisResult> {
            SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
            SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::synthesizer_speak_ssml_async(m_hsynth, ssml.data(), static_cast<uint32_t>(ssml.length()), &hasync));
            SPX_EXITFN_ON_FAIL(::synthesizer_speak_async_wait_for(hasync, UINT32_MAX, &hresult));

        SPX_EXITFN_CLEANUP:
            auto releaseHr = synthesizer_async_handle_release(hasync);
            SPX_REPORT_ON_FAIL(releaseHr);

            return std::make_shared<SpeechSynthesisResult>(hresult);
        });

        return future;
    }

#ifndef SWIG
    /// <summary>
    /// Execute the speech synthesis on SSML, asynchronously.
    /// Added in version 1.9.0
    /// </summary>
    /// <param name="ssml">The SSML for synthesis.</param>
    /// <returns>An asynchronous operation representing the synthesis. It returns a value of <see cref="SpeechSynthesisResult"/> as result.</returns>
    std::future<std::shared_ptr<SpeechSynthesisResult>> SpeakSsmlAsync(const std::wstring& ssml)
    {
        return SpeakSsmlAsync(Utils::ToUTF8(ssml));
    }
#endif

    /// <summary>
    /// Start the speech synthesis on plain text, synchronously.
    /// </summary>
    /// <param name="text">The plain text for synthesis.</param>
    /// <returns>A smart pointer wrapping a speech synthesis result.</returns>
    std::shared_ptr<SpeechSynthesisResult> StartSpeakingText(const std::string& text)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(::synthesizer_start_speaking_text(m_hsynth, text.data(), static_cast<uint32_t>(text.length()), &hresult));

        return std::make_shared<SpeechSynthesisResult>(hresult);
    }

#ifndef SWIG
    /// <summary>
    /// Start the speech synthesis on plain text, synchronously.
    /// Added in version 1.9.0
    /// </summary>
    /// <param name="text">The plain text for synthesis.</param>
    /// <returns>A smart pointer wrapping a speech synthesis result.</returns>
    std::shared_ptr<SpeechSynthesisResult> StartSpeakingText(const std::wstring& text)
    {
        return StartSpeakingText(Utils::ToUTF8(text));
    }
#endif

    /// <summary>
    /// Start the speech synthesis on SSML, synchronously.
    /// </summary>
    /// <param name="ssml">The SSML for synthesis.</param>
    /// <returns>A smart pointer wrapping a speech synthesis result.</returns>
    std::shared_ptr<SpeechSynthesisResult> StartSpeakingSsml(const std::string& ssml)
    {
        SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(::synthesizer_start_speaking_ssml(m_hsynth, ssml.data(), static_cast<uint32_t>(ssml.length()), &hresult));

        return std::make_shared<SpeechSynthesisResult>(hresult);
    }

#ifndef SWIG
    /// <summary>
    /// Start the speech synthesis on SSML, synchronously.
    /// Added in version 1.9.0
    /// </summary>
    /// <param name="ssml">The SSML for synthesis.</param>
    /// <returns>A smart pointer wrapping a speech synthesis result.</returns>
    std::shared_ptr<SpeechSynthesisResult> StartSpeakingSsml(const std::wstring& ssml)
    {
        return StartSpeakingSsml(Utils::ToUTF8(ssml));
    }
#endif

    /// <summary>
    /// Start the speech synthesis on plain text, asynchronously.
    /// </summary>
    /// <param name="text">The plain text for synthesis.</param>
    /// <returns>An asynchronous operation representing the synthesis. It returns a value of <see cref="SpeechSynthesisResult"/> as result.</returns>
    std::future<std::shared_ptr<SpeechSynthesisResult>> StartSpeakingTextAsync(const std::string& text)
    {
        auto keepAlive = this->shared_from_this();

        auto future = std::async(std::launch::async, [keepAlive, this, text]() -> std::shared_ptr<SpeechSynthesisResult> {
            SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
            SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::synthesizer_start_speaking_text_async(m_hsynth, text.data(), static_cast<uint32_t>(text.length()), &hasync));
            SPX_EXITFN_ON_FAIL(::synthesizer_speak_async_wait_for(hasync, UINT32_MAX, &hresult));

        SPX_EXITFN_CLEANUP:
            auto releaseHr = synthesizer_async_handle_release(hasync);
            SPX_REPORT_ON_FAIL(releaseHr);

            return std::make_shared<SpeechSynthesisResult>(hresult);
        });

        return future;
    }

#ifndef SWIG
    /// <summary>
    /// Start the speech synthesis on plain text, asynchronously.
    /// Added in version 1.9.0
    /// </summary>
    /// <param name="text">The plain text for synthesis.</param>
    /// <returns>An asynchronous operation representing the synthesis. It returns a value of <see cref="SpeechSynthesisResult"/> as result.</returns>
    std::future<std::shared_ptr<SpeechSynthesisResult>> StartSpeakingTextAsync(const std::wstring& text)
    {
        return StartSpeakingTextAsync(Utils::ToUTF8(text));
    }
#endif

    /// <summary>
    /// Start the speech synthesis on SSML, asynchronously.
    /// </summary>
    /// <param name="ssml">The SSML for synthesis.</param>
    /// <returns>An asynchronous operation representing the synthesis. It returns a value of <see cref="SpeechSynthesisResult"/> as result.</returns>
    std::future<std::shared_ptr<SpeechSynthesisResult>> StartSpeakingSsmlAsync(const std::string& ssml)
    {
        auto keepAlive = this->shared_from_this();

        auto future = std::async(std::launch::async, [keepAlive, this, ssml]() -> std::shared_ptr<SpeechSynthesisResult> {
            SPXRESULTHANDLE hresult = SPXHANDLE_INVALID;
            SPXASYNCHANDLE hasync = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(::synthesizer_start_speaking_ssml_async(m_hsynth, ssml.data(), static_cast<uint32_t>(ssml.length()), &hasync));
            SPX_EXITFN_ON_FAIL(::synthesizer_speak_async_wait_for(hasync, UINT32_MAX, &hresult));

        SPX_EXITFN_CLEANUP:
            auto releaseHr = synthesizer_async_handle_release(hasync);
            SPX_REPORT_ON_FAIL(releaseHr);

            return std::make_shared<SpeechSynthesisResult>(hresult);
        });

        return future;
    }

#ifndef SWIG
    /// <summary>
    /// Start the speech synthesis on SSML, asynchronously.
    /// Added in version 1.9.0
    /// </summary>
    /// <param name="ssml">The SSML for synthesis.</param>
    /// <returns>An asynchronous operation representing the synthesis. It returns a value of <see cref="SpeechSynthesisResult"/> as result.</returns>
    std::future<std::shared_ptr<SpeechSynthesisResult>> StartSpeakingSsmlAsync(const std::wstring& ssml)
    {
        return StartSpeakingSsmlAsync(Utils::ToUTF8(ssml));
    }
#endif

    /// <summary>
    /// Sets the authorization token that will be used for connecting to the service.
    /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
    /// expires, the caller needs to refresh it by calling this setter with a new valid token.
    /// Otherwise, the synthesizer will encounter errors while speech synthesis.
    /// Added in version 1.7.0
    /// </summary>
    /// <param name="token">The authorization token.</param>
    void SetAuthorizationToken(const SPXSTRING& token)
    {
        Properties.SetProperty(PropertyId::SpeechServiceAuthorization_Token, token);
    }

    /// <summary>
    /// Gets the authorization token.
    /// Added in version 1.7.0
    /// </summary>
    /// <returns>Authorization token</returns>
    SPXSTRING GetAuthorizationToken() const
    {
        return Properties.GetProperty(PropertyId::SpeechServiceAuthorization_Token, SPXSTRING());
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~SpeechSynthesizer()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

        // Disconnect the event signals in reverse construction order
        WordBoundary.DisconnectAll();
        SynthesisCanceled.DisconnectAll();
        SynthesisCompleted.DisconnectAll();
        Synthesizing.DisconnectAll();
        SynthesisStarted.DisconnectAll();

        synthesizer_handle_release(m_hsynth);
    }

    /// <summary>
    /// A collection of properties and their values defined for this <see cref="SpeechSynthesizer"/>.
    /// </summary>
    PropertyCollection& Properties;

    /// <summary>
    /// The event signals that a speech synthesis result is received when the synthesis just started.
    /// </summary>
    EventSignal<const SpeechSynthesisEventArgs&> SynthesisStarted;

    /// <summary>
    /// The event signals that a speech synthesis result is received while the synthesis is on going.
    /// </summary>
    EventSignal<const SpeechSynthesisEventArgs&> Synthesizing;

    /// <summary>
    /// The event signals that a speech synthesis result is received when the synthesis completed.
    /// </summary>
    EventSignal<const SpeechSynthesisEventArgs&> SynthesisCompleted;

    /// <summary>
    /// The event signals that a speech synthesis result is received when the synthesis is canceled.
    /// </summary>
    EventSignal<const SpeechSynthesisEventArgs&> SynthesisCanceled;

    /// <summary>
    /// The event signals that a speech synthesis word boundary is received while the synthesis is on going.
    /// Added in version 1.7.0
    /// </summary>
    EventSignal<const SpeechSynthesisWordBoundaryEventArgs&> WordBoundary;

private:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    /// <param name="hsynth">Synthesizer handle.</param>
    explicit SpeechSynthesizer(SPXRECOHANDLE hsynth) :
        m_hsynth(hsynth),
        m_properties(hsynth),
        Properties(m_properties),
        SynthesisStarted(GetSpeechSynthesisEventConnectionsChangedCallback(), GetSpeechSynthesisEventConnectionsChangedCallback(), false),
        Synthesizing(GetSpeechSynthesisEventConnectionsChangedCallback(), GetSpeechSynthesisEventConnectionsChangedCallback(), false),
        SynthesisCompleted(GetSpeechSynthesisEventConnectionsChangedCallback(), GetSpeechSynthesisEventConnectionsChangedCallback(), false),
        SynthesisCanceled(GetSpeechSynthesisEventConnectionsChangedCallback(), GetSpeechSynthesisEventConnectionsChangedCallback(), false),
        WordBoundary(GetWordBoundaryEventConnectionsChangedCallback(), GetWordBoundaryEventConnectionsChangedCallback(), false)
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    std::function<void(const EventSignal<const SpeechSynthesisEventArgs&>&)> GetSpeechSynthesisEventConnectionsChangedCallback()
    {
        return [=](const EventSignal<const SpeechSynthesisEventArgs&>& eventSignal) {
            if (&eventSignal == &SynthesisStarted)
            {
                synthesizer_started_set_callback(m_hsynth, SynthesisStarted.IsConnected() ? FireEvent_SynthesisStarted : nullptr, this);
            }
            else if (&eventSignal == &Synthesizing)
            {
                synthesizer_synthesizing_set_callback(m_hsynth, Synthesizing.IsConnected() ? FireEvent_Synthesizing : nullptr, this);
            }
            else if (&eventSignal == &SynthesisCompleted)
            {
                synthesizer_completed_set_callback(m_hsynth, SynthesisCompleted.IsConnected() ? FireEvent_SynthesisCompleted : nullptr, this);
            }
            else if (&eventSignal == &SynthesisCanceled)
            {
                synthesizer_canceled_set_callback(m_hsynth, SynthesisCanceled.IsConnected() ? FireEvent_SynthesisCanceled : nullptr, this);
            }
        };
    }

    std::function<void(const EventSignal<const SpeechSynthesisWordBoundaryEventArgs&>&)> GetWordBoundaryEventConnectionsChangedCallback()
    {
        return [=](const EventSignal<const SpeechSynthesisWordBoundaryEventArgs&>& eventSignal) {
            if (&eventSignal == &WordBoundary)
            {
                synthesizer_word_boundary_set_callback(m_hsynth, WordBoundary.IsConnected() ? FireEvent_WordBoundary : nullptr, this);
            }
        };
    }

    static void FireEvent_SynthesisStarted(SPXRECOHANDLE hsynth, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hsynth);
        std::unique_ptr<SpeechSynthesisEventArgs> synthEvent{ new SpeechSynthesisEventArgs(hevent) };

        auto pThis = static_cast<SpeechSynthesizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->SynthesisStarted.Signal(*synthEvent.get());
    }

    static void FireEvent_Synthesizing(SPXRECOHANDLE hsynth, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hsynth);
        std::unique_ptr<SpeechSynthesisEventArgs> synthEvent{ new SpeechSynthesisEventArgs(hevent) };

        auto pThis = static_cast<SpeechSynthesizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->Synthesizing.Signal(*synthEvent.get());
    }

    static void FireEvent_SynthesisCompleted(SPXRECOHANDLE hsynth, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hsynth);
        std::unique_ptr<SpeechSynthesisEventArgs> synthEvent{ new SpeechSynthesisEventArgs(hevent) };

        auto pThis = static_cast<SpeechSynthesizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->SynthesisCompleted.Signal(*synthEvent.get());
    }

    static void FireEvent_SynthesisCanceled(SPXRECOHANDLE hsynth, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hsynth);
        std::unique_ptr<SpeechSynthesisEventArgs> synthEvent{ new SpeechSynthesisEventArgs(hevent) };

        auto pThis = static_cast<SpeechSynthesizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->SynthesisCanceled.Signal(*synthEvent.get());
    }

    static void FireEvent_WordBoundary(SPXRECOHANDLE hsynth, SPXEVENTHANDLE hevent, void* pvContext)
    {
        UNUSED(hsynth);
        std::unique_ptr<SpeechSynthesisWordBoundaryEventArgs> wordBoundaryEvent{ new SpeechSynthesisWordBoundaryEventArgs(hevent) };

        auto pThis = static_cast<SpeechSynthesizer*>(pvContext);
        auto keepAlive = pThis->shared_from_this();
        pThis->WordBoundary.Signal(*wordBoundaryEvent.get());
    }
};


} } } // Microsoft::CognitiveServices::Speech

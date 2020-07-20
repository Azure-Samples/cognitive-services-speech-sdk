#include "stdafx.h"

#include "audio_session_shim.h"
#include "property_id_2_name_map.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

void CSpxAudioSessionShim::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    InitSiteKeepAlive();
}

void CSpxAudioSessionShim::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    EnsureStopAudioSource();
    TermAudioSource();

    TermSiteKeepAlive();
}

void CSpxAudioSessionShim::StartAudio()
{
    EnsureStartAudioSource();
}

void CSpxAudioSessionShim::StopAudio()
{
    EnsureStopAudioSource();
}

/// <summary>
/// CSpxAudioSessionShim::InitSiteKeepAlive - Ensures the Session's Site is kept alive for the duration of the lifetime of this Session.
/// </summary>
/// <remarks> Due to current owenrship model, and our late-into-the-cycle changes for SpeechConfig objects
/// the CSpxSession is sited to the CSpxApiFactory. This ApiFactory is not held by the dev user at or above
/// the CAPI. Thus ... we must hold it alive in order for the properties to be obtainable via the standard
/// ISpxNamedProperties mechanisms... It will be released on ::Term()
/// </remarks>
void CSpxAudioSessionShim::InitSiteKeepAlive()
{
    m_siteKeepAlive = GetSite();
}

/// <summary>
/// CSpxAudioSessionShim::TermSiteKeepAlive - Ensures the Session's Site is released.
/// </summary>
void CSpxAudioSessionShim::TermSiteKeepAlive()
{
    m_siteKeepAlive.reset();
    SPX_DBG_ASSERT(m_siteKeepAlive == nullptr);
}

/// <summary>
/// ISpxAudioSourceInitDelegateImpl::InitDelegatePtr - Initialize the AudioSourceInit DelegatePtr on first use.
/// </summary>
void CSpxAudioSessionShim::InitDelegatePtr(std::shared_ptr<ISpxAudioSourceInit>& ptr)
{
    // Obtain the ISpxAudioSourceInit interface from the underlying AudioSource object
    ptr = SpxQueryInterface<ISpxAudioSourceInit>(CSpxSessionAudioSourceHelper::EnsureInitAudioSource());
}

void CSpxAudioSessionShim::InitDelegatePtr(std::shared_ptr<ISpxAudioSourceNotifyMe>& ptr)
{
    ptr = SpxQueryInterface<ISpxAudioSourceNotifyMe>(CSpxSessionAudioSourceHelper::EnsureInitAudioSource());
}

/// <summary>
/// CSpxSession::TermAudioSource - Terminate the underlying AudioSource after releasing all references obtained from it.
/// </summary>
void CSpxAudioSessionShim::TermAudioSource()
{
    // Zombie and Clear the AudioSourceInit DelegatePtr
    AudioSourceInitDelegate::Zombie(true);
    AudioSourceInitDelegate::Clear();
    SPX_DBG_ASSERT(AudioSourceInitDelegate::IsClear());

    // Terminate the underlying AudioSource
    CSpxSessionAudioSourceHelper::TermAudioSource();
    SPX_DBG_ASSERT(CSpxSessionAudioSourceHelper::IsClear());
}

void CSpxAudioSessionShim::AudioSourceDataAvailable(bool /* first */)
{
    auto site = GetSite();
    auto ptr = site->QueryInterface<ISpxAudioProcessor>();
    if (ptr != nullptr)
    {
        std::string userId{};
        std::string timestamp{};
        TryQueryInterface<ISpxAudioSourceBufferProperties>(m_data, [&userId, &timestamp](ISpxAudioSourceBufferProperties& props)
        {
            auto propValue = props.GetBufferProperty(GetPropertyName(PropertyId::DataBuffer_UserId), "");
            userId = propValue != nullptr ? propValue.get() : "";
            propValue = props.GetBufferProperty(GetPropertyName(PropertyId::DataBuffer_TimeStamp), "");
            timestamp = propValue != nullptr ? propValue.get() : "";
        });

        auto ready = static_cast<uint32_t>(m_data->GetBytesReady());
        auto buffer = SpxAllocSharedBuffer<uint8_t>(ready);
        m_data->Read(buffer.get(), ready);
        auto chunk = std::make_shared<DataChunk>(std::move(buffer), ready, std::move(timestamp), std::move(userId));
        ptr->ProcessAudio(chunk);
    }
}

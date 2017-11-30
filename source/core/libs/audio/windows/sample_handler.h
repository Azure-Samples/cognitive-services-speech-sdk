#pragma once
#include "spxcore_common.h"
#include <mfreadwrite.h>
#include "iunknown.h"
#include <atlbase.h>


namespace CARBON_IMPL_NAMESPACE() {

class ISpxAudioProcessor;
struct WAVEFORMATEX;

class SampleHandler : public IUnknownBase<IMFSourceReaderCallback>
{
public:
    using Sink_Type = std::shared_ptr<ISpxAudioProcessor>;

    SampleHandler(GUID major, GUID subType);
    virtual ~SampleHandler() = default;

    SPXHR Start(const Sink_Type& sink);
    void Stop();

    // IMFSourceReaderCallback
    virtual STDMETHODIMP_(HRESULT) OnEvent(_In_ DWORD dwStreamIndex, _In_ IMFMediaEvent *pEvent) override;
    virtual STDMETHODIMP_(HRESULT) OnFlush(_In_ DWORD dwStreamIndex) override;
    virtual STDMETHODIMP_(HRESULT) OnReadSample(_In_ HRESULT hrStatus, _In_ DWORD dwStreamIndex, _In_ DWORD dwStreamFlags,
        _In_ LONGLONG llTimestamp, _In_opt_ IMFSample *pSample) override;
    
    SPXHR SetStreamReader(_In_ IMFSourceReader* reader, DWORD streamIndex);

    virtual const WAVEFORMATEX& GetFormat() = 0;

private:
    SPXHR QueueRead();
    SPXHR Process(IMFSample *sample);
    SPXHR ConfigureStream(_In_ IMFSourceReader* reader, DWORD streamIndex);
    SPXHR CreateMediaType(_Inout_ CComPtr<IMFMediaType>& mediaType);

    ATL::CComPtr<IMFSourceReader> m_reader;
    ATL::CComPtr<IMFMediaType> m_audioType;
    DWORD m_streamIndex;
    GUID m_majorType;
    GUID m_subType;
    Sink_Type m_sink;
};

}; // CARBON_IMPL_NAMESPACE()
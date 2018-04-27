#pragma once
#include <atomic>
#include <future>
#include <memory>
#include "asyncop.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "named_properties_impl.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxRecognizer :
    public ISpxRecognizer,
    public ISpxRecognizerEvents,
    public ISpxNamedPropertiesImpl,
    public ISpxSessionFromRecognizer,
    public ISpxObjectWithSiteInitImpl<ISpxRecognizerSite>
{
public:

    CSpxRecognizer();
    virtual ~CSpxRecognizer();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxSessionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizerEvents)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizer)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectWithSiteInit
    void Init() override;
    void Term() override;

    // --- ISpxNamedProperties (overrides)
    void SetStringValue(const wchar_t* name, const wchar_t* value) override;

    // --- ISpxRecognizer

    bool IsEnabled() override;
    void Enable() override;
    void Disable() override;

    CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync() override;
    CSpxAsyncOp<void> StartContinuousRecognitionAsync() override;
    CSpxAsyncOp<void> StopContinuousRecognitionAsync() override;

    CSpxAsyncOp<void> StartKeywordRecognitionAsync(const std::wstring& keyword) override;
    CSpxAsyncOp<void> StopKeywordRecognitionAsync() override;

    // --- ISpxSessionFromRecognizer

    std::shared_ptr<ISpxSession> GetDefaultSession() override;

    // --- ISpxRecognizerEvents

    void FireSessionStarted(const std::wstring& sessionId) override;
    void FireSessionStopped(const std::wstring& sessionId) override;

    void FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset) override;
    void FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset) override;

    void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) override;


protected:

    void EnsureDefaultSession();
    void TermDefaultSession();
    
    void OnIsEnabledChanged();

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() override;


private:

    CSpxRecognizer(const CSpxRecognizer&) = delete;
    CSpxRecognizer(const CSpxRecognizer&&) = delete;

    CSpxRecognizer& operator=(const CSpxRecognizer&) = delete;

    std::shared_ptr<ISpxSession> m_defaultSession;
    std::atomic_bool m_fEnabled;

    void FireRecoEvent(ISpxRecognizerEvents::RecoEvent_Type* pevent, const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result, uint64_t offset = 0);
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

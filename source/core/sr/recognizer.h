#pragma once
#include <atomic>
#include <future>
#include <memory>
#include "asyncop.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include "service_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxRecognizer :
    public ISpxRecognizer,
    public ISpxRecognizerEvents,
    public ISpxServiceProvider,
    public ISpxSessionFromRecognizer,
    public ISpxObjectWithSiteInitImpl<ISpxRecognizerSite>,
    public ISpxPropertyBagImpl,
    public ISpxConnectionFromRecognizer,
    public ISpxGenericSite,
    public ISpxConnectionSite
{
public:

    CSpxRecognizer();
    virtual ~CSpxRecognizer();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxSessionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizerEvents)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxConnectionFromRecognizer)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectWithSiteInit
    void Init() override;
    void Term() override;

    // --- ISpxNamedProperties (overrides)
    void SetStringValue(const char* name, const char* value) override;

    // --- ISpxRecognizer
    bool IsEnabled() override;
    void Enable() override;
    void Disable() override;

    CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync() override;
    CSpxAsyncOp<void> StartContinuousRecognitionAsync() override;
    CSpxAsyncOp<void> StopContinuousRecognitionAsync() override;

    CSpxAsyncOp<void> StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model) override;
    CSpxAsyncOp<void> StopKeywordRecognitionAsync() override;

    // --- ISpxSessionFromRecognizer
    std::shared_ptr<ISpxSession> GetDefaultSession() override;

    // --- ISpxRecognizerEvents
    void FireSessionStarted(const std::wstring& sessionId) override;
    void FireSessionStopped(const std::wstring& sessionId) override;

    void FireConnected(const std::wstring& sessionId) override;
    void FireDisconnected(const std::wstring& sessionId) override;

    void FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset) override;
    void FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset) override;

    void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxConnectionFromRecognizer
    std::shared_ptr<ISpxConnection> GetConnection() override;

    // --- ISpxConnectionSite
    std::shared_ptr<ISpxRecognizer> GetRecognizer() override;

protected:
    void EnsureDefaultSession();
    void TermDefaultSession();
    
    void OnIsEnabledChanged();

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;


private:

    CSpxRecognizer(const CSpxRecognizer&) = delete;
    CSpxRecognizer(const CSpxRecognizer&&) = delete;

    CSpxRecognizer& operator=(const CSpxRecognizer&) = delete;

    std::shared_ptr<ISpxSession> m_defaultSession;
    std::atomic_bool m_fEnabled;

    void SetStringValueInProperties(const char* name, const char* value);
    std::string GetStringValueFromProperties(const char* name, const char* defaultValue);
    void FireRecoEvent(ISpxRecognizerEvents::RecoEvent_Type* pevent, const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result, uint64_t offset = 0);
    std::shared_ptr<ISpxEventArgsFactory> GetEventArgsFactory();
    std::shared_ptr<ISpxSessionEventArgs> CreateSessionEventArgs(const std::wstring& sessionId);
    std::shared_ptr<ISpxConnectionEventArgs> CreateConnectionEventArgs(const std::wstring& sessionId);
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

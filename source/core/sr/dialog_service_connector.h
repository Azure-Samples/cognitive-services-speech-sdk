//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// dialog_service_connector.h: Implementation declarations for CSpxDialogServiceConnector C++ class.
//
#pragma once

#include <atomic>
#include <future>
#include <memory>
#include "asyncop.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include "service_helpers.h"
#include "recognizer.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxDialogServiceConnector :
    public CSpxRecognizer,
    public ISpxDialogServiceConnector,
    public ISpxDialogServiceConnectorEvents
{
public:

    CSpxDialogServiceConnector();
    virtual ~CSpxDialogServiceConnector();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxSessionFromRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizerEvents)
        SPX_INTERFACE_MAP_ENTRY(ISpxDialogServiceConnectorEvents)
        SPX_INTERFACE_MAP_ENTRY(ISpxDialogServiceConnector)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizer)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxGrammarList)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectWithSiteInit
    void Init() override;
    void Term() override;

    // --- ISpxNamedProperties (overrides)
    void SetStringValue(const char* name, const char* value) override;

    // --- ISpxRecognizer

    bool IsEnabled() override { return true; };
    void Enable() override {};
    void Disable()override {};

    // --- ISpxDialogServiceConnector

    CSpxAsyncOp<void> ConnectAsync() override;
    CSpxAsyncOp<void> DisconnectAsync() override;

    CSpxAsyncOp<std::string> SendActivityAsync(std::shared_ptr<ISpxActivity> activity) final;

    CSpxAsyncOp<void> StartContinuousListeningAsync() override;
    CSpxAsyncOp<void> StopContinuousListeningAsync() override;

    CSpxAsyncOp<void> StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model) final;
    CSpxAsyncOp<void> StopKeywordRecognitionAsync() final;

    CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> ListenOnceAsync() override;

    // --- ISpxSessionFromRecognizer

    std::shared_ptr<ISpxSession> GetDefaultSession() override;

    // --- ISpxRecognizerEvents

    void FireSessionStarted(const std::wstring& sessionId) override;
    void FireSessionStopped(const std::wstring& sessionId) override;

    void FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset) override;
    void FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset) override;

    void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) override;

    // --- ISpxDialogServiceConnectorEvents
    void FireActivityReceived(const std::wstring& sessionId, std::shared_ptr<ISpxActivity> activity, std::shared_ptr<ISpxAudioOutput> audio) final;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

protected:

    void EnsureDefaultSession();
    void TermDefaultSession();

    void OnIsEnabledChanged();

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;

    void CheckLogFilename();

private:

    CSpxDialogServiceConnector(const CSpxDialogServiceConnector&) = delete;
    CSpxDialogServiceConnector(const CSpxDialogServiceConnector&&) = delete;

    CSpxDialogServiceConnector& operator=(const CSpxDialogServiceConnector&) = delete;

    std::shared_ptr<ISpxSession> m_defaultSession;
    std::atomic_bool m_fEnabled;

    void SetStringValueInProperties(const char* name, const char* value);
    std::string GetStringValueFromProperties(const char* name, const char* defaultValue);
    void FireRecoEvent(ISpxRecognizerEvents::RecoEvent_Type* pevent, const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result, uint64_t offset = 0);

    void SetRecoMode(const char* modeToSet);
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

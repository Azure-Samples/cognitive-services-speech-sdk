//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_stream_session.h: Implementation declarations for CSpxAudioStreamSession C++ class
//

#pragma once
#include <queue>
#include "spxcore_common.h"
#include "interface_helpers.h"
#include "named_properties_impl.h"
#include "packaged_task_helpers.h"
#include "service_helpers.h"

#include <shared_mutex>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxAudioStreamSession :
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxAudioStreamSessionInit, 
    public ISpxAudioProcessor,
    public ISpxServiceProvider,
    public ISpxSession,
    public ISpxGenericSite,
    public ISpxRecognizerSite,
    public ISpxLuEngineAdapterSite,
    public ISpxKwsEngineAdapterSite,
    public ISpxRecoEngineAdapterSite,
    public ISpxRecoResultFactory,
    public ISpxEventArgsFactory,
    public ISpxNamedPropertiesImpl
{
public:

    CSpxAudioStreamSession();
    virtual ~CSpxAudioStreamSession();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxSession)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecognizerSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxLuEngineAdapterSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxKwsEngineAdapterSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecoEngineAdapterSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecoResultFactory)
        SPX_INTERFACE_MAP_ENTRY(ISpxEventArgsFactory)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamSessionInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit

    void Init() override;
    void Term() override;

    // --- ISpxAudioStreamSessionInit
    
    void InitFromFile(const wchar_t* pszFileName) override;
    void InitFromMicrophone() override;
    void InitFromStream(AudioInputStream* audioInputStream) override;

    // --- ISpxAudioProcessor

    void SetFormat(WAVEFORMATEX* pformat) override;
    void ProcessAudio(AudioData_Type data, uint32_t size) override;

    // --- IServiceProvider ---

    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY_OBJECT(ISpxIntentTriggerService, GetLuEngineAdapter())
    SPX_SERVICE_MAP_ENTRY(ISpxRecoResultFactory)
    SPX_SERVICE_MAP_ENTRY(ISpxEventArgsFactory)
    SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxSession ---

    const std::wstring& GetSessionId() const override;

    void AddRecognizer(std::shared_ptr<ISpxRecognizer> recognizer) override;
    void RemoveRecognizer(ISpxRecognizer* recognizer) override;

    CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> RecognizeAsync() override;
    CSpxAsyncOp<void> StartContinuousRecognitionAsync() override;
    CSpxAsyncOp<void> StopContinuousRecognitionAsync() override;

    CSpxAsyncOp<void> StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model) override;
    CSpxAsyncOp<void> StopKeywordRecognitionAsync() override;


private: 

    DISABLE_COPY_AND_MOVE(CSpxAudioStreamSession);

    enum class RecognitionKind {
        Idle = 0,
        Keyword = 1,
        KwsSingleShot = 2,
        SingleShot = 3,
        Continuous = 4 };

    enum class SessionState {
        Idle = 0,
        WaitForPumpSetFormatStart = 1,
        ProcessingAudio = 2,
        HotSwapPaused = 3,
        StoppingPump = 4,
        WaitForAdapterCompletedSetFormatStop = 5
    };

    CSpxAsyncOp<void> StartRecognitionAsync(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model = nullptr);
    CSpxAsyncOp<void> StopRecognitionAsync(RecognitionKind stopKind);

    void StartRecognizing(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model = nullptr);
    void StopRecognizing(RecognitionKind stopKind);

    std::shared_ptr<ISpxRecognitionResult> WaitForRecognition();
    void WaitForRecognition_Complete(std::shared_ptr<ISpxRecognitionResult> result);

    void FireSessionStartedEvent();
    void FireSessionStoppedEvent();

    void FireSpeechStartDetectedEvent(uint64_t offset);
    void FireSpeechEndDetectedEvent(uint64_t offset);

    void EnsureFireResultEvent();
    void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result);

    enum EventType {SessionStart, SessionStop, SpeechStart, SpeechEnd, RecoResultEvent};
    void FireEvent(EventType sessionType, std::shared_ptr<ISpxRecognitionResult> result = nullptr, wchar_t* sessionId = nullptr, uint64_t offset = 0);


public:

    // --- ISpxKwsEngineAdapterSite
    void KeywordDetected(ISpxKwsEngineAdapter* adapter, uint64_t offset, uint32_t size, AudioData_Type audioData) override;
    void AdapterCompletedSetFormatStop(ISpxKwsEngineAdapter* /* adapter */) override { AdapterCompletedSetFormatStop(AdapterDoneProcessingAudio::Keyword); }

    // --- ISpxRecoEngineAdapterSite (first part...)
    void GetScenarioCount(uint16_t* countSpeech, uint16_t* countIntent, uint16_t* countTranslation) override;

    std::list<std::string> GetListenForList() override;
    void GetIntentInfo(std::string& provider, std::string& id, std::string& key, std::string& region) override;

    void AdapterStartingTurn(ISpxRecoEngineAdapter* adapter) override;
    void AdapterStartedTurn(ISpxRecoEngineAdapter* adapter, const std::string& id) override;
    void AdapterStoppedTurn(ISpxRecoEngineAdapter* adapter) override;
    void AdapterDetectedSpeechStart(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;
    void AdapterDetectedSpeechEnd(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;
    void AdapterDetectedSoundStart(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;
    void AdapterDetectedSoundEnd(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;

    // -- ISpxEventArgsFactory
    std::shared_ptr<ISpxSessionEventArgs> CreateSessionEventArgs(const std::wstring& sessionId) override;
    std::shared_ptr<ISpxRecognitionEventArgs> CreateRecognitionEventArgs(const std::wstring& sessionId, uint64_t offset) override;
    std::shared_ptr<ISpxRecognitionEventArgs> CreateRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) override;

    // --- ISpxRecoResultFactory
    std::shared_ptr<ISpxRecognitionResult> CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type, uint64_t offset, uint64_t duration) override;
    std::shared_ptr<ISpxRecognitionResult> CreateFinalResult(ResultType type, const wchar_t* resultId, Reason reason, const wchar_t* text, uint64_t offset, uint64_t duration) override;
    std::shared_ptr<ISpxRecognitionResult> CreateErrorResult(const wchar_t* text, ResultType type) override;

    // --- ISpxRecoEngineAdapterSite (second part...)
    void FireAdapterResult_Intermediate(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result) override;
    void FireAdapterResult_FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result) override;
    void FireAdapterResult_TranslationSynthesis(ISpxRecoEngineAdapter* adapter, std::shared_ptr<ISpxRecognitionResult> result) override;

    void AdapterCompletedSetFormatStop(ISpxRecoEngineAdapter* /* adapter */) override { AdapterCompletedSetFormatStop(AdapterDoneProcessingAudio::Speech); }
    void AdapterRequestingAudioMute(ISpxRecoEngineAdapter* adapter, bool muteAudio) override;

    void AdditionalMessage(ISpxRecoEngineAdapter* adapter, uint64_t offset, AdditionalMessagePayload_Type payload) override;
    void Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload) override;

    // --- ISpxRecognizerSite
    std::shared_ptr<ISpxSession> GetDefaultSession() override;

    // --- ISpxNamedProperties (overrides)
    std::wstring GetStringValue(const wchar_t* name, const wchar_t* defaultValue) const override;


private:

    std::shared_ptr<ISpxRecoEngineAdapter> EnsureInitRecoEngineAdapter();
    void EnsureResetRecoEngineAdapter();
    void InitRecoEngineAdapter();

    void StartResetEngineAdapter();
    void EnsureResetEngineEngineAdapterComplete();

    void EnsureIntentRegionSet();
    std::string SpeechRegionFromIntentRegion(const std::string& intentRegion);

    std::shared_ptr<ISpxKwsEngineAdapter> EnsureInitKwsEngineAdapter(std::shared_ptr<ISpxKwsModel> model);
    void InitKwsEngineAdapter(std::shared_ptr<ISpxKwsModel> model);

    void ProcessAudioDataNow(AudioData_Type data, uint32_t size);

    void ProcessAudioDataLater(AudioData_Type audio, uint32_t size);
    void ProcessAudioDataLater_Complete();
    void ProcessAudioDataLater_Overflow();
    void ProcessAudioDataLater_Clear();

    void HotSwapToKwsSingleShotWhilePaused();
    void WaitForKwsSingleShotRecognition();

    void StartAudioPump(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model);
    void HotSwapAdaptersWhilePaused(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model = nullptr);

    void InformAdapterSetFormatStarting(WAVEFORMATEX* format);
    void InformAdapterSetFormatStopping(SessionState comingFromState);

    enum AdapterDoneProcessingAudio { Keyword, Speech };
    void AdapterCompletedSetFormatStop(AdapterDoneProcessingAudio doneAdapter);

    bool IsKind(RecognitionKind kind);
    bool IsState(SessionState state);
    bool ChangeState(SessionState sessionStateFrom, SessionState sessionStateTo);
    bool ChangeState(SessionState sessionStateFrom, RecognitionKind recoKindTo, SessionState sessionStateTo);
    bool ChangeState(RecognitionKind recoKindFrom, SessionState sessionStateFrom, RecognitionKind recoKindTo, SessionState sessionStateTo);

    void EnsureInitLuEngineAdapter();
    void InitLuEngineAdapter();

    std::list<std::string> GetListenForListFromLuEngineAdapter();
    void GetIntentInfoFromLuEngineAdapter(std::string& provider, std::string& id, std::string& key, std::string& region);

    std::shared_ptr<ISpxLuEngineAdapter> GetLuEngineAdapter();
    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;

    void WaitForIdle();


private:
    // Unique identifier of the session, used mostly for diagnostics.
    // Is represented by UUID without dashes.
    const std::wstring m_sessionId;

    #ifdef _MSC_VER
    using ReadWriteMutex_Type = std::shared_mutex;
    using WriteLock_Type = std::unique_lock<std::shared_mutex>;
    using ReadLock_Type = std::shared_lock<std::shared_mutex>;
    #else
    using ReadWriteMutex_Type = std::shared_timed_mutex;
    using WriteLock_Type = std::unique_lock<std::shared_timed_mutex>;
    using ReadLock_Type = std::shared_lock<std::shared_timed_mutex>;
    #endif

    //  To orchestrate the conversion of "Audio Data" into "Results" and "Events", we'll use utilize
    //  one "Audio Pump" and multiple "Adapters"

    SpxWAVEFORMATEX_Type m_format;
    std::shared_ptr<ISpxAudioPump> m_audioPump;
    
    std::shared_ptr<ISpxKwsEngineAdapter> m_kwsAdapter;
    std::shared_ptr<ISpxKwsModel> m_kwsModel;

    std::shared_ptr<ISpxRecoEngineAdapter> m_recoAdapter;
    std::shared_ptr<ISpxRecoEngineAdapter> m_resetRecoAdapter;

    std::shared_ptr<ISpxLuEngineAdapter> m_luAdapter;

    //  Our current "state" is kept in two parts, both protected by a reader/writer lock
    //
    //      1.) RecognitionKind (m_recoKind): Keeps track of what kind of recognition we're doing
    //      2.) SessionState (m_sessionState): Keeps track of what we're doing with Audio data
    //
    ReadWriteMutex_Type m_stateMutex;
    RecognitionKind m_recoKind;
    SessionState m_sessionState;

    bool m_expectAdapterStartedTurn;
    bool m_expectAdapterStoppedTurn;
    bool m_adapterAudioMuted;
    RecognitionKind m_turnEndStopKind;


    //  When we're in the SessionState::ProcessingAudio, we'll relay "Audio Data" to from the Pump
    //  to exactly one (and only one) of the engine adapters via it's ISpxAudioProcessor interface
    //  
    //  Using or changing the Adapter (as ISpxAudioProcessor) requires locking/unlocking the reader writer lock
    //
    ReadWriteMutex_Type m_combinedAdapterAndStateMutex;
    std::shared_ptr<ISpxAudioProcessor> m_audioProcessor;

    // Other member data ...

    const int m_maxMsStashedBeforeSimulateRealtime = 500;
    const int m_simulateRealtimePercentage = 50;
    const int m_maxMsStashedBeforeOverflow = 5000;

    std::mutex m_processAudioLaterMutex;
    std::queue<std::pair<AudioData_Type, uint32_t>> m_processAudioLater;
    uint64_t m_sizeProcessAudioLater;

    std::mutex m_mutex;
    std::condition_variable m_cv;

    // 1 minute as timeout value for RecognizeAsync(), which should be sufficient even for translation in conversation mode.
    // Note using std::chrono::minutes::max() could cause wait_for to exit straight away instead of 
    // infinite timeout, because wait_for() in VS is implemented via wait_until() and a possible integer
    // overflow could make new time < now.
    const std::chrono::minutes m_recoAsyncTimeoutDuration = std::chrono::minutes(1);
    const std::chrono::seconds m_waitForAdapterCompletedSetFormatStopTimeout = std::chrono::seconds(20);
    const std::chrono::milliseconds m_shutdownTimeout = std::chrono::milliseconds(500);

    bool m_recoAsyncWaiting;
    std::shared_ptr<ISpxRecognitionResult> m_recoAsyncResult;

    CSpxPackagedTaskHelper m_taskHelper;
    std::list<std::weak_ptr<ISpxRecognizer>> m_recognizers;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

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

#ifdef _MSC_VER
#include <shared_mutex>
#endif // _MSC_VER


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
    std::list<std::string> GetListenForList() override;
    void GetIntentInfo(std::string& provider, std::string& id, std::string& key) override;

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
    std::shared_ptr<ISpxRecognitionResult> CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type = ResultType::Speech) override;
    std::shared_ptr<ISpxRecognitionResult> CreateFinalResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type = ResultType::Speech) override;
    std::shared_ptr<ISpxRecognitionResult> CreateNoMatchResult(enum ResultType type = ResultType::Speech) override;
    std::shared_ptr<ISpxRecognitionResult> CreateErrorResult(const wchar_t* text, ResultType type = ResultType::Speech) override;

    // --- ISpxRecoEngineAdapterSite (second part...)
    void FireAdapterResult_Intermediate(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result) override;
    void FireAdapterResult_FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result) override;
    void FireAdapterResult_TranslationSynthesis(ISpxRecoEngineAdapter* adapter, std::shared_ptr<ISpxRecognitionResult> result) override;

    void AdapterCompletedSetFormatStop(ISpxRecoEngineAdapter* /* adapter */) override { AdapterCompletedSetFormatStop(AdapterDoneProcessingAudio::Speech); }
    void AdapterRequestingAudioIdle(ISpxRecoEngineAdapter* adapter) override;

    void AdditionalMessage(ISpxRecoEngineAdapter* adapter, uint64_t offset, AdditionalMessagePayload_Type payload) override;
    void Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload) override;

    // --- ISpxRecognizerSite
    std::shared_ptr<ISpxSession> GetDefaultSession() override;

    // --- ISpxNamedProperties (overrides)
    std::wstring GetStringValue(const wchar_t* name, const wchar_t* defaultValue) override;


private:

    std::shared_ptr<ISpxRecoEngineAdapter> EnsureInitRecoEngineAdapter();
    void InitRecoEngineAdapter();

    std::shared_ptr<ISpxKwsEngineAdapter> EnsureInitKwsEngineAdapter(std::shared_ptr<ISpxKwsModel> model);
    void InitKwsEngineAdapter(std::shared_ptr<ISpxKwsModel> model);

    void ProcessAudioDataNow(AudioData_Type data, uint32_t size);

    void ProcessAudioDataLater(AudioData_Type audio, uint32_t size);
    void ProcessAudioDataLater_Complete();
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
    void GetIntentInfoFromLuEngineAdapter(std::string& provider, std::string& id, std::string& key);

    std::shared_ptr<ISpxLuEngineAdapter> GetLuEngineAdapter();
    std::shared_ptr<ISpxNamedProperties> GetParentProperties() override;


private:

    const std::wstring m_sessionId;

    #ifdef _MSC_VER
    using ReadWriteMutex_Type = std::shared_mutex;
    using WriteLock_Type = std::unique_lock<std::shared_mutex>;
    using ReadLock_Type = std::shared_lock<std::shared_mutex>;
    #else
    using ReadWriteMutex_Type = std::mutex;
    using WriteLock_Type = std::unique_lock<std::mutex>;
    using ReadLock_Type = std::unique_lock<std::mutex>;
    #endif

    //  To orchestrate the conversion of "Audio Data" into "Results" and "Events", we'll use utilize
    //  one "Audio Pump" and multiple "Adapters"

    SpxWAVEFORMATEX_Type m_format;
    std::shared_ptr<ISpxAudioPump> m_audioPump;
    
    std::shared_ptr<ISpxKwsEngineAdapter> m_kwsAdapter;
    std::shared_ptr<ISpxKwsModel> m_kwsModel;

    std::shared_ptr<ISpxRecoEngineAdapter> m_recoAdapter;
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
    bool m_adapterRequestedIdle;
    RecognitionKind m_turnEndStopKind;


    //  When we're in the SessionState::ProcessingAudio, we'll relay "Audio Data" to from the Pump
    //  to exactly one (and only one) of the engine adapters via it's ISpxAudioProcessor interface
    //  
    //  Using or changing the Adapter (as ISpxAudioProcessor) requires locking/unlocking the reader writer lock
    //
    ReadWriteMutex_Type m_combinedAdapterAndStateMutex;
    std::shared_ptr<ISpxAudioProcessor> m_audioProcessor;

    // Other member data ...

    const int m_maxMsStashedBeforeSimulateRealtime = 300;

    std::mutex m_processAudioLaterMutex;
    std::queue<std::pair<AudioData_Type, uint32_t>> m_processAudioLater;
    uint64_t m_sizeProcessAudioLater;

    std::mutex m_mutex;
    std::condition_variable m_cv;

    const int m_recoAsyncTimeout = 10;
    const int m_waitForAdatperCompletedSetFormatStopTimeout = 20;
    const int m_shutdownTimeoutInMs = 500;

    bool m_recoAsyncWaiting;
    std::shared_ptr<ISpxRecognitionResult> m_recoAsyncResult;

    CSpxPackagedTaskHelper m_taskHelper;
    std::list<std::weak_ptr<ISpxRecognizer>> m_recognizers;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl

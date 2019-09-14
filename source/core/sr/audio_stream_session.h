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
#include "property_bag_impl.h"
#include "packaged_task_helpers.h"
#include "service_helpers.h"
#include "audio_buffer.h"
#include "thread_service.h"

#include <shared_mutex>
#include <unordered_map>

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
    public ISpxAudioPumpSite,
    public ISpxRecoEngineAdapterSite,
    public ISpxRecoResultFactory,
    public ISpxEventArgsFactory,
    public ISpxPropertyBagImpl,
    public ISpxInteractionIdProvider,
    public ISpxSpeechEventPayloadProvider,
    public ISpxGetUspMessageParamsFromUser
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
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioPumpSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecoEngineAdapterSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecoResultFactory)
        SPX_INTERFACE_MAP_ENTRY(ISpxEventArgsFactory)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioStreamSessionInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxAudioProcessor)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxInteractionIdProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxSpeechEventPayloadProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxGetUspMessageParamsFromUser)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit

    void Init() override;
    void Term() override;

    // --- ISpxAudioStreamSessionInit

    void InitFromFile(const wchar_t* pszFileName) override;
    void InitFromMicrophone() override;
    void InitFromStream(std::shared_ptr<ISpxAudioStream> stream) override;

    // --- ISpxAudioProcessor

    void SetFormat(const SPXWAVEFORMATEX* pformat) override;
    void ProcessAudio(const DataChunkPtr& audioChunk) override;

    // --- IServiceProvider ---

    SPX_SERVICE_MAP_BEGIN()
    SPX_SERVICE_MAP_ENTRY_OBJECT(ISpxIntentTriggerService, GetLuEngineAdapter())
    SPX_SERVICE_MAP_ENTRY(ISpxRecoResultFactory)
    SPX_SERVICE_MAP_ENTRY(ISpxEventArgsFactory)
    SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
    SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_ENTRY_FUNC(InternalQueryService)
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

    CSpxAsyncOp<std::string> SendActivityAsync(std::string activity) final;

    void OpenConnection(bool forContinuousRecognition) override;
    void CloseConnection() override;
    void WriteTelemetryLatency(uint64_t latencyInTicks, bool isPhraseLatency) override;

    void SendSpeechEventMessage(std::string&& payload) override;
    void SendNetworkMessage(std::string&& path, std::string&& payload) override;

    bool IsStreaming() override;

    // --- ISpxKwsEngineAdapterSite
    void KeywordDetected(ISpxKwsEngineAdapter* adapter, uint64_t offset, uint64_t duration, double confidence, const std::string& keyword, const DataChunkPtr& audioChunk) override;
    void AdapterCompletedSetFormatStop(ISpxKwsEngineAdapter* /* adapter */) override { AdapterCompletedSetFormatStop(AdapterDoneProcessingAudio::Keyword); }

    // --- ISpxRecoEngineAdapterSite (first part...)
    void GetScenarioCount(uint16_t* countSpeech, uint16_t* countIntent, uint16_t* countTranslation, uint16_t* countDialog, uint16_t* countTranscriber) override;

    std::list<std::string> GetListenForList() override;
    void GetIntentInfo(std::string& provider, std::string& id, std::string& key, std::string& region) override;
    std::shared_ptr<ISpxRecognitionResult> GetSpottedKeywordResult() override;

    void AdapterStartingTurn(ISpxRecoEngineAdapter* adapter) override;
    void AdapterStartedTurn(ISpxRecoEngineAdapter* adapter, const std::string& id) override;
    void AdapterStoppedTurn(ISpxRecoEngineAdapter* adapter) override;
    void AdapterDetectedSpeechStart(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;
    void AdapterDetectedSpeechEnd(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;
    void AdapterDetectedSoundStart(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;
    void AdapterDetectedSoundEnd(ISpxRecoEngineAdapter* adapter, uint64_t offset) override;
    void AdapterEndOfDictation(ISpxRecoEngineAdapter* adapter, uint64_t offset, uint64_t duration) override;

    // -- ISpxEventArgsFactory
    std::shared_ptr<ISpxSessionEventArgs> CreateSessionEventArgs(const std::wstring& sessionId) override;
    std::shared_ptr<ISpxConnectionEventArgs> CreateConnectionEventArgs(const std::wstring& sessionId) override;
    std::shared_ptr<ISpxRecognitionEventArgs> CreateRecognitionEventArgs(const std::wstring& sessionId, uint64_t offset) override;
    std::shared_ptr<ISpxRecognitionEventArgs> CreateRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) override;
    std::shared_ptr<ISpxActivityEventArgs> CreateActivityEventArgs(std::string activity, std::shared_ptr<ISpxAudioOutput> audio) final;

    // --- ISpxRecoResultFactory
    std::shared_ptr<ISpxRecognitionResult> CreateIntermediateResult(const wchar_t* resultId, const wchar_t* text, uint64_t offset, uint64_t duration) override;
    std::shared_ptr<ISpxRecognitionResult> CreateFinalResult(const wchar_t* resultId, ResultReason reason, NoMatchReason noMatchReason, CancellationReason cancellation, CancellationErrorCode errorCode, const wchar_t* text, uint64_t offset, uint64_t duration) override;
    std::shared_ptr<ISpxRecognitionResult> CreateKeywordResult(const double confidence, const uint64_t offset, const uint64_t duration, const wchar_t* keyword, ResultReason reason) override;

    // --- ISpxRecoEngineAdapterSite (second part...)
    void FireAdapterResult_Intermediate(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result) override;
    void FireAdapterResult_KeywordResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result, bool isAccepted) override;
    void FireAdapterResult_FinalResult(ISpxRecoEngineAdapter* adapter, uint64_t offset, std::shared_ptr<ISpxRecognitionResult> result) override;
    void FireAdapterResult_ActivityReceived(ISpxRecoEngineAdapter* adapter, std::string activity, std::shared_ptr<ISpxAudioOutput> audio) final;
    void FireAdapterResult_TranslationSynthesis(ISpxRecoEngineAdapter* adapter, std::shared_ptr<ISpxRecognitionResult> result) override;
    void FireConnectedEvent() override;
    void FireDisconnectedEvent() override;

    void AdapterCompletedSetFormatStop(ISpxRecoEngineAdapter* /* adapter */) override { AdapterCompletedSetFormatStop(AdapterDoneProcessingAudio::Speech); }
    void AdapterRequestingAudioMute(ISpxRecoEngineAdapter* adapter, bool muteAudio) override;

    void AdditionalMessage(ISpxRecoEngineAdapter* adapter, uint64_t offset, AdditionalMessagePayload_Type payload) override;
    void Error(ISpxRecoEngineAdapter* adapter, ErrorPayload_Type payload) override;

    // --- ISpxAudioPumpSite
    void Error(const std::string& error) override;

    // --- ISpxRecognizerSite
    std::shared_ptr<ISpxSession> GetDefaultSession() override;

    // --- ISpxNamedProperties (overrides)
    std::string GetStringValue(const char* name, const char* defaultValue) const override;
    void SetStringValue(const char* name, const char* value) override;

    // --- ISpxInteractionIdProvider
    std::string PeekNextInteractionId(InteractionIdPurpose purpose) final;
    std::string GetInteractionId(InteractionIdPurpose purpose) final;

    // --- ISpxSpeechEventPayloadProvider
    std::string GetSpeechEventPayload(bool startMeeting) override;

    // --- ISpxGetUspMessageParamsFromUser
    CSpxStringMap GetParametersFromUser(std::string&& path) override;

private:
    std::shared_ptr<ISpxThreadService> InternalQueryService(const char* serviceName);

    void CheckError(const std::string& error);

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
        WaitForAdapterCompletedSetFormatStop = 5,
        ProcessingAudioLeftovers = 6
    };

    template<class ISpx_Recognizer_Type>
    bool IsRecognizerType() const noexcept;

    CSpxAsyncOp<void> StartRecognitionAsync(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model = nullptr);
    CSpxAsyncOp<void> StopRecognitionAsync(RecognitionKind stopKind);

    void StartRecognizing(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model = nullptr);
    void StopRecognizing(RecognitionKind stopKind);

    void WaitForRecognition_Complete(std::shared_ptr<ISpxRecognitionResult> result);

    void FireSessionStartedEvent();
    void FireSessionStoppedEvent();

    void FireSpeechStartDetectedEvent(uint64_t offset);
    void FireSpeechEndDetectedEvent(uint64_t offset);

    void EnsureFireResultEvent();
    void FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result);

    enum EventType { SessionStart, SessionStop, SpeechStart, SpeechEnd, RecoResultEvent, ActivityReceivedEvent, Connected, Disconnected };
    void FireEvent(EventType sessionType, std::shared_ptr<ISpxRecognitionResult> result = nullptr, const wchar_t* sessionId = nullptr, uint64_t offset = 0, std::string activity = std::string{}, std::shared_ptr<ISpxAudioOutput> audio = nullptr);

private:
    std::packaged_task<void()> CreateTask(std::function<void()> func, bool catchAll = true);
    std::shared_ptr<ISpxRecoEngineAdapter> EnsureInitRecoEngineAdapter();
    void InitRecoEngineAdapter();

    void StartResetEngineAdapter();
    void EnsureResetEngineEngineAdapterComplete();

    void EnsureIntentRegionSet();
    std::string SpeechRegionFromIntentRegion(const std::string& intentRegion);

    std::shared_ptr<ISpxKwsEngineAdapter> EnsureInitKwsEngineAdapter(std::shared_ptr<ISpxKwsModel> model);
    void InitKwsEngineAdapter(std::shared_ptr<ISpxKwsModel> model);

    bool ProcessNextAudio();

    void HotSwapToKwsSingleShotWhilePaused(std::shared_ptr<ISpxRecognitionResult> spottedKeywordResult);

    void StartAudioPump(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model);
    void HotSwapAdaptersWhilePaused(RecognitionKind startKind, std::shared_ptr<ISpxKwsModel> model = nullptr);

    void InformAdapterSetFormatStarting(const SPXWAVEFORMATEX* format);
    void InformAdapterSetFormatStopping(SessionState comingFromState);
    void EncounteredEndOfStream();

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

    void WaitForIdle(std::chrono::milliseconds timeout);
    void Ensure16kHzSampleRate();
    void CancelPendingSingleShot();
    void SlowDownThreadIfNecessary(uint32_t size);
    void DispatchEvent(const std::list<std::weak_ptr<ISpxRecognizer>>& weakRecognizers,
                       const std::wstring& sessionId, EventType sessionType, uint64_t offset,
                       std::shared_ptr<ISpxRecognitionResult> result,
                       std::string activity, std::shared_ptr<ISpxAudioOutput> audio);

    struct Operation;
    void RecognizeOnceAsync(const std::shared_ptr<Operation>& singleShot);

    void SetAudioConfigurationInProperties();
    void WriteTracingEvent();
    uint64_t GetResultLatencyInMs(const ProcessedAudioTimestampPtr& audiotimestamp) const;

    void SetThrottleVariables(const SPXWAVEFORMATEX* format);

    void UpdateAdapterResult_JsonResult(std::shared_ptr<ISpxRecognitionResult> result);

private:

    std::shared_ptr<ISpxGenericSite> m_siteKeepAlive;

    // Unique identifier of the session, used mostly for diagnostics.
    // Is represented by UUID without dashes.
    const std::wstring m_sessionId;

    using milliseconds = std::chrono::milliseconds;
    using seconds = std::chrono::seconds;
    using minutes = std::chrono::minutes;

    //  To orchestrate the conversion of "Audio Data" into "Results" and "Events", we'll use utilize
    //  one "Audio Pump" and multiple "Adapters"

    SpxWAVEFORMATEX_Type m_format;
    std::mutex m_formatMutex;
    std::shared_ptr<ISpxAudioPump> m_audioPump;

    std::shared_ptr<ISpxKwsEngineAdapter> m_kwsAdapter;
    std::shared_ptr<ISpxKwsModel> m_kwsModel;

    std::shared_ptr<ISpxRecoEngineAdapter> m_recoAdapter;
    std::shared_ptr<ISpxRecoEngineAdapter> m_resetRecoAdapter;

    std::shared_ptr<ISpxLuEngineAdapter> m_luAdapter;
    std::shared_ptr<ISpxAudioStreamReader> m_codecAdapter;

    // Our current "state" is kept in two parts and can only be changed from the background thread.
    //      1.) RecognitionKind (m_recoKind): Keeps track of what kind of recognition we're doing
    //      2.) SessionState (m_sessionState): Keeps track of what we're doing with Audio data
    // We still currently notify about the state change using the below conditional variable.
    // (TODO: this will be removed, but currently RecognizeAsync has to wait till the session
    // is in a clear state in order the next RecognizeAsync to succeeed.)
    std::mutex m_stateMutex;
    std::condition_variable m_cv;

    RecognitionKind m_recoKind;
    SessionState m_sessionState;

    bool m_sawEndOfStream;      // Flag indicating that we have processed all data and got response from the service.
    bool m_fireEndOfStreamAtSessionStop;

    bool m_expectAdapterStartedTurn;
    bool m_expectAdapterStoppedTurn;
    bool m_expectFirstHypothesis;
    bool m_adapterAudioMuted;
    bool m_audioPumpStoppedBeforeHotSwap;
    RecognitionKind m_turnEndStopKind;

    // In order to reliably deliver audio, we always swap audio processor
    // together with its audio buffer. Otherwise data can be processed by a stale processor
    std::shared_ptr<ISpxAudioProcessor> m_audioProcessor;
    bool m_isKwsProcessor;
    AudioBufferPtr m_audioBuffer;
    DataChunkPtr m_spottedKeyword;

    // We replay after the last successful result. Richland currently has the upper bound
    // of 30 seconds to generate a speech segment. To be on the safe side, similar to the old SDK we buffer for 1 minute.
    seconds m_maxBufferedBeforeOverflow = seconds(60);
    milliseconds m_maxTransmittedInFastLane = milliseconds(5000);
    constexpr static milliseconds MaxBufferedBeforeSimulateRealtime = milliseconds(500);
    constexpr static int SimulateRealtimePercentage = 50;
    uint64_t m_maxFastLaneSizeBytes = 16000 * 2 * 5;
    uint64_t m_maxBufferedSizeBeforeThrottleBytes = 16000;
    uint32_t m_avgBytesPerSecond = 16000 * 2;

    // The minimum time that before the next audio frame should be processed.
    // This time is calculated as each audio frame is processed, and the processing
    // of the next frame will delay until this time is reached.
    std::chrono::steady_clock::time_point m_nextAudioProcessTime = std::chrono::steady_clock::now();

    // In the event the system's steady_clock lacks the precision to throttle properly, fall back
    // to duration based where we will wait a % of the audio packet length.
    bool m_useDurationBasedThrottle = false;

    static seconds StopRecognitionTimeout;

    std::list<std::weak_ptr<ISpxRecognizer>> m_recognizers;
    mutable std::mutex m_recognizersLock;

    bool m_isReliableDelivery;
    uint64_t m_lastErrorGlobalOffset;
    uint64_t m_currentTurnGlobalOffset;

    uint64_t m_bytesTransited;

    std::shared_ptr<CSpxThreadService> m_threadService;

    struct Operation
    {
        // The below static constant is set to 1 minute as timeout value for
        // RecognizeAsync()/SingleShot for KWS,
        // which should be sufficient even for translation in conversation mode.
        // Note using std::chrono::minutes::max() could cause wait_for to exit straight away instead of
        // infinite timeout, because wait_for() in VS is implemented via wait_until() and a possible integer
        // overflow could make new time < now.
        const static minutes Timeout;
        static std::atomic<int64_t> OperationId;

        explicit Operation(RecognitionKind kind) : m_operationId{ OperationId++ }, m_kind{ kind }
        {
            m_future = std::shared_future<std::shared_ptr<ISpxRecognitionResult>>(m_promise.get_future());
        }

        const int64_t m_operationId;
        const RecognitionKind m_kind;
        std::promise<std::shared_ptr<ISpxRecognitionResult>> m_promise;
        std::shared_future<std::shared_ptr<ISpxRecognitionResult>> m_future;

        // Details about the spotted keyword to verify.
        std::shared_ptr<ISpxRecognitionResult> m_spottedKeywordResult;
    };

    // Single shot in flight operation.
    std::shared_ptr<Operation> m_singleShotInFlight;

    struct InteractionId
    {
        inline InteractionId(std::string nextSpeech, std::string nextActivity):
            m_nextSpeech{nextSpeech}, m_nextActivity{nextActivity}
        {}

        std::string m_nextSpeech;
        std::string m_nextActivity;
        std::mutex m_lock;
    };

    InteractionId m_interactionId;

};


} } } } // Microsoft::CognitiveServices::Speech::Impl

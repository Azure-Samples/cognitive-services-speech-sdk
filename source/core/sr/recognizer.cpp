#include "stdafx.h"
#include "recognizer.h"
#include <future>
#include <sstream>
#include "site_helpers.h"
#include "service_helpers.h"
#include "create_object_helpers.h"
#include "property_id_2_name_map.h"
#include "file_logger.h"

#include "iostream"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

constexpr auto NOT_SET_VALUE = "!!<<NOT SET>!!";

CSpxRecognizer::CSpxRecognizer() :
    ISpxRecognizerEvents(nullptr, nullptr),
    m_fEnabled(true)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxRecognizer::~CSpxRecognizer()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    TermDefaultSession();
}

void CSpxRecognizer::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    EnsureDefaultSession();
    CheckLogFilename();
}

void CSpxRecognizer::Term()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxRecognizer::SetStringValue(const char* name, const char* value)
{
    SetStringValueInProperties(name, value);
}

std::shared_ptr<ISpxGrammar> CSpxRecognizer::GetPhraseListGrammar(const wchar_t* name)
{
    SPX_IFFALSE_THROW_HR(name == nullptr || name[0] == L'\0', SPXERR_INVALID_ARG); // only support one phrase list at this point
    return SpxQueryInterface<ISpxGrammar>(EnsureDefaultPhraseListGrammar());
}

void CSpxRecognizer::AddGrammar(std::shared_ptr<ISpxGrammar> grammar)
{
    SPX_IFFALSE_THROW_HR(grammar != nullptr, SPXERR_INVALID_ARG);

    m_grammarlist.push_back(grammar);
}

std::list<std::string> CSpxRecognizer::GetListenForList()
{
    auto retVal = std::list<std::string>();

    for (auto& grammar : m_grammarlist)
    {
        auto grammarListenFor = grammar->GetListenForList();
        retVal.insert(retVal.end(), grammarListenFor.begin(), grammarListenFor.end());
    }

    return retVal;
}

bool CSpxRecognizer::IsEnabled()
{
    return m_fEnabled;
}

void CSpxRecognizer::Enable()
{
    if (m_fEnabled.exchange(true) != true)
    {
        OnIsEnabledChanged();
    }
}

void CSpxRecognizer::Disable()
{
    if (m_fEnabled.exchange(false) != false)
    {
        OnIsEnabledChanged();
    }
}

void CSpxRecognizer::OpenConnection(bool forContinuousRecognition)
{
    m_defaultSession->OpenConnection(forContinuousRecognition);
}

void CSpxRecognizer::CloseConnection()
{
    m_defaultSession->CloseConnection();
}

CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult>> CSpxRecognizer::RecognizeAsync()
{
    const char* recoModePropertyName = GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode);
    auto currentRecoMode =  GetStringValueFromProperties(recoModePropertyName, "");
    auto recoModeToSet = dynamic_cast<ISpxTranslationRecognizer *>(this) != nullptr
        ? g_recoModeConversation
        : g_recoModeInteractive;

    if (currentRecoMode.empty())
    {
        SetStringValueInProperties(recoModePropertyName, recoModeToSet);
    }
    else
    {
        // If the reco mode is set to dictation (which can only be set before starting any recognition), just use it.
        // But switching between interactive and conversation after connection setup is not allowed.
        SPX_IFTRUE_THROW_HR((currentRecoMode.compare(g_recoModeDictation) != 0 && currentRecoMode.compare(recoModeToSet)) != 0, SPXERR_SWITCH_MODE_NOT_ALLOWED);
    }

    return m_defaultSession->RecognizeAsync();
}

CSpxAsyncOp<void> CSpxRecognizer::StartContinuousRecognitionAsync()
{
    const char* recoModePropertyName = GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode);
    auto currentRecoMode = GetStringValueFromProperties(recoModePropertyName, "");
    auto recoModeToSet = dynamic_cast<ISpxIntentRecognizer *>(this) != nullptr
        ? g_recoModeInteractive
        : g_recoModeConversation;
    if (currentRecoMode.empty())
    {
        SetStringValueInProperties(recoModePropertyName, recoModeToSet);
    }
    else
    {
        // If the reco mode is set to dictation (which can only be set before starting any recognition), just use it.
        // But switching between interactive and conversation after connection setup is not allowed.
        SPX_IFTRUE_THROW_HR((currentRecoMode.compare(g_recoModeDictation) != 0 && currentRecoMode.compare(recoModeToSet) != 0), SPXERR_SWITCH_MODE_NOT_ALLOWED);;
    }
    return m_defaultSession->StartContinuousRecognitionAsync();
}

CSpxAsyncOp<void> CSpxRecognizer::StopContinuousRecognitionAsync()
{
    if (m_defaultSession)
    {
       return m_defaultSession->StopContinuousRecognitionAsync();
    }
    else
    {
        // in some error cases, m_defaultSession is empty. Return AOS_Error.
        std::promise<void> void_promise;
        void_promise.set_exception(std::make_exception_ptr(std::runtime_error("The default session is a nullptr.")));
        std::shared_future<void> void_future = void_promise.get_future().share();
        return CSpxAsyncOp<void>(void_future, AOS_Error);
    }
}

CSpxAsyncOp<void> CSpxRecognizer::StartKeywordRecognitionAsync(std::shared_ptr<ISpxKwsModel> model)
{
    const char* recoModePropertyName = GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode);
    auto currentRecoMode = GetStringValueFromProperties(recoModePropertyName, "");

    // currently, kws uses recoModeInteractive as default, but takes the passed mode, if configured
    if (currentRecoMode.empty())
    {
        SetStringValueInProperties(recoModePropertyName, g_recoModeInteractive);
    }
    return m_defaultSession->StartKeywordRecognitionAsync(model);
}

CSpxAsyncOp<void> CSpxRecognizer::StopKeywordRecognitionAsync()
{
    return m_defaultSession->StopKeywordRecognitionAsync();
}

std::shared_ptr<ISpxSession> CSpxRecognizer::GetDefaultSession()
{
    EnsureDefaultSession();
    return m_defaultSession;
}

std::shared_ptr<ISpxEventArgsFactory> CSpxRecognizer::GetEventArgsFactory()
{
    SPX_DBG_ASSERT(GetSite());
    return SpxQueryService<ISpxEventArgsFactory>(GetSite());
}

std::shared_ptr<ISpxSessionEventArgs> CSpxRecognizer::CreateSessionEventArgs(const std::wstring& sessionId)
{
    auto factory = GetEventArgsFactory();
    return factory->CreateSessionEventArgs(sessionId);
}

std::shared_ptr<ISpxConnectionEventArgs> CSpxRecognizer::CreateConnectionEventArgs(const std::wstring& sessionId)
{
    auto factory = GetEventArgsFactory();
    return factory->CreateConnectionEventArgs(sessionId);
}

void CSpxRecognizer::FireSessionStarted(const std::wstring& sessionId)
{
    auto sessionEvent = CreateSessionEventArgs(sessionId);
    SessionStarted.Signal(sessionEvent);
}

void CSpxRecognizer::FireSessionStopped(const std::wstring& sessionId)
{
    auto sessionEvent = CreateSessionEventArgs(sessionId);
    SessionStopped.Signal(sessionEvent);
}

void CSpxRecognizer::FireConnected(const std::wstring& sessionId)
{
    auto connectionEvent = CreateConnectionEventArgs(sessionId);
    Connected.Signal(connectionEvent);
}

void CSpxRecognizer::FireDisconnected(const std::wstring& sessionId)
{
    auto connectionEvent = CreateConnectionEventArgs(sessionId);
    Disconnected.Signal(connectionEvent);
}

void CSpxRecognizer::FireSpeechStartDetected(const std::wstring& sessionId, uint64_t offset)
{
    FireRecoEvent(&SpeechStartDetected, sessionId, nullptr, offset);
}

void CSpxRecognizer::FireSpeechEndDetected(const std::wstring& sessionId, uint64_t offset)
{
    FireRecoEvent(&SpeechEndDetected, sessionId, nullptr, offset);
}

void CSpxRecognizer::FireResultEvent(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    ISpxRecognizerEvents::RecoEvent_Type* pevent = nullptr;
    auto reason = result->GetReason();
    switch (reason)
    {
    case ResultReason::Canceled:
        pevent = &Canceled;
        break;

    case ResultReason::NoMatch:
    case ResultReason::RecognizedSpeech:
    case ResultReason::RecognizedIntent:
    case ResultReason::RecognizedKeyword:
    case ResultReason::TranslatedSpeech:
        pevent = &FinalResult;
        SPX_DBG_TRACE_VERBOSE_IF(!pevent->IsConnected(), "%s: No FinalResult event signal connected!! nobody listening...", __FUNCTION__);
        break;

    case ResultReason::RecognizingSpeech:
    case ResultReason::RecognizingIntent:
    case ResultReason::RecognizingKeyword:
    case ResultReason::TranslatingSpeech:
        pevent = &IntermediateResult;
        break;

    case ResultReason::SynthesizingAudio:
    case ResultReason::SynthesizingAudioCompleted:
        pevent = &TranslationSynthesisResult;
        break;

    default:
        // This should be changed to throw exception. But currently it causes problem in lock.
        // Bug: https://msasg.visualstudio.com/Skyman/_workitems/edit/1314877
        SPX_DBG_ASSERT_WITH_MESSAGE(false, "The reason found in the result was unexpected.");
        break;
    }

    FireRecoEvent(pevent, sessionId, result);
}

void CSpxRecognizer::FireRecoEvent(ISpxRecognizerEvents::RecoEvent_Type* pevent, const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result, uint64_t offset)
{
    if (pevent != nullptr && pevent->IsConnected())
    {
        auto factory = GetEventArgsFactory();
        auto recoEvent = (result != nullptr)
            ? factory->CreateRecognitionEventArgs(sessionId, result)
            : factory->CreateRecognitionEventArgs(sessionId, offset);
        pevent->Signal(recoEvent);
    }
}

void CSpxRecognizer::SetStringValueInProperties(const char* name, const char* value)
{
    // For now, we can only have one Recognizer per Session, so, we'll just pass this over to the default session.
    EnsureDefaultSession();
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
    namedProperties->SetStringValue(name, value);
}

std::string CSpxRecognizer::GetStringValueFromProperties(const char* name, const char* defaultValue)
{
    // For now, we can only have one Recognizer per Session, so, we'll just pass this over to the default session.
    EnsureDefaultSession();
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
    return namedProperties->GetStringValue(name, defaultValue);
}

void CSpxRecognizer::EnsureDefaultSession()
{
    if (m_defaultSession == nullptr)
    {
        SPX_DBG_ASSERT(GetSite());
        m_defaultSession = GetSite()->GetDefaultSession();
    }
}

void CSpxRecognizer::TermDefaultSession()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    if (m_defaultSession != nullptr)
    {
        m_defaultSession->RemoveRecognizer(this);
    }
    SpxTermAndClear(m_defaultSession);
}

std::shared_ptr<ISpxPhraseList> CSpxRecognizer::EnsureDefaultPhraseListGrammar()
{
    if (m_phraselist == nullptr)
    {
        auto phraselist = SpxCreateObjectWithSite<ISpxPhraseList>("CSpxPhraseListGrammar", this);
        phraselist->InitPhraseList(L"");

        m_phraselist = phraselist;

        AddGrammar(SpxQueryInterface<ISpxGrammar>(m_phraselist));
    }

    return m_phraselist;
}

void CSpxRecognizer::OnIsEnabledChanged()
{
    // no op currently
}

void CSpxRecognizer::CheckLogFilename()
{
    auto namedProperties = SpxQueryService<ISpxNamedProperties>(m_defaultSession);
    if (namedProperties == nullptr)
    {
        return;
    }

    auto logFile = namedProperties->GetStringValue(GetPropertyName(PropertyId::Speech_LogFilename), NOT_SET_VALUE);
    auto& logger = FileLogger::Instance();

    // are we already logging? If named properties doesn't explicitly set logging, let's not turn it off here
    if (logger.IsFileLoggingEnabled() && logFile == NOT_SET_VALUE)
    {
        return;
    }

    FileLogger::Instance().SetFileOptions(namedProperties);
}

std::shared_ptr<ISpxNamedProperties> CSpxRecognizer::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

std::shared_ptr<ISpxConnection> CSpxRecognizer::GetConnection()
{
    auto recognizerAsSite = SpxSiteFromThis(this);
    auto connection = SpxCreateObjectWithSite<ISpxConnection>("CSpxConnection", recognizerAsSite);

    auto initConnection = SpxQueryInterface<ISpxConnectionInit>(connection);
    initConnection->Init(SpxSharedPtrFromThis<ISpxRecognizer>(this), SpxSharedPtrFromThis<ISpxMessageParamFromUser>(this));

    return connection;
}

void CSpxRecognizer::SetParameter(std::string&& path, std::string&& name, std::string&& value)
{
    if (value.length() > MAX_JSON_PAYLOAD_FROM_USER)
    {
        ThrowInvalidArgumentException("The value for SpeechContext exceed 50 MBytes!");
    }

    try
    {
        auto j = nlohmann::json::parse(value);
    }
    catch (nlohmann::json::parse_error& e)
    {
        UNUSED(e);
        std::stringstream ss;
        ss << "The user specified path: ";
        ss << path;
        ss << "  parameter name: ";
        ss << name;
        ss << " parameter value: ";
        ss << value;
        ss << " has invalid json string.";
        ThrowInvalidArgumentException(ss.str());
    }

    transform(path.begin(), path.end(), path.begin(), [](unsigned char c) ->char { return (char)::tolower(c); });
    {
        std::unique_lock<std::mutex> lock{ m_uspParameterLock };

        auto existing = m_uspParametersFromUser.find(path);
        if (existing == end(m_uspParametersFromUser))
        {
            m_uspParametersFromUser[path] = { {move(name), move(value)} };
        }
        else
        {
            existing->second.insert({ {name,value} });
        }
    }
}

CSpxStringMap CSpxRecognizer::GetParametersFromUser(std::string&& path)
{
    CSpxStringMap result;
    {
        std::unique_lock<std::mutex> lock{ m_uspParameterLock };

        auto search = m_uspParametersFromUser.find(path);
        if (search != end(m_uspParametersFromUser))
        {
            result = search->second;
        }

    }
    return result;
}

void CSpxRecognizer::SendNetworkMessage(std::string&& path, std::string&& payload)
{
    if (payload.length() > MAX_JSON_PAYLOAD_FROM_USER)
    {
        ThrowInvalidArgumentException("The value for SpeechEvent exceed 50 MBytes!");
    }

    try
    {
        auto j = nlohmann::json::parse(payload);
    }
    catch (nlohmann::json::parse_error& e)
    {
        std::string message = "The payload of speech event is invalid, " + std::string(e.what());
        ThrowInvalidArgumentException(message);
    }

    SPX_IFTRUE_THROW_HR(m_defaultSession == nullptr, SPXERR_INVALID_ARG);
    m_defaultSession->SendNetworkMessage(std::move(path), std::move(payload));
}

} } } } // Microsoft::CognitiveServices::Speech::Impl

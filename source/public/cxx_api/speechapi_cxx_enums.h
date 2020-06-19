//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_enums.h: Public API declarations for C++ enumerations
//

#pragma once

#include <string>
#include <speechapi_cxx_common.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

constexpr const char* TrueString = "true";
constexpr const char* FalseString = "false";
constexpr const char CommaDelim = ',';

/// <summary>
/// Defines speech property ids.
/// Changed in version 1.4.0.
/// </summary>
enum class PropertyId
{
    /// <summary>
    /// The Cognitive Services Speech Service subscription key. If you are using an intent recognizer, you need
    /// to specify the LUIS endpoint key for your particular LUIS app. Under normal circumstances, you shouldn't
    /// have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::FromSubscription"/>.
    /// </summary>
    SpeechServiceConnection_Key = 1000,

    /// <summary>
    /// The Cognitive Services Speech Service endpoint (url). Under normal circumstances, you shouldn't
    /// have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::FromEndpoint"/>.
    /// NOTE: This endpoint is not the same as the endpoint used to obtain an access token.
    /// </summary>
    SpeechServiceConnection_Endpoint = 1001,

    /// <summary>
    /// The Cognitive Services Speech Service region. Under normal circumstances, you shouldn't have to
    /// use this property directly.
    /// Instead, use <see cref="SpeechConfig::FromSubscription"/>, <see cref="SpeechConfig::FromEndpoint"/>,
    /// <see cref="SpeechConfig::FromHost"/>, <see cref="SpeechConfig::FromAuthorizationToken"/>.
    /// </summary>
    SpeechServiceConnection_Region = 1002,

    /// <summary>
    /// The Cognitive Services Speech Service authorization token (aka access token). Under normal circumstances,
    /// you shouldn't have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::FromAuthorizationToken"/>,
    /// <see cref="SpeechRecognizer::SetAuthorizationToken"/>, <see cref="IntentRecognizer::SetAuthorizationToken"/>,
    /// <see cref="TranslationRecognizer::SetAuthorizationToken"/>.
    /// </summary>
    SpeechServiceAuthorization_Token = 1003,

    /// <summary>
    /// The Cognitive Services Speech Service authorization type. Currently unused.
    /// </summary>
    SpeechServiceAuthorization_Type = 1004,

    /// <summary>
    /// The Cognitive Services Custom Speech Service endpoint id. Under normal circumstances, you shouldn't
    /// have to use this property directly.
    /// Instead use <see cref="SpeechConfig::SetEndpointId"/>.
    /// NOTE: The endpoint id is available in the Custom Speech Portal, listed under Endpoint Details.
    /// </summary>
    SpeechServiceConnection_EndpointId = 1005,

    /// <summary>
    /// The Cognitive Services Speech Service host (url). Under normal circumstances, you shouldn't
    /// have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::FromHost"/>.
    /// </summary>
    SpeechServiceConnection_Host = 1006,

    /// <summary>
    /// The host name of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
    /// you shouldn't have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::SetProxy"/>.
    /// NOTE: This property id was added in version 1.1.0.
    /// </summary>
    SpeechServiceConnection_ProxyHostName = 1100,

    /// <summary>
    /// The port of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
    /// you shouldn't have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::SetProxy"/>.
    /// NOTE: This property id was added in version 1.1.0.
    /// </summary>
    SpeechServiceConnection_ProxyPort = 1101,

    /// <summary>
    /// The user name of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
    /// you shouldn't have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::SetProxy"/>.
    /// NOTE: This property id was added in version 1.1.0.
    /// </summary>
    SpeechServiceConnection_ProxyUserName = 1102,

    /// <summary>
    /// The password of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
    /// you shouldn't have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::SetProxy"/>.
    /// NOTE: This property id was added in version 1.1.0.
    /// </summary>
    SpeechServiceConnection_ProxyPassword = 1103,

    /// <summary>
    /// The URL string built from speech configuration.
    /// This property is intended to be read-only. The SDK is using it internally.
    /// NOTE: Added in version 1.5.0.
    /// </summary>
    SpeechServiceConnection_Url = 1104,

    /// <summary>
    /// The list of comma separated languages used as target translation languages. Under normal circumstances,
    /// you shouldn't have to use this property directly. Instead use <see cref="SpeechTranslationConfig::AddTargetLanguage"/>
    /// and <see cref="SpeechTranslationConfig::GetTargetLanguages"/>.
    /// </summary>
    SpeechServiceConnection_TranslationToLanguages = 2000,

    /// <summary>
    /// The name of the Cognitive Service Text to Speech Service voice. Under normal circumstances, you shouldn't have to use this
    /// property directly. Instead use <see cref="SpeechTranslationConfig::SetVoiceName"/>.
    /// NOTE: Valid voice names can be found <a href="https://aka.ms/csspeech/voicenames">here</a>.
    /// </summary>
    SpeechServiceConnection_TranslationVoice = 2001,

    /// <summary>
    /// Translation features. For internal use.
    /// </summary>
    SpeechServiceConnection_TranslationFeatures = 2002,

    /// <summary>
    /// The Language Understanding Service region. Under normal circumstances, you shouldn't have to use this property directly.
    /// Instead use <see cref="LanguageUnderstandingModel"/>.
    /// </summary>
    SpeechServiceConnection_IntentRegion = 2003,

    /// <summary>
    /// The Cognitive Services Speech Service recognition mode. Can be "INTERACTIVE", "CONVERSATION", "DICTATION".
    /// This property is intended to be read-only. The SDK is using it internally.
    /// </summary>
    SpeechServiceConnection_RecoMode = 3000,

    /// <summary>
    /// The spoken language to be recognized (in BCP-47 format). Under normal circumstances, you shouldn't have to use this property
    /// directly.
    /// Instead, use <see cref="SpeechConfig::SetSpeechRecognitionLanguage"/>.
    /// </summary>
    SpeechServiceConnection_RecoLanguage = 3001,

    /// <summary>
    /// The session id. This id is a universally unique identifier (aka UUID) representing a specific binding of an audio input stream
    /// and the underlying speech recognition instance to which it is bound. Under normal circumstances, you shouldn't have to use this
    /// property directly.
    /// Instead use <see cref="SessionEventArgs::SessionId"/>.
    /// </summary>
    Speech_SessionId = 3002,

    /// <summary>
    /// The query parameters provided by users. They will be passed to service as URL query parameters.
    /// Added in version 1.5.0
    /// </summary>
    SpeechServiceConnection_UserDefinedQueryParameters = 3003,

    /// <summary>
    /// The spoken language to be synthesized (e.g. en-US)
    /// Added in version 1.4.0
    /// </summary>
    SpeechServiceConnection_SynthLanguage = 3100,

    /// <summary>
    /// The name of the TTS voice to be used for speech synthesis
    /// Added in version 1.4.0
    /// </summary>
    SpeechServiceConnection_SynthVoice = 3101,

    /// <summary>
    /// The string to specify TTS output audio format
    /// Added in version 1.4.0
    /// </summary>
    SpeechServiceConnection_SynthOutputFormat = 3102,

    /// <summary>
    /// The initial silence timeout value (in milliseconds) used by the service.
    /// Added in version 1.5.0
    /// </summary>
    SpeechServiceConnection_InitialSilenceTimeoutMs = 3200,

    /// <summary>
    /// The end silence timeout value (in milliseconds) used by the service.
    /// Added in version 1.5.0
    /// </summary>
    SpeechServiceConnection_EndSilenceTimeoutMs = 3201,

    /// <summary>
    /// A boolean value specifying whether audio logging is enabled in the service or not.
    /// Added in version 1.5.0
    /// </summary>
    SpeechServiceConnection_EnableAudioLogging = 3202,

    /// <summary>
    /// The auto detect source languages
    /// Added in version 1.8.0
    /// </summary>
    SpeechServiceConnection_AutoDetectSourceLanguages = 3300,

    /// <summary>
    /// The auto detect source language result
    /// Added in version 1.8.0
    /// </summary>
    SpeechServiceConnection_AutoDetectSourceLanguageResult = 3301,

    /// <summary>
    /// The requested Cognitive Services Speech Service response output format (simple or detailed). Under normal circumstances, you shouldn't have
    /// to use this property directly.
    /// Instead use <see cref="SpeechConfig::SetOutputFormat"/>.
    /// </summary>
    SpeechServiceResponse_RequestDetailedResultTrueFalse = 4000,

    /// <summary>
    /// The requested Cognitive Services Speech Service response output profanity level. Currently unused.
    /// </summary>
    SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4001,

    /// <summary>
    /// The requested Cognitive Services Speech Service response output profanity setting.
    /// Allowed values are "masked", "removed", and "raw".
    /// Added in version 1.5.0.
    /// </summary>
    SpeechServiceResponse_ProfanityOption = 4002,

    /// <summary>
    /// A string value specifying which post processing option should be used by service.
    /// Allowed values are "TrueText".
    /// Added in version 1.5.0
    /// </summary>
    SpeechServiceResponse_PostProcessingOption = 4003,

    /// <summary>
    /// A boolean value specifying whether to include word-level timestamps in the response result.
    /// Added in version 1.5.0
    /// </summary>
    SpeechServiceResponse_RequestWordLevelTimestamps = 4004,

    /// <summary>
    /// The number of times a word has to be in partial results to be returned.
    /// Added in version 1.5.0
    /// </summary>
    SpeechServiceResponse_StablePartialResultThreshold = 4005,

    /// <summary>
    /// A string value specifying the output format option in the response result. Internal use only.
    /// Added in version 1.5.0.
    /// </summary>
    SpeechServiceResponse_OutputFormatOption = 4006,

    /// <summary>
    /// A boolean value to request for stabilizing translation partial results by omitting words in the end.
    /// Added in version 1.5.0.
    /// </summary>
    SpeechServiceResponse_TranslationRequestStablePartialResult = 4100,

    /// <summary>
    /// The Cognitive Services Speech Service response output (in JSON format). This property is available on recognition result objects only.
    /// </summary>
    SpeechServiceResponse_JsonResult = 5000,

    /// <summary>
    /// The Cognitive Services Speech Service error details (in JSON format). Under normal circumstances, you shouldn't have to
    /// use this property directly.
    /// Instead, use <see cref="CancellationDetails::ErrorDetails"/>.
    /// </summary>
    SpeechServiceResponse_JsonErrorDetails = 5001,

    /// <summary>
    /// The recognition latency in milliseconds. Read-only, available on final speech/translation/intent results.
    /// This measures the latency between when an audio input is received by the SDK, and the moment the final result is received from the service.
    /// The SDK computes the time difference between the last audio fragment from the audio input that is contributing to the final result, and the time the final result is received from the speech service.
    /// Added in version 1.3.0.
    /// </summary>
    SpeechServiceResponse_RecognitionLatencyMs = 5002,

    /// <summary>
    /// The cancellation reason. Currently unused.
    /// </summary>
    CancellationDetails_Reason = 6000,

    /// <summary>
    /// The cancellation text. Currently unused.
    /// </summary>
    CancellationDetails_ReasonText = 6001,

    /// <summary>
    /// The cancellation detailed text. Currently unused.
    /// </summary>
    CancellationDetails_ReasonDetailedText = 6002,

    /// <summary>
    /// The Language Understanding Service response output (in JSON format). Available via <see cref="IntentRecognitionResult.Properties"/>.
    /// </summary>
    LanguageUnderstandingServiceResponse_JsonResult = 7000,

    /// <summary>
    /// The device name for audio capture. Under normal circumstances, you shouldn't have to
    /// use this property directly.
    /// Instead, use <see cref="AudioConfig::FromMicrophoneInput"/>.
    /// NOTE: This property id was added in version 1.3.0.
    /// </summary>
    AudioConfig_DeviceNameForCapture = 8000,

    /// <summary>
    /// The number of channels for audio capture. Internal use only.
    /// NOTE: This property id was added in version 1.3.0.
    /// </summary>
    AudioConfig_NumberOfChannelsForCapture = 8001,

    /// <summary>
    /// The sample rate (in Hz) for audio capture. Internal use only.
    /// NOTE: This property id was added in version 1.3.0.
    /// </summary>
    AudioConfig_SampleRateForCapture = 8002,

    /// <summary>
    /// The number of bits of each sample for audio capture. Internal use only.
    /// NOTE: This property id was added in version 1.3.0.
    /// </summary>
    AudioConfig_BitsPerSampleForCapture = 8003,

    /// <summary>
    /// The audio source. Allowed values are "Microphones", "File", and "Stream".
    /// Added in version 1.3.0.
    /// </summary>
    AudioConfig_AudioSource = 8004,

    /// <summary>
    /// The file name to write logs.
    /// Added in version 1.4.0.
    /// </summary>
    Speech_LogFilename = 9001,

    /// <summary>
    /// Identifier used to connect to the backend service.
    /// Added in version 1.5.0.
    /// </summary>
    Conversation_ApplicationId = 10000,

    /// <summary>
    /// Type of dialog backend to connect to.
    /// Added in version 1.7.0.
    /// </summary>
    Conversation_DialogType = 10001,

    /// <summary>
    /// Silence timeout for listening
    /// Added in version 1.5.0.
    /// </summary>
    Conversation_Initial_Silence_Timeout = 10002,

    /// <summary>
    /// From id to be used on speech recognition activities
    /// Added in version 1.5.0.
    /// </summary>
    Conversation_From_Id = 10003,

    /// <summary>
    /// ConversationId for the session.
    /// Added in version 1.8.0.
    /// </summary>
    Conversation_Conversation_Id = 10004,

    /// <summary>
    /// Comma separated list of custom voice deployment ids.
    /// Added in version 1.8.0.
    /// </summary>
    Conversation_Custom_Voice_Deployment_Ids = 10005,

    /// <summary>
    /// Speech activity template, stamp properties in the template on the activity generated by the service for speech.
    /// Added in version 1.10.0.
    /// </summary>
    Conversation_Speech_Activity_Template = 10006,

    /// <summary>
    /// Your participant identifier in the current conversation.
    /// Added in version 1.13.0
    /// </summary>
    Conversation_ParticipantId = 10007,

    /// <summary>
    /// The time stamp associated to data buffer written by client when using Pull/Push audio input streams.
    /// The time stamp is a 64-bit value with a resolution of 90 kHz. It is the same as the presentation timestamp in an MPEG transport stream. See https://en.wikipedia.org/wiki/Presentation_timestamp
    /// Added in version 1.5.0.
    /// </summary>
    DataBuffer_TimeStamp = 11001,

    /// <summary>
    /// The user id associated to data buffer written by client when using Pull/Push audio input streams.
    /// Added in version 1.5.0.
    /// </summary>
    DataBuffer_UserId = 11002
};

enum class OutputFormat
{
    Simple = 0,
    Detailed = 1
};

enum class ProfanityOption
{
    Masked = 0,
    Removed = 1,
    Raw = 2
};

/// <summary>
/// Specifies the possible reasons a recognition result might be generated.
/// </summary>
enum class ResultReason
{
    /// <summary>
    /// Indicates speech could not be recognized. More details can be found in the NoMatchDetails object.
    /// </summary>
    NoMatch = 0,

    /// <summary>
    /// Indicates that the recognition was canceled. More details can be found using the CancellationDetails object.
    /// </summary>
    Canceled = 1,

    /// <summary>
    /// Indicates the speech result contains hypothesis text.
    /// </summary>
    RecognizingSpeech = 2,

    /// <summary>
    /// Indicates the speech result contains final text that has been recognized.
    /// Speech Recognition is now complete for this phrase.
    /// </summary>
    RecognizedSpeech = 3,

    /// <summary>
    /// Indicates the intent result contains hypothesis text and intent.
    /// </summary>
    RecognizingIntent = 4,

    /// <summary>
    /// Indicates the intent result contains final text and intent.
    /// Speech Recognition and Intent determination are now complete for this phrase.
    /// </summary>
    RecognizedIntent = 5,

    /// <summary>
    /// Indicates the translation result contains hypothesis text and its translation(s).
    /// </summary>
    TranslatingSpeech = 6,

    /// <summary>
    /// Indicates the translation result contains final text and corresponding translation(s).
    /// Speech Recognition and Translation are now complete for this phrase.
    /// </summary>
    TranslatedSpeech = 7,

    /// <summary>
    /// Indicates the synthesized audio result contains a non-zero amount of audio data
    /// </summary>
    SynthesizingAudio = 8,

    /// <summary>
    /// Indicates the synthesized audio is now complete for this phrase.
    /// </summary>
    SynthesizingAudioCompleted = 9,

    /// <summary>
    /// Indicates the speech result contains (unverified) keyword text.
    /// Added in version 1.3.0
    /// </summary>
    RecognizingKeyword = 10,

    /// <summary>
    /// Indicates that keyword recognition completed recognizing the given keyword.
    /// Added in version 1.3.0
    /// </summary>
    RecognizedKeyword = 11,

    /// <summary>
    /// Indicates the speech synthesis is now started
    /// Added in version 1.4.0
    /// </summary>
    SynthesizingAudioStarted = 12,

    /// <summary>
    /// Indicates the transcription result contains hypothesis text and its translation(s) for
    /// other participants in the conversation.
    /// Added in version 1.8.0
    /// </summary>
    TranslatingParticipantSpeech = 13,

    /// <summary>
    /// Indicates the transcription result contains final text and corresponding translation(s)
    /// for other participants in the conversation. Speech Recognition and Translation are now
    /// complete for this phrase.
    /// Added in version 1.8.0
    /// </summary>
    TranslatedParticipantSpeech = 14,

    /// <summary>
    /// Indicates the transcription result contains the instant message and corresponding
    /// translation(s).
    /// Added in version 1.8.0
    /// </summary>
    TranslatedInstantMessage = 15,

    /// <summary>
    /// Indicates the transcription result contains the instant message for other participants
    /// in the conversation and corresponding translation(s).
    /// Added in version 1.8.0
    /// </summary>
    TranslatedParticipantInstantMessage = 16,

    /// <summary>
    /// Indicates the voice profile is being enrolling and customers need to send more audio to create a voice profile.
    /// Added in version 1.12.0
    /// </summary>
    EnrollingVoiceProfile = 17,

    /// <summary>
    /// The voice profile has been enrolled.
    /// Added in version 1.12.0
    /// </summary>
    EnrolledVoiceProfile = 18,

    /// <summary>
    /// Indicates successful identification of some speakers.
    /// Added in version 1.12.0
    /// </summary>
    RecognizedSpeakers = 19,

    /// <summary>
    /// Indicates successfully verified one speaker.
    /// Added in version 1.12.0
    /// </summary>
    RecognizedSpeaker = 20,

    /// <summary>
    /// Indicates a voice profile has been reset successfully.
    /// Added in version 1.12.0
    /// </summary>
    ResetVoiceProfile = 21,

    /// <summary>
    /// Indicates a voice profile has been deleted successfully.
    /// Added in version 1.12.0
    /// </summary>
    DeletedVoiceProfile = 22
};

/// <summary>
/// Defines the possible reasons a recognition result might be canceled.
/// </summary>
enum class CancellationReason
{
    /// <summary>
    /// Indicates that an error occurred during speech recognition.
    /// </summary>
    Error = 1,

    /// <summary>
    /// Indicates that the end of the audio stream was reached.
    /// </summary>
    EndOfStream = 2
};

/// <summary>
/// Defines error code in case that CancellationReason is Error.
/// Added in version 1.1.0.
/// </summary>
enum class CancellationErrorCode
{
    /// <summary>
    /// No error.
    /// If CancellationReason is EndOfStream, CancellationErrorCode
    /// is set to NoError.
    /// </summary>
    NoError = 0,

    /// <summary>
    /// Indicates an authentication error.
    /// An authentication error occurs if subscription key or authorization token is invalid, expired,
    /// or does not match the region being used.
    /// </summary>
    AuthenticationFailure = 1,

    /// <summary>
    /// Indicates that one or more recognition parameters are invalid or the audio format is not supported.
    /// </summary>
    BadRequest = 2,

    /// <summary>
    /// Indicates that the number of parallel requests exceeded the number of allowed concurrent transcriptions for the subscription.
    /// </summary>
    TooManyRequests = 3,

    /// <summary>
    /// Indicates that the free subscription used by the request ran out of quota.
    /// </summary>
    Forbidden = 4,

    /// <summary>
    /// Indicates a connection error.
    /// </summary>
    ConnectionFailure = 5,

    /// <summary>
    /// Indicates a time-out error when waiting for response from service.
    /// </summary>
    ServiceTimeout = 6,

    /// <summary>
    /// Indicates that an error is returned by the service.
    /// </summary>
    ServiceError = 7,

    /// <summary>
    /// Indicates that the service is currently unavailable.
    /// </summary>
    ServiceUnavailable = 8,

    /// <summary>
    /// Indicates an unexpected runtime error.
    /// </summary>
    RuntimeError = 9,

    /// <summary>
    /// Indicates the Speech Service is temporarily requesting a reconnect to a different endpoint.
    /// </summary>
    /// <remarks>Used internally</remarks>
    ServiceRedirectTemporary = 10,

    /// <summary>
    /// Indicates the Speech Service is permanently requesting a reconnect to a different endpoint.
    /// </summary>
    /// <remarks>Used internally</remarks>
    ServiceRedirectPermanent = 11,
};

/// <summary>
/// Defines the possible reasons a recognition result might not be recognized.
/// </summary>
enum class NoMatchReason
{
    /// <summary>
    /// Indicates that speech was detected, but not recognized.
    /// </summary>
    NotRecognized = 1,

    /// <summary>
    /// Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
    /// </summary>
    InitialSilenceTimeout = 2,

    /// <summary>
    /// Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
    /// </summary>
    InitialBabbleTimeout = 3,

    /// <summary>
    /// Indicates that the spotted keyword has been rejected by the keyword verification service.
    /// Added in version 1.5.0.
    /// </summary>
    KeywordNotRecognized = 4
};

/// <summary>
/// Defines the possible types for an activity json value.
/// Added in version 1.5.0
/// </summary>
enum class ActivityJSONType : int
{
    Null = 0,
    Object = 1,
    Array = 2,
    String = 3,
    Double = 4,
    UInt = 5,
    Int = 6,
    Boolean = 7
};


/// <summary>
/// Defines the possible speech synthesis output audio formats.
/// Added in version 1.4.0
/// </summary>
enum class SpeechSynthesisOutputFormat
{
    /// <summary>
    /// raw-8khz-8bit-mono-mulaw
    /// </summary>
    Raw8Khz8BitMonoMULaw = 1,

    /// <summary>
    /// riff-16khz-16kbps-mono-siren
    /// </summary>
    Riff16Khz16KbpsMonoSiren = 2,

    /// <summary>
    /// audio-16khz-16kbps-mono-siren
    /// </summary>
    Audio16Khz16KbpsMonoSiren = 3,

    /// <summary>
    /// audio-16khz-32kbitrate-mono-mp3
    /// </summary>
    Audio16Khz32KBitRateMonoMp3 = 4,

    /// <summary>
    /// audio-16khz-128kbitrate-mono-mp3
    /// </summary>
    Audio16Khz128KBitRateMonoMp3 = 5,

    /// <summary>
    /// audio-16khz-64kbitrate-mono-mp3
    /// </summary>
    Audio16Khz64KBitRateMonoMp3 = 6,

    /// <summary>
    /// audio-24khz-48kbitrate-mono-mp3
    /// </summary>
    Audio24Khz48KBitRateMonoMp3 =7,

    /// <summary>
    /// audio-24khz-96kbitrate-mono-mp3
    /// </summary>
    Audio24Khz96KBitRateMonoMp3 = 8,

    /// <summary>
    /// audio-24khz-160kbitrate-mono-mp3
    /// </summary>
    Audio24Khz160KBitRateMonoMp3 = 9,

    /// <summary>
    /// raw-16khz-16bit-mono-truesilk
    /// </summary>
    Raw16Khz16BitMonoTrueSilk = 10,

    /// <summary>
    /// riff-16khz-16bit-mono-pcm
    /// </summary>
    Riff16Khz16BitMonoPcm = 11,

    /// <summary>
    /// riff-8khz-16bit-mono-pcm
    /// </summary>
    Riff8Khz16BitMonoPcm = 12,

    /// <summary>
    /// riff-24khz-16bit-mono-pcm
    /// </summary>
    Riff24Khz16BitMonoPcm = 13,

    /// <summary>
    /// riff-8khz-8bit-mono-mulaw
    /// </summary>
    Riff8Khz8BitMonoMULaw = 14,

    /// <summary>
    /// raw-16khz-16bit-mono-pcm
    /// </summary>
    Raw16Khz16BitMonoPcm = 15,

    /// <summary>
    /// raw-24khz-16bit-mono-pcm
    /// </summary>
    Raw24Khz16BitMonoPcm = 16,

    /// <summary>
    /// raw-8khz-16bit-mono-pcm
    /// </summary>
    Raw8Khz16BitMonoPcm = 17
};

/// <summary>
/// Defines the possible status of audio data stream.
/// Added in version 1.4.0
/// </summary>
enum class StreamStatus
{
    /// <summary>
    /// The audio data stream status is unknown
    /// </summary>
    Unknown = 0,

    /// <summary>
    /// The audio data stream contains no data
    /// </summary>
    NoData = 1,

    /// <summary>
    /// The audio data stream contains partial data of a speak request
    /// </summary>
    PartialData = 2,

    /// <summary>
    /// The audio data stream contains all data of a speak request
    /// </summary>
    AllData = 3,

    /// <summary>
    /// The audio data stream was canceled
    /// </summary>
    Canceled = 4
};

/// <summary>
/// Defines channels used to pass property settings to service.
/// Added in version 1.5.0.
/// </summary>
enum class ServicePropertyChannel
{
    /// <summary>
    /// Uses URI query parameter to pass property settings to service.
    /// </summary>
    UriQueryParameter = 0
};

namespace Transcription
{
    /// <summary>
    /// Why the participant changed event was raised
    /// Added in version 1.8.0
    /// </summary>
    enum class ParticipantChangedReason
    {
        /// <summary>
        /// Participant has joined the conversation
        /// </summary>
        JoinedConversation,

        /// <summary>
        /// Participant has left the conversation. This could be voluntary, or involuntary
        /// (e.g. they are experiencing networking issues)
        /// </summary>
        LeftConversation,

        /// <summary>
        /// The participants' state has changed (e.g. they became muted, changed their nickname)
        /// </summary>
        Updated
    };
}

/// <summary>
/// Defines voice profile types
/// </summary>
enum class VoiceProfileType
{
    /// <summary>
    /// Text independent speaker identification.
    /// </summary>
    TextIndependentIdentification = 1,

    /// <summary>
    ///  Text dependent speaker verification.
    /// </summary>
    TextDependentVerification = 2,

    /// <summary>
    /// Text independent verification.
    /// </summary>
    TextIndependentVerification = 3
};

/// <summary>
/// Defines the scope that a Recognition Factor is applied to.
/// </summary>
enum class RecognitionFactorScope
{
    /// <summary>
    /// A Recognition Factor will apply to grammars that can be referenced as individual partial phrases.
    /// </summary>
    /// <remarks>
    /// Currently only applies to PhraseListGrammars
    /// </remarks>
    PartialPhrase = 1,
};

} } } // Microsoft::CognitiveServices::Speech

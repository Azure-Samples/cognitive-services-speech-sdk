//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// NOTES:
// - Install the NuGet package Microsoft.CognitiveServices.Speech.
// - The Speech SDK on Windows requires the Microsoft Visual C++ Redistributable for Visual Studio 2019 on the system. See:
// https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-sdk
// - In Windows, place the following files from the Speech SDK in the same folder as your compiled .exe:
//     - Microsoft.CognitiveServices.Speech.core.dll
//     - Microsoft.CognitiveServices.Speech.extension.audio.sys.dll

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <speechapi_cxx.h>
#include "binary_file_reader.h"
#include "caption_helper.h"
#include "string_helper.h"
#include "user_config.h"
#include "wav_file_reader.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Speaker;

// Helper functions.
template <typename T>
std::vector<T> concatVectors(std::vector<T>& v1, std::vector<T>& v2)
{
    if (0 == v1.size())
    {
        return std::vector<T>(v2);
    }
    else if (0 == v2.size())
    {
        return std::vector<T>(v1);
    }
    else
    {
        std::vector<T> retval(v1);
        retval.insert(retval.end(), v2.begin(), v2.end());
        return retval;
    }
}

class Captioning
{
private:

    std::shared_ptr<UserConfig> m_userConfig = NULL;
    std::shared_ptr<AudioStreamFormat> m_format = NULL;
    std::shared_ptr<BinaryFileReader> m_callback = NULL;
    std::shared_ptr<PullAudioInputStream> m_stream = NULL;
    int m_srtSequenceNumber = 1;
    std::optional<Caption> m_previousCaption = std::nullopt;
    std::optional<Timestamp> m_previousEndTime = std::nullopt;
    bool m_previousResultIsRecognized = false;
    std::vector<std::string> m_recognizedLines;
    std::vector<std::shared_ptr<SpeechRecognitionResult>> m_offlineResults;

    void WriteToConsole(std::string text)
    {
        if (!m_userConfig->suppressConsoleOutput)
        {
            std::cout << text << std::flush;
        }
    }

    void WriteToConsoleOrFile(std::string text)
    {
        WriteToConsole(text);
        if (m_userConfig->outputFile.has_value())
        {
            std::ofstream outputStream;
            outputStream.open(m_userConfig->outputFile.value(), std::ios_base::app);
            outputStream << text;
            outputStream.close();
        }
    }

    std::string GetTimestamp(Timestamp startTime, Timestamp endTime)
    {
        return StringFromTimestamp(startTime, m_userConfig->useSubRipTextCaptionFormat) + " --> " + StringFromTimestamp(endTime, m_userConfig->useSubRipTextCaptionFormat);
    }

    std::string StringFromCaption(Caption caption)
    {
        std::string retval;
        if (m_userConfig->useSubRipTextCaptionFormat)
        {
            retval += caption.sequence + "\n";
        }
        retval += GetTimestamp(caption.begin, caption.end) + "\n";
        retval += caption.text + "\n\n";
        return retval;
    }

    std::string AdjustRealTimeCaptionText(std::string text, bool isRecognizedResult)
    {
        // Split the caption text into multiple lines based on maxLineLength and lines.
        auto captionHelper = std::make_shared<CaptionHelper>(m_userConfig->language, m_userConfig->maxLineLength, m_userConfig->lines, std::vector<std::shared_ptr<RecognitionResult>>());
        std::vector<std::string> lines = captionHelper->LinesFromText(text);

        // Recognizing results can change with each new result, so we do not save previous Recognizing results.
        // Recognized results are final, so we save them in a member value.
        std::vector<std::string> recognizingLines;
        if (isRecognizedResult)
        {
            m_recognizedLines.insert(m_recognizedLines.end(), lines.begin(), lines.end());
        }
        else
        {
            recognizingLines.insert(recognizingLines.end(), lines.begin(), lines.end());
        }

        std::vector<std::string> combinedLines = concatVectors(m_recognizedLines, recognizingLines);
        auto combinedLinesSize = combinedLines.size();
        int takeLast = combinedLinesSize < m_userConfig->lines ? combinedLinesSize : m_userConfig->lines;
        std::vector<std::string> retval(combinedLines.end() - takeLast, combinedLines.end());
        return StringHelper::Join(retval, "\n");
    }

    std::optional<std::string> CaptionFromRealTimeResult(std::shared_ptr<SpeechRecognitionResult> result, bool isRecognizedResult)
    {
        std::optional<std::string> retval = std::nullopt;

        Timestamp startTime = TimestampFromTicks(result->Offset());
        Timestamp endTime = TimestampFromTicks(result->Offset() + result->Duration());
        // If the end timestamp for the previous result is later
        // than the end timestamp for this result, drop the result.
        // This sometimes happens when we receive a lot of Recognizing results close together.
        if (m_previousEndTime.has_value() && CompareTimestamps(m_previousEndTime.value(), endTime) > 0)
        {
            // Do nothing.
        }
        else
        {
            // Record the end timestamp for this result.
            m_previousEndTime = endTime;

            // Convert the SpeechRecognitionResult to a caption.
            // We are not ready to set the text for this caption.
            // First we need to determine whether to clear m_recognizedLines.
            auto caption = Caption(m_userConfig->language, m_srtSequenceNumber++, TimestampPlusMilliseconds(startTime, m_userConfig->delay), TimestampPlusMilliseconds(endTime, m_userConfig->delay), "");

            // If we have a previous caption...
            if (m_previousCaption.has_value())
            {
                // If the previous result was type Recognized...
                if (m_previousResultIsRecognized)
                {
                    // Set the end timestamp for the previous caption to the earliest of:
                    // - The end timestamp for the previous caption plus the remain time.
                    // - The start timestamp for the current caption.
                    Timestamp previousEnd = TimestampPlusMilliseconds(m_previousCaption.value().end, m_userConfig->remainTime);
                    m_previousCaption.value().end = CompareTimestamps(previousEnd, caption.begin) < 0 ? previousEnd : caption.begin;
                    // If the gap between the original end timestamp for the previous caption
                    // and the start timestamp for the current caption is larger than remainTime,
                    // clear the cached recognized lines.
                    // Note this needs to be done before we call AdjustRealTimeCaptionText
                    // for the current caption, because it uses m_recognizedLines.
                    if (CompareTimestamps(previousEnd, caption.begin) < 0)
                    {
                        m_recognizedLines.clear();
                    }
                }
                // If the previous result was type Recognizing, simply set the start timestamp
                // for the current caption to the end timestamp for the previous caption.
                // Note this presumes there will not be a large gap between Recognizing results,
                // because such a gap would cause the previous Recognizing result to be succeeded
                // by a Recognized result.
                else
                {
                    caption.begin = m_previousCaption.value().end;
                }

                retval = std::optional<std::string>{ StringFromCaption(m_previousCaption.value()) };
            }

            // Break the caption text into lines if needed.
            caption.text = AdjustRealTimeCaptionText(result->Text, isRecognizedResult);
            // Save the current caption as the previous caption.
            m_previousCaption = caption;
            // Save the result type as the previous result type.
            m_previousResultIsRecognized = isRecognizedResult;
        }

        return retval;
    }

    std::vector<Caption> CaptionsFromOfflineResults()
    {
        // Although SpeechRecognitionResult inherits RecognitionResult, we cannot pass
        // std::vector<std::shared_ptr<SpeechRecognitionResult>> as a
        // std::vector<std::shared_ptr<RecognitionResult>>.
        std::vector<std::shared_ptr<RecognitionResult>> offlineResults(m_offlineResults.size());
        std::transform(m_offlineResults.begin(), m_offlineResults.end(), offlineResults.begin(), [](std::shared_ptr<SpeechRecognitionResult> result) {
            return std::static_pointer_cast<RecognitionResult>(result);
        });
        std::vector<Caption> captions = CaptionHelper::GetCaptions(m_userConfig->language, m_userConfig->maxLineLength, m_userConfig->lines, offlineResults);

        // In offline mode, all captions come from RecognitionResults of type Recognized.
        // Set the end timestamp for each caption to the earliest of:
        // - The end timestamp for this caption plus the remain time.
        // - The start timestamp for the next caption.
        for (auto index = 0; index < captions.size() - 1; index++)
        {
            Caption caption_1 = captions[index];
            Caption caption_2 = captions[index + 1];
            Timestamp end = TimestampPlusMilliseconds(caption_1.end, m_userConfig->remainTime);
            caption_1.end = CompareTimestamps(end, caption_2.begin) < 0 ? end : caption_2.begin;
        }
        Caption lastCaption = captions[captions.size() - 1];
        lastCaption.end = TimestampPlusMilliseconds(lastCaption.end, m_userConfig->remainTime);
        return captions;
    }

    std::shared_ptr<Audio::AudioConfig> AudioConfigFromUserConfig()
    {
        if (m_userConfig->inputFile.has_value())
        {
            if (StringHelper::EndsWith(m_userConfig->inputFile.value(), ".wav"))
            {
                auto reader = std::make_shared<WavFileReader>(m_userConfig->inputFile.value());
                m_format = AudioStreamFormat::GetWaveFormatPCM(reader->GetFormat().SamplesPerSec, (uint8_t)reader->GetFormat().BitsPerSample, (uint8_t)reader->GetFormat().Channels);
                reader->Close();
            }
            else
            {
                m_format = AudioStreamFormat::GetCompressedFormat(m_userConfig->compressedAudioFormat);
            }
            m_callback = std::make_shared<BinaryFileReader>(m_userConfig->inputFile.value());
            m_stream = AudioInputStream::CreatePullStream(m_format, m_callback);
            return AudioConfig::FromStreamInput(m_stream);
        }
        else
        {
            return Audio::AudioConfig::FromDefaultMicrophoneInput();
        }
    }

    std::shared_ptr<SpeechConfig> SpeechConfigFromUserConfig()
    {
        std::shared_ptr<SpeechConfig> speechConfig;
        speechConfig = SpeechConfig::FromSubscription(m_userConfig->subscriptionKey, m_userConfig->region);

        speechConfig->SetProfanity(m_userConfig->profanityOption);

        if (m_userConfig->stablePartialResultThreshold.has_value())
        {
            // Note: To get default value:
            // std::cout << speechConfig->GetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold) << std::endl;
            speechConfig->SetProperty(PropertyId::SpeechServiceResponse_StablePartialResultThreshold, m_userConfig->stablePartialResultThreshold.value());
        }
        
        speechConfig->SetProperty(PropertyId::SpeechServiceResponse_PostProcessingOption, "TrueText");
        speechConfig->SetSpeechRecognitionLanguage(m_userConfig->language);
        
        return speechConfig;
    }

public:
    Captioning(std::shared_ptr<UserConfig> userConfig)
        : m_userConfig(userConfig)
    {
        if (m_userConfig->outputFile.has_value())
        {
            std::filesystem::path outputFile { m_userConfig->outputFile.value() };
            // If the output file exists, truncate it.
            if (std::filesystem::exists(outputFile))
            {
                std::ofstream outputStream;
                outputStream.open(m_userConfig->outputFile.value());
                outputStream.close();            
            }
        }
        if (!m_userConfig->useSubRipTextCaptionFormat)
        {
            WriteToConsoleOrFile("WEBVTT\n\n");
        }
    }

    std::shared_ptr<SpeechRecognizer> SpeechRecognizerFromUserConfig()
    {
        std::shared_ptr<AudioConfig> audioConfig = AudioConfigFromUserConfig();
        std::shared_ptr<SpeechConfig> speechConfig = SpeechConfigFromUserConfig();
        std::shared_ptr<SpeechRecognizer> speechRecognizer;

        speechRecognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);

        if (m_userConfig->phraseList.has_value())
        {
            auto grammar = PhraseListGrammar::FromRecognizer(speechRecognizer);
            for (auto phrase : StringHelper::Split(m_userConfig->phraseList.value(), ';')) {
                grammar->AddPhrase(phrase);
            }
        }
        
        return speechRecognizer;
    }

    std::optional<std::string> RecognizeContinuous(std::shared_ptr<SpeechRecognizer> speechRecognizer)
    {
        std::promise<std::optional<std::string>> recognitionEnd;

        // We only use Recognizing results in real-time mode.
        if (CaptioningMode::RealTime == m_userConfig->captioningMode)
        {
            // Capture variables we will need inside the lambda. See:
            // https://www.cppstories.com/2020/08/lambda-capturing.html/
            speechRecognizer->Recognizing.Connect([this](const SpeechRecognitionEventArgs& e)
                {
                    if (ResultReason::RecognizingSpeech == e.Result->Reason && e.Result->Text.length() > 0)
                    {
                        std::optional<std::string> caption = CaptionFromRealTimeResult(e.Result, false);
                        if (caption.has_value())
                        {
                            WriteToConsoleOrFile(caption.value());
                        }
                    }
                    else if (ResultReason::NoMatch == e.Result->Reason)
                    {
                        WriteToConsole("NOMATCH: Speech could not be recognized.\n");
                    }
                });
        }

        speechRecognizer->Recognized.Connect([this](const SpeechRecognitionEventArgs& e)
            {
                if (ResultReason::RecognizedSpeech == e.Result->Reason && e.Result->Text.length() > 0)
                {
                    if (CaptioningMode::Offline == m_userConfig->captioningMode)
                    {
                        m_offlineResults.push_back(e.Result);
                    }
                    else
                    {
                        std::optional<std::string> caption = CaptionFromRealTimeResult(e.Result, true);
                        if (caption.has_value())
                        {
                            WriteToConsoleOrFile(caption.value());
                        }
                    }
                }
                else if (ResultReason::NoMatch == e.Result->Reason)
                {
                    WriteToConsole("NOMATCH: Speech could not be recognized.\n");
                }
            });

        speechRecognizer->Canceled.Connect([this, &recognitionEnd](const SpeechRecognitionCanceledEventArgs& e)
            {
                if (CancellationReason::EndOfStream == e.Reason)
                {
                    WriteToConsole("End of stream reached.\n");
                    recognitionEnd.set_value(std::nullopt); // Notify to stop recognition.
                }
                else if (CancellationReason::CancelledByUser == e.Reason)
                {
                    WriteToConsole("User canceled request.\n");
                    recognitionEnd.set_value(std::nullopt); // Notify to stop recognition.
                }
                else if (CancellationReason::Error == e.Reason)
                {
                    std::ostringstream error;
                    error << "Encountered error.\n"
                        << "ErrorCode: " << (int)e.ErrorCode << "\n"
                        << "ErrorDetails: " << e.ErrorDetails << std::endl;
                    recognitionEnd.set_value(std::optional<std::string>{ error.str() }); // Notify to stop recognition.
                }
                else
                {
                    std::ostringstream error;
                    error << "Request was cancelled for an unrecognized reason: " << (int)e.Reason << std::endl;
                    recognitionEnd.set_value(std::optional<std::string>{ error.str() }); // Notify to stop recognition.
                }
            });

        speechRecognizer->SessionStopped.Connect([this, &recognitionEnd](const SessionEventArgs& e)
            {
                WriteToConsole("Session stopped.\n");
                recognitionEnd.set_value(std::nullopt); // Notify to stop recognition.
            });

        // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
        speechRecognizer->StartContinuousRecognitionAsync().get();

        // Waits for recognition end.
        std::optional<std::string> result = recognitionEnd.get_future().get();

        // Stops recognition.
        speechRecognizer->StopContinuousRecognitionAsync().get();

        return result;
    }

    void Finish()
    {
        if (CaptioningMode::Offline == m_userConfig->captioningMode)
        {
            for (Caption caption : CaptionsFromOfflineResults())
            {
                WriteToConsoleOrFile(StringFromCaption(caption));
            }
        }
        else if (CaptioningMode::RealTime == m_userConfig->captioningMode)
        {
            // Show the last "previous" caption, which is actually the last caption.
            if (m_previousCaption.has_value())
            {
                m_previousCaption.value().end = TimestampPlusMilliseconds(m_previousCaption.value().end, m_userConfig->remainTime);
                WriteToConsoleOrFile(StringFromCaption(m_previousCaption.value()));
            }
        }
    }
};

int main(int argc, char* argv[])
{
    const std::string usage = "Usage: captioning.exe [...]\n\n"
"  HELP\n"
"    --help                           Show this help and stop.\n\n"
"  CONNECTION\n"
"    --key KEY                        Your Azure Speech service resource key.\n"
"                                     Overrides the SPEECH_KEY environment variable. You must set the environment variable (recommended) or use the `--key` option.\n"
"    --region REGION                  Your Azure Speech service region.\n"
"                                     Overrides the SPEECH_REGION environment variable. You must set the environment variable (recommended) or use the `--region` option.\n"
"                                     Examples: westus, eastus\n\n"
"  LANGUAGE\n"
"    --language LANG                  Specify language. This is used when breaking captions into lines.\n"
"                                     Default value is en-US.\n"
"                                     Examples: en-US, ja-JP\n\n"
"  INPUT\n"
"    --input FILE                     Input audio from file (default input is the microphone.)\n"
"    --format FORMAT                  Use compressed audio format.\n"
"                                     If this is not present, uncompressed format (wav) is assumed.\n"
"                                     Valid only with --file.\n"
"                                     Valid values: alaw, any, flac, mp3, mulaw, ogg_opus\n\n"
"  MODE\n"
"    --offline                        Output offline results.\n"
"                                     Overrides --realTime.\n"
"    --realTime                       Output real-time results.\n"
"                                     Default output mode is offline.\n\n"
"  ACCURACY\n"
"    --phrases ""PHRASE1;PHRASE2""    Example: ""Constoso;Jessie;Rehaan""\n\n"
"  OUTPUT\n"
"    --output FILE                    Output captions to text file.\n"
"    --srt                            Output captions in SubRip Text format (default format is WebVTT.)\n"
"    --maxLineLength LENGTH           Set the maximum number of characters per line for a caption to LENGTH.\n"
"                                     Minimum is 20. Default is 37 (30 for Chinese).\n"
"    --lines LINES                    Set the number of lines for a caption to LINES.\n"
"                                     Minimum is 1. Default is 2.\n"
"    --delay MILLISECONDS             How many MILLISECONDS to delay the appearance of each caption.\n"
"                                     Minimum is 0.0. Default is 1000.\n"
"    --remainTime MILLISECONDS        How many MILLISECONDS a caption should remain on screen if it is not replaced by another.\n"
"                                     Minimum is 0.0. Default is 1000.\n\n"
"    --quiet                          Suppress console output, except errors.\n"
"    --profanity OPTION               Valid values: raw, remove, mask\n"
"    --threshold NUMBER               Set stable partial result threshold.\n"
"                                     Default value: 3\n";

    try
    {
        if (CommandLineOptionExists(argv, argv + argc, "--help"))
        {
            std::cout << usage << std::endl;
        }
        else
        {
            std::shared_ptr<UserConfig> userConfig = UserConfigFromArgs(argc, argv, usage);
            auto captioning = std::make_shared<Captioning>(userConfig);
            std::shared_ptr<SpeechRecognizer> speechRecognizer = captioning->SpeechRecognizerFromUserConfig();
            std::optional<std::string> error = captioning->RecognizeContinuous(speechRecognizer);
            if (error.has_value())
            {
                std::cout << error.value() << std::endl;
            }
            captioning->Finish();
            
        }
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
    }
}

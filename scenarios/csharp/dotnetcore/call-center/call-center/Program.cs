//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.Encodings.Web;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;

namespace CallCenter
{
    public class TranscriptionPhrase
    {
        readonly public string text;
        readonly public string itn;
        readonly public string lexical;
        readonly public int speakerNumber;
        readonly public string offset;
        readonly public double offsetInTicks;
        
        public TranscriptionPhrase(string text, string itn, string lexical, int speakerNumber, string offset, double offsetInTicks)
        {
            this.text = text;
            this.itn = itn;
            this.lexical = lexical;
            this.speakerNumber = speakerNumber;
            this.offset = offset;
            this.offsetInTicks = offsetInTicks;
        }
    }
    
    public class SentimentAnalysisResult
    {
        readonly public int speakerNumber;
        readonly public double offsetInTicks;
        readonly public JsonElement document;
        
        public SentimentAnalysisResult(int speakerNumber, double offsetInTicks, JsonElement document)
        {
            this.speakerNumber = speakerNumber;
            this.offsetInTicks = offsetInTicks;
            this.document = document;
        }
    }
    
    public class ConversationAnalysisForSimpleOutput
    {
        readonly public (string aspect, string summary)[] summary;
        readonly public (string category, string text)[][] PIIAnalysis;
        
        public ConversationAnalysisForSimpleOutput((string, string)[] summary, (string, string)[][] PIIAnalysis)
        {
            this.summary = summary;
            this.PIIAnalysis = PIIAnalysis;
        }
    }
    
    public class CombinedRedactedContent
    {
        readonly public int channel;
        readonly public StringBuilder lexical;
        readonly public StringBuilder itn;
        readonly public StringBuilder display;
        
        public CombinedRedactedContent(int channel)
        {
            this.channel = channel;
            this.lexical = new StringBuilder();
            this.itn = new StringBuilder();
            this.display = new StringBuilder();
        }
    }
    
    public class Program
    {
        private UserConfig userConfig;
        
        // How long to wait while polling batch transcription and conversation analysis status.
        private const int waitSeconds = 10;

        // This should not change unless you switch to a new version of the Speech REST API.
        private const string speechTranscriptionPath = "speechtotext/v3.0/transcriptions";

        // These should not change unless you switch to a new version of the Cognitive Language REST API.
        private const string sentimentAnalysisPath = "language/:analyze-text";
        private const string sentimentAnalysisQuery = "api-version=2022-05-01";
        private const string conversationAnalysisPath = "/language/analyze-conversations/jobs";
        private const string conversationAnalysisQuery = "api-version=2022-05-15-preview";
        private const string conversationSummaryModelVersion = "2022-05-15-preview";

        //
        // Constructor
        //
        public Program(string[] args, string usage)
        {
            this.userConfig = new UserConfig(args, usage);
        }

        // Helper method to convert JsonElement to object prior to .NET 6.
        private T ObjectFromJsonElement<T>(JsonElement element)
        {
            var json = element.GetRawText();
            return JsonSerializer.Deserialize<T>(json);
        }

        // Helper method to change value in JsonElement by converting it to a Dictionary and back again.
        private JsonElement AddOrChangeValueInJsonElement<T>(JsonElement element, string key, T newValue)
        {
            // Convert JsonElement to dictionary.
            Dictionary<string, JsonElement> dictionary = ObjectFromJsonElement<Dictionary<string, JsonElement>>(element);
            // Convert newValue to JsonElement.
            using (JsonDocument doc = JsonDocument.Parse(JsonSerializer.Serialize(newValue)))
            {
                // Add/update key in dictionary to newValue.
                // NOTE: Always clone the root element of the JsonDocument. Otherwise, once it is disposed,
                // when you try to access any data you obtained from it, you get the error:
                // "Cannot access a disposed object.
                // Object name: 'JsonDocument'."
                // See:
                // https://docs.microsoft.com/dotnet/api/system.text.json.jsonelement.clone
                dictionary[key] = doc.RootElement.Clone();
                // Convert dictionary back to JsonElement.
                using (JsonDocument doc_2 = JsonDocument.Parse(JsonSerializer.Serialize(dictionary)))
                {
                    return doc_2.RootElement.Clone();
                }
            }
        }

        private async Task<string> CreateTranscription()
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = speechTranscriptionPath;

            // Create Transcription API JSON request sample and schema:
            // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
            // Notes:
            // - locale and displayName are required.
            // - diarizationEnabled should only be used with mono audio input.
            var content = new
                {
                    contentUrls = new string[] { this.userConfig.inputAudioURL },
                    properties = new { diarizationEnabled = !this.userConfig.useStereoAudio },
                    locale = this.userConfig.locale,
                    displayName = $"call_center_{DateTime.Now.ToString()}"
                };
            var response = await RestHelper.SendPost(uri.Uri.ToString(), JsonSerializer.Serialize(content), this.userConfig.speechSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.Created });
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {
                // Create Transcription API JSON response sample and schema:
                // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
                // NOTE: Remember to call ToString(), EnumerateArray(), GetInt32(), etc. on the return value from GetProperty
                // to convert it from a JsonElement.
                var transcriptionUri = document.RootElement.Clone().GetProperty("self").ToString();
                // The transcription ID is at the end of the transcription URI.
                var transcriptionId = transcriptionUri.Split("/").Last();
                // Verify the transcription ID is a valid GUID.
                Guid guid;
                if (!Guid.TryParse(transcriptionId, out guid))
                {
                    throw new Exception($"Unable to parse response from Create Transcription API:{Environment.NewLine}{response.content}");
                }
                return transcriptionId;
            }
        }

        private async Task<bool> GetTranscriptionStatus(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}";
            var response = await RestHelper.SendGet(uri.Uri.ToString(), this.userConfig.speechSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {
                if (0 == string.Compare("Failed", document.RootElement.Clone().GetProperty("status").ToString(), StringComparison.InvariantCultureIgnoreCase))
                {
                    throw new Exception($"Unable to transcribe audio input. Response:{Environment.NewLine}{response.content}");
                }
                else
                {
                    return 0 == string.Compare("Succeeded", document.RootElement.Clone().GetProperty("status").ToString(), StringComparison.InvariantCultureIgnoreCase);
                }
            }
        }

        private async Task WaitForTranscription(string transcriptionId)
        {
            var done = false;
            while(!done)
            {
                Console.WriteLine($"Waiting {waitSeconds} seconds for transcription to complete.");
                Thread.Sleep(waitSeconds * 1000);
                done = await GetTranscriptionStatus(transcriptionId);
            }
            return;
        }

        private async Task<JsonElement> GetTranscriptionFiles(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}/files";
            var response = await RestHelper.SendGet(uri.Uri.ToString(), this.userConfig.speechSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {            
                return document.RootElement.Clone();
            }
        }

        private string GetTranscriptionUri(JsonElement transcriptionFiles)
        {
            // Get Transcription Files JSON response sample and schema:
            // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
            return transcriptionFiles.GetProperty("values").EnumerateArray()
                .First(value => 0 == String.Compare("Transcription", value.GetProperty("kind").ToString(), StringComparison.InvariantCultureIgnoreCase))
                .GetProperty("links").GetProperty("contentUrl").ToString();
        }

        private async Task<JsonElement> GetTranscription(string transcriptionUri)
        {
            var response = await RestHelper.SendGet(transcriptionUri, "", new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {
                return document.RootElement.Clone();
            }
        }

        private TranscriptionPhrase[] GetTranscriptionPhrases(JsonElement transcription)
        {
            return transcription
                .GetProperty("recognizedPhrases").EnumerateArray()
                .Select(phrase =>
                {
                    var best = phrase.GetProperty("nBest").EnumerateArray().First();
                    // If the user specified stereo audio, and therefore we turned off diarization,
                    // the speaker number is replaced by a channel number.
                    var speakerNumber = this.userConfig.useStereoAudio ? phrase.GetProperty("channel").GetInt32() : phrase.GetProperty("speaker").GetInt32();
                    return new TranscriptionPhrase(best.GetProperty("display").ToString(), best.GetProperty("itn").ToString(), best.GetProperty("lexical").ToString(), speakerNumber, phrase.GetProperty("offset").ToString(), phrase.GetProperty("offsetInTicks").GetDouble());
                })
                .OrderBy(phrase => phrase.offsetInTicks)
                .ToArray();
        }

        private async Task DeleteTranscription(string transcriptionId)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.speechEndpoint);
            uri.Path = $"{speechTranscriptionPath}/{transcriptionId}";
            await RestHelper.SendDelete(uri.Uri.ToString(), this.userConfig.speechSubscriptionKey);
            return;
        }

        private SentimentAnalysisResult[] GetSentimentAnalysis(TranscriptionPhrase[] phrases)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.languageEndpoint);
            uri.Path = sentimentAnalysisPath;
            uri.Query = sentimentAnalysisQuery;

            // Create a map of phrase ID to phrase data so we can retrieve it later.
            var phraseData = new Dictionary<int, (int speakerNumber, double offsetInTicks)>();

            // Convert each transcription phrase to a "document" as expected by the sentiment analysis REST API.
            // Include a counter to use as a document ID.
            var documentsToSend = phrases.Select((phrase, id) =>
                {
                    phraseData.Add(id, (phrase.speakerNumber, phrase.offsetInTicks));
                    return new
                    {
                        id = id,
                        language = this.userConfig.language,
                        text = phrase.text
                    };
                }
            );
            
            // We can only analyze sentiment for 10 documents per request.
            // We cannot use SelectMany here because the lambda returns a Task.
            var tasks = documentsToSend.Chunk(10).Select(async chunk =>
                {
                    var content_1 = new { kind = "SentimentAnalysis", analysisInput = new { documents = chunk }};
                    var content_2 = JsonSerializer.Serialize(content_1);
                    var response = await RestHelper.SendPost(uri.Uri.ToString(), content_2, this.userConfig.languageSubscriptionKey, new HttpStatusCode[] { HttpStatusCode.OK });
                    return response.content;
                }
            ).ToArray();
            Task.WaitAll(tasks);
            return tasks.SelectMany(task =>
                {
                    var result = task.Result;
                    using (JsonDocument document = JsonDocument.Parse(result))
                    {
                        return document.RootElement.Clone()
                            .GetProperty("results")
                            .GetProperty("documents").EnumerateArray()
                            .Select(document =>
                                {
                                    (int speakerNumber, double offsetInTicks) = phraseData[Int32.Parse(document.GetProperty("id").ToString())];
                                    return new SentimentAnalysisResult(speakerNumber, offsetInTicks, document);
                                }
                            );
                    }
                }
            ).ToArray();
        }

        private string[] GetSentimentsForSimpleOutput(SentimentAnalysisResult[] sentimentAnalysisResults)
        {
            return sentimentAnalysisResults
                .OrderBy(x => x.offsetInTicks)
                .Select(result => result.document.GetProperty("sentiment").ToString())
                .ToArray();
        }

        private JsonElement[] GetSentimentConfidenceScores(SentimentAnalysisResult[] sentimentAnalysisResults)
        {
            return sentimentAnalysisResults
                .OrderBy(x => x.offsetInTicks)
                .Select(result => result.document.GetProperty("confidenceScores"))
                .ToArray();
        }

        private JsonElement MergeSentimentConfidenceScoresIntoTranscription(JsonElement transcription, JsonElement[] sentimentConfidenceScores)
        {
            IEnumerable<JsonElement> recognizedPhrases_2 = transcription
                .GetProperty("recognizedPhrases").EnumerateArray()
                .OrderBy(phrase => phrase.GetProperty("offsetInTicks").GetDouble())
                // Include a counter to retrieve the sentiment data.
                .Select((phrase, id) =>
                    {
                        IEnumerable<JsonElement> nBest_2 = phrase
                            .GetProperty("nBest")
                            .EnumerateArray()
                            // Add the sentiment confidence scores to the JsonElement in the nBest array.
                            .Select(nBestItem => AddOrChangeValueInJsonElement<JsonElement>(nBestItem, "sentiment", sentimentConfidenceScores[id]));
                        // Update the nBest item in the recognizedPhrases array.
                        return AddOrChangeValueInJsonElement<IEnumerable<JsonElement>>(phrase, "nBest", nBest_2);
                    }
                );
            // Update the recognizedPhrases item in the transcription.
            return AddOrChangeValueInJsonElement<IEnumerable<JsonElement>>(transcription, "recognizedPhrases", recognizedPhrases_2);
        }
        
        private object[] TranscriptionPhrasesToConversationItems(TranscriptionPhrase[] transcriptionPhrases)
        {
            // Include a counter to use as a document ID.
            return transcriptionPhrases.Select((phrase, documentID) =>
                {
                    return new
                    {
                        id = documentID,
                        text = phrase.text,
                        itn = phrase.itn,
                        lexical = phrase.lexical,
                        // The first person to speak is probably the agent.
                        role = 1 == phrase.speakerNumber ? "Agent" : "Customer",
                        participantId = phrase.speakerNumber
                    };
                }
            ).ToArray();
        }

        private async Task<string> RequestConversationAnalysis(object[] conversationItems)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, this.userConfig.languageEndpoint);
            uri.Path = conversationAnalysisPath;
            uri.Query = conversationAnalysisQuery;

            var displayName = $"call_center_{DateTime.Now.ToString()}";

            var content = new
            {
                displayName = displayName,
                analysisInput = new
                {
                    conversations = new object[]
                    {
                        new
                        {
                            id = "conversation1",
                            language = this.userConfig.language,
                            modality = "transcript",
                            conversationItems = conversationItems
                        }
                    }
                },
                tasks = new object[]
                {
                    new
                    {
                        taskName = "summary_1",
                        kind = "ConversationalSummarizationTask",
                        parameters = new
                        {
                            modelVersion = conversationSummaryModelVersion,
                            summaryAspects = new string[]
                            {
                                "Issue",
                                "Resolution"
                            }
                        }
                    },
                    new
                    {
                        parameters = new
                        {
                            piiCategories = new[]
                            {
                                "All",
                            },
                            includeAudioRedaction = false,
                            redactionSource = "lexical",
                            modelVersion = conversationSummaryModelVersion,
                            loggingOptOut = false,
                        },
                        taskName = "PII_1",
                        kind = "ConversationalPIITask"
                    }
                }
            };
            var response = await RestHelper.SendPost(uri.Uri.ToString(), JsonSerializer.Serialize(content), this.userConfig.languageSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.Accepted });
            return response.response.Headers.GetValues("operation-location").First();
        }

        private async Task<bool> GetConversationAnalysisStatus(string conversationAnalysisUrl)
        {
            var response = await RestHelper.SendGet(conversationAnalysisUrl, this.userConfig.languageSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {
                if (0 == string.Compare("Failed", document.RootElement.Clone().GetProperty("status").ToString(), StringComparison.InvariantCultureIgnoreCase))
                {
                    throw new Exception($"Unable to analyze conversation. Response:{Environment.NewLine}{response.content}");
                }
                else
                {
                    return 0 == string.Compare("Succeeded", document.RootElement.Clone().GetProperty("status").ToString(), StringComparison.InvariantCultureIgnoreCase);
                }                
            }
        }

        private async Task WaitForConversationAnalysis(string conversationAnalysisUrl)
        {
            var done = false;
            while (!done)
            {
                Console.WriteLine($"Waiting {waitSeconds} seconds for conversation analysis to complete.");
                Thread.Sleep(waitSeconds * 1000);
                done = await GetConversationAnalysisStatus(conversationAnalysisUrl);
            }
            return;
        }

        private async Task<JsonElement> GetConversationAnalysis(string conversationAnalysisUrl)
        {
            var response = await RestHelper.SendGet(conversationAnalysisUrl, this.userConfig.languageSubscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            using (JsonDocument document = JsonDocument.Parse(response.content))
            {
                return document.RootElement.Clone();
            }
        }
        
        private ConversationAnalysisForSimpleOutput GetConversationAnalysisForSimpleOutput(JsonElement conversationAnalysis)
        {
            var tasks = conversationAnalysis.GetProperty("tasks").GetProperty("items").EnumerateArray();
            
            var summaryTask = tasks.First(task => 0 == String.Compare(task.GetProperty("taskName").ToString(), "summary_1", StringComparison.InvariantCultureIgnoreCase));
            var conversationSummary = summaryTask.GetProperty("results").GetProperty("conversations").EnumerateArray()
                .First().GetProperty("summaries").EnumerateArray()
                .Select(summary => (summary.GetProperty("aspect").ToString(), summary.GetProperty("text").ToString()))
                .ToArray();

            var PIITask = tasks.First(task => 0 == String.Compare(task.GetProperty("taskName").ToString(), "PII_1", StringComparison.InvariantCultureIgnoreCase));
            var conversationPIIAnalysis = PIITask.GetProperty("results").GetProperty("conversations").EnumerateArray()
                .First().GetProperty("conversationItems").EnumerateArray()
                .Select(document =>
                    {
                        return document.GetProperty("entities").EnumerateArray()
                            .Select(entity => (entity.GetProperty("category").ToString(), entity.GetProperty("text").ToString()))
                            .ToArray();
                    })
                .ToArray();
            
            return new ConversationAnalysisForSimpleOutput(conversationSummary, conversationPIIAnalysis);
        }

        // Create a string that contains each transcription phrase, followed by sentiment, PII, and so on.
        private String GetSimpleOutput(
            TranscriptionPhrase[] transcriptionPhrases,
            string[] transcriptionSentiments,
            ConversationAnalysisForSimpleOutput conversationAnalysis)
        {
            var result = new StringBuilder();
            for (var index = 0; index < transcriptionPhrases.Length; index++)
            {
                result.AppendLine($"Phrase: {transcriptionPhrases[index].text}");
                result.AppendLine($"Speaker: {transcriptionPhrases[index].speakerNumber}");
                if (index < transcriptionSentiments.Length)
                {
                    result.AppendLine($"Sentiment: {transcriptionSentiments[index]}");
                }
                if (index < conversationAnalysis.PIIAnalysis.Length)
                {
                    if (conversationAnalysis.PIIAnalysis[index].Length > 0)
                    {
                        var entities = conversationAnalysis.PIIAnalysis[index].Aggregate("",
                            (result, entity) => $"{result}    Category: {entity.category}. Text: {entity.text}.{Environment.NewLine}");
                        result.Append($"Recognized entities (PII):{Environment.NewLine}{entities}");
                    }
                    else
                    {
                        result.AppendLine($"Recognized entities (PII): none.");
                    }
                }
                result.AppendLine();
            }
            result.AppendLine(conversationAnalysis.summary.Aggregate($"Conversation summary:{Environment.NewLine}",
                (result, item) => $"{result}    Aspect: {item.aspect}. Summary: {item.summary}.{Environment.NewLine}"));
            return result.ToString();
        }

        private void PrintSimpleOutput(string outputFilePathValue, TranscriptionPhrase[] transcriptionPhrases, SentimentAnalysisResult[] sentimentAnalysisResults, JsonElement conversationAnalysis)
        {
            string[] sentiments = GetSentimentsForSimpleOutput(sentimentAnalysisResults);
            ConversationAnalysisForSimpleOutput conversation = GetConversationAnalysisForSimpleOutput(conversationAnalysis);
            File.WriteAllText(outputFilePathValue, GetSimpleOutput(transcriptionPhrases, sentiments, conversation));
        }

        private object GetConversationAnalysisForFullOutput(TranscriptionPhrase[] transcriptionPhrases, JsonElement conversationAnalysis)
        {
            // Get the conversation summary and conversation PII analysis task results.
            IEnumerable<JsonElement> tasks = conversationAnalysis.GetProperty("tasks").GetProperty("items").EnumerateArray();
            JsonElement conversationSummaryResults = tasks.First(task => 0 == String.Compare(task.GetProperty("taskName").ToString(), "summary_1", StringComparison.InvariantCultureIgnoreCase)).GetProperty("results");
            JsonElement conversationPiiResults = tasks.First(task => 0 == String.Compare(task.GetProperty("taskName").ToString(), "PII_1", StringComparison.InvariantCultureIgnoreCase)).GetProperty("results");
            // There should be only one conversation.
            JsonElement conversation = conversationPiiResults.GetProperty("conversations").EnumerateArray().First();
            // Order conversation items by ID so they match the order of the transcription phrases.
            IEnumerable<JsonElement> conversationItems = conversation.GetProperty("conversationItems").EnumerateArray().OrderBy(conversationItem => Int32.Parse(conversationItem.GetProperty("id").ToString()));
            var combinedRedactedContent = new CombinedRedactedContent[]{ new CombinedRedactedContent(0), new CombinedRedactedContent(1) };
            IEnumerable<JsonElement> conversationItems_2 = conversationItems.Select((conversationItem, index) =>
                {
                    // Get the channel and offset for this conversation item from the corresponding transcription phrase.
                    var channel = transcriptionPhrases[index].speakerNumber;
                    // Add channel and offset to conversation item JsonElement.
                    var conversationItem_2 = AddOrChangeValueInJsonElement<string>(conversationItem, "channel", channel.ToString());
                    var conversationItem_3 = AddOrChangeValueInJsonElement<string>(conversationItem_2, "offset", transcriptionPhrases[index].offset);
                    // Get the text, lexical, and itn fields from redacted content, and append them to the combined redacted content for this channel.
                    var redactedContent = conversationItem.GetProperty("redactedContent");
                    combinedRedactedContent[channel].display.AppendFormat("{0} ", redactedContent.GetProperty("text").ToString());
                    combinedRedactedContent[channel].lexical.AppendFormat("{0} ", redactedContent.GetProperty("lexical").ToString());
                    combinedRedactedContent[channel].itn.AppendFormat("{0} ", redactedContent.GetProperty("itn").ToString());
                    return conversationItem_3;
                }
            );
            // Update the conversationItems item in the conversation JsonElement.
            JsonElement conversation_2 = AddOrChangeValueInJsonElement<IEnumerable<JsonElement>>(conversation, "conversationItems", conversationItems_2);
            return new
            {
                conversationSummaryResults = conversationSummaryResults,
                conversationPiiResults = new
                {
                    combinedRedactedContent = combinedRedactedContent.Select(item =>
                        {
                            return new
                            {
                                channel = item.channel.ToString(),
                                display = item.display.ToString(),
                                itn = item.itn.ToString(),
                                lexical = item.lexical.ToString()
                            };
                        }
                    ),
                    conversations = new JsonElement[]{ conversation_2 }
                }
            };
        }

        private void PrintFullOutput(JsonElement transcription, JsonElement[] sentimentConfidenceScores, TranscriptionPhrase[] transcriptionPhrases, JsonElement conversationAnalysis)
        {
            var results = new
            {
                transcription = MergeSentimentConfidenceScoresIntoTranscription(transcription, sentimentConfidenceScores),
                conversationAnalyticsResults = GetConversationAnalysisForFullOutput(transcriptionPhrases, conversationAnalysis)
            };
            Console.WriteLine(JsonSerializer.Serialize(results, new JsonSerializerOptions() { WriteIndented = true, Encoder = JavaScriptEncoder.UnsafeRelaxedJsonEscaping }));
        }

        // Note: To pass command-line arguments, run:
        // dotnet run -- [args]
        // For example:
        // dotnet run -- --help
        public async Task Run()
        {
            // How to use batch transcription:
            // https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
            var transcriptionId = await CreateTranscription();
            await WaitForTranscription(transcriptionId);
            Console.WriteLine($"Transcription ID: {transcriptionId}");
            JsonElement transcriptionFiles = await GetTranscriptionFiles(transcriptionId);
            var transcriptionUri = GetTranscriptionUri(transcriptionFiles);
            JsonElement transcription = await GetTranscription(transcriptionUri);
            var transcriptionPhrases = GetTranscriptionPhrases(transcription);
            SentimentAnalysisResult[] sentimentAnalysisResults = GetSentimentAnalysis(transcriptionPhrases);
            JsonElement[] sentimentConfidenceScores = GetSentimentConfidenceScores(sentimentAnalysisResults);
            var conversationItems = TranscriptionPhrasesToConversationItems(transcriptionPhrases);
            // NOTE: Conversation summary is currently in gated public preview. You can sign up here:
            // https://aka.ms/applyforconversationsummarization/
            var conversationAnalysisUrl = await RequestConversationAnalysis(conversationItems);
            await WaitForConversationAnalysis(conversationAnalysisUrl);
            JsonElement conversationAnalysis = await GetConversationAnalysis(conversationAnalysisUrl);
            if (this.userConfig.outputFilePath is string outputFilePathValue)
            {
                PrintSimpleOutput(outputFilePathValue, transcriptionPhrases, sentimentAnalysisResults, conversationAnalysis);
            }
            PrintFullOutput(transcription, sentimentConfidenceScores, transcriptionPhrases, conversationAnalysis);
            // Clean up resources.
            Console.WriteLine("Deleting transcription.");
            await DeleteTranscription(transcriptionId);
            return;
        }

        public async static Task Main(string[] args)
        {
            string usage = @"USAGE: dotnet run -- [...]

  HELP
    --help                          Show this help and stop.

  CONNECTION
    --speechKey KEY                 Your Azure Speech service subscription key. Required.
    --speechRegion REGION           Your Azure Speech service region. Required.
                                    Examples: westus, eastus
    --languageKey KEY               Your Azure Cognitive Language subscription key. Required.
    --languageEndpoint ENDPOINT     Your Azure Cognitive Language endpoint. Required.

  LANGUAGE
    --language LANGUAGE             The language to use for sentiment analysis and conversation analysis.
                                    This should be a two-letter ISO 639-1 code.
                                    Default: en
    --locale LOCALE                 The locale to use for batch transcription of audio.
                                    Default: en-US

  INPUT
    --input URL                     Input audio from URL. Required.
    --stereo                        Use stereo audio format.
                                    If this is not present, mono is assumed.

  OUTPUT
    --output FILE                   Output phrase list and conversation summary to text file.
";
            
            if (args.Contains("--help"))
            {
                Console.WriteLine(usage);
            }
            else
            {
                await (new Program(args, usage)).Run();
            }
        }
    }
}

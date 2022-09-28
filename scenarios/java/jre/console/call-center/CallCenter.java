//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.stream.*;
import javax.net.ssl.HttpsURLConnection;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

/* To run:
javac HelperClasses.java -cp ".;target\dependency\*"
javac CallCenter.java -cp ".;target\dependency\*"
java -cp ".;target\dependency\*" CallCenter <parameters>
*/

public class CallCenter
{
    private UserConfig _userConfig;
    
    // This should not change unless you switch to a new version of the Speech REST API.
    final private static String speechTranscriptionPath = "/speechtotext/v3.0/transcriptions";

    // These should not change unless you switch to a new version of the Cognitive Language REST API.
    final private static String sentimentAnalysisPath = "/language/:analyze-text";
    final private static String sentimentAnalysisQuery = "?api-version=2022-05-01";
    final private static String conversationAnalysisPath = "/language/analyze-conversations/jobs";
    final private static String conversationAnalysisQuery = "?api-version=2022-05-15-preview";
    final public static String conversationSummaryModelVersion = "2022-05-15-preview";

    // How long to wait while polling batch transcription and conversation analysis status.
    final private static int waitSeconds = 10;

    // Split list *xs* into chunks of size *size*.
    private static <T> List<List<T>> Chunk(List<T> xs, int size)
    {
        return IntStream.range(0, xs.size())
            // Map the index to a tuple of the index and the item at that index.
            .mapToObj(i -> new AbstractMap.SimpleEntry<Integer, T>(i, xs.get(i)))
            // Group the tuples by index. For example, for size 10, group them into indices 0-9, 10-19, and so on.
            .collect(Collectors.groupingBy(x -> x.getKey() / size))
            // Discard the group keys.
            .values()
            .stream()
            // For each group (chunk), discard the indices from the tuples. Convert each chunk from a stream back to a list.
            .map(chunk -> chunk.stream().map(x -> x.getValue()).collect(Collectors.toList()))
            // Convert the stream of chunks to a list.
            .collect(Collectors.toList());
    }

    private void Initialize(UserConfig userConfig) throws IOException
    {
        _userConfig = userConfig;

        if (_userConfig.getOutputFilePath().isPresent())
        {
            var outputFile = new File(_userConfig.getOutputFilePath().get());
            if (outputFile.exists())
            {
                outputFile.delete();
            }
        }
    }

    private String CreateTranscription(String inputAudioURL) throws Exception
    {
        // Create Transcription REST API request and response JSON sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
        // Notes:
        // - locale and displayName are required.
        // - diarizationEnabled should only be used with mono audio input.
        String uri = _userConfig.getSpeechEndpoint().get() + speechTranscriptionPath;
        var request = new TranscriptionRequest(
            new String[] { inputAudioURL },
            new TranscriptionRequestProperties(!_userConfig.getUseStereoAudio(), "PT30M"),
            _userConfig.getLocale(),
            String.format("call_center_%s", java.time.Instant.now())
        );
        RestResult result = RestHelper.sendPost(uri, new Gson().toJson(request), _userConfig.getSpeechSubscriptionKey().get(), new int[] { HttpURLConnection.HTTP_CREATED });
        String transcriptionUri_1 = result.getJson().get("self").getAsString();
        // The transcription ID is at the end of the transcription URI.
        String[] transcriptionUri_2 = transcriptionUri_1.split("/");
        String transcriptionId = transcriptionUri_2[transcriptionUri_2.length - 1];
        // Verify the transcription ID is a valid GUID.
        try
        {
            UUID uuid = UUID.fromString(transcriptionId);
        }
        catch (IllegalArgumentException exception)
        {
            throw new Exception(String.format("Unable to parse response from Create Transcription API:%s%s", System.lineSeparator(), result.getText()));
        }        
        return transcriptionId;
    }
    
    private boolean GetTranscriptionStatus(String transcriptionId) throws Exception
    {
        // Get Transcription REST API request and response JSON sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscription
        String uri = _userConfig.getSpeechEndpoint().get() + String.format("%s/%s", speechTranscriptionPath, transcriptionId);
        RestResult result = RestHelper.sendGet(uri, _userConfig.getSpeechSubscriptionKey().get(), new int[] { HttpURLConnection.HTTP_OK });
        String status = result.getJson().get("status").getAsString().toLowerCase();
        if (status.equals("failed"))
        {
            throw new Exception(String.format("Unable to transcribe audio input. Response:%s%s", System.lineSeparator(), result.getText()));
        }
        else
        {
            return status.equals("succeeded");
        }
    }

    private void WaitForTranscription(String transcriptionId) throws Exception
    {
        boolean done = false;
        while (!done)
        {
            System.out.println(String.format("Waiting %d seconds for transcription to complete.", waitSeconds));
            Thread.sleep(waitSeconds * 1000);
            done = this.GetTranscriptionStatus(transcriptionId);
        }
        return;
    }

    private RestResult GetTranscriptionFiles(String transcriptionId) throws Exception
    {
        String uri = _userConfig.getSpeechEndpoint().get() + String.format("%s/%s/files", speechTranscriptionPath, transcriptionId);
        return RestHelper.sendGet(uri, _userConfig.getSpeechSubscriptionKey().get(), new int[] { HttpURLConnection.HTTP_OK });        
    }

    private String GetTranscriptionUri(RestResult transcriptionFiles) throws Exception
    {
        Optional<String> contentUri = Optional.empty();
        // Get Transcription Files REST API request and response JSON sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
        Iterator<JsonElement> iterator = transcriptionFiles.getJson().getAsJsonArray("values").iterator();
        while (iterator.hasNext())
        {
            JsonObject value = (JsonObject)iterator.next().getAsJsonObject();
            if (value.get("kind").getAsString().toLowerCase().equals("transcription"))
            {
                contentUri = Optional.of(value.getAsJsonObject("links").get("contentUrl").getAsString());
                break;
            }
        }
        if (!contentUri.isPresent())
        {
            throw new Exception (String.format("Unable to parse response from Get Transcription Files API:%s%s", System.lineSeparator(), transcriptionFiles.getText()));
        }
        return contentUri.get();
    }

    private JsonObject GetTranscription(String transcriptionUri) throws Exception
    {
        RestResult result = RestHelper.sendGet(transcriptionUri, "", new int[] { HttpURLConnection.HTTP_OK });
        return result.getJson();
    }

    private List<TranscriptionPhrase> GetTranscriptionPhrases(JsonObject transcription) throws Exception
    {
        var phrases = transcription.getAsJsonArray("recognizedPhrases");
        return IntStream.range(0, phrases.size()).mapToObj(index ->
        {
            JsonObject phrase = phrases.get(index).getAsJsonObject();
            JsonObject best = phrase.getAsJsonArray("nBest").get(0).getAsJsonObject();
            // If the user specified stereo audio, and therefore we turned off diarization,
            // the speaker number is replaced by a channel number.
            // If the user specified stereo audio, and therefore we turned off diarization,
            // only the channel property is present.
            // Note: Channels are numbered from 0. Speakers are numbered from 1.
            int speakerNumber = 0;
            if (phrase.has("speaker"))
            {
                speakerNumber = phrase.get("speaker").getAsInt();
            }
            else if (phrase.has("channel"))
            {
                speakerNumber = phrase.get("channel").getAsInt();
            }
            
            return new TranscriptionPhrase(index, best.get("display").getAsString(), best.get("itn").getAsString(), best.get("lexical").getAsString(), speakerNumber, phrase.get("offset").getAsString(), phrase.get("offsetInTicks").getAsDouble());
        })
        .collect(Collectors.toList());
    }

    private void DeleteTranscription(String transcriptionId) throws Exception
    {
        String uri = _userConfig.getSpeechEndpoint().get() + String.format("%s/%s", speechTranscriptionPath, transcriptionId);
        RestHelper.sendDelete(uri, _userConfig.getSpeechSubscriptionKey().get(), new int[] { HttpURLConnection.HTTP_NO_CONTENT } );
        return;
    }

    private List<SentimentAnalysisResult> GetSentimentAnalysis(List<TranscriptionPhrase> phrases) throws Exception
    {
        // Analyze Text REST API request and response JSON sample and schema:
        // https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeText
        String uri = String.format("%s%s/%s", _userConfig.getLanguageEndpoint(), sentimentAnalysisPath, sentimentAnalysisQuery);
        // Convert each transcription phrase to a "document" as expected by the Analyze Text REST API.
        List<SentimentRequestDocument> documentsToSend_1 = IntStream.range(0, phrases.size())
            .mapToObj(index -> new SentimentRequestDocument (index, _userConfig.getLanguage(), phrases.get(index).text))
            .collect(Collectors.toList());
        // We can only analyze sentiment for 10 documents per request.
        List<List<SentimentRequestDocument>> documentsToSend_2 = Chunk(documentsToSend_1, 10);
        // We can't call methods that throw exceptions (in this case,
        // sendPost) inside a lambda, which means we can't use map here.
        var index = 0;
        List<SentimentAnalysisResult> retval = new ArrayList<SentimentAnalysisResult>();
        for (List<SentimentRequestDocument> documentsToSend_3 : documentsToSend_2)
        {
            RestResult result = RestHelper.sendPost(uri, new Gson().toJson(new SentimentRequest(documentsToSend_3)), _userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
            for (JsonElement element : result.getJson().getAsJsonObject("results").getAsJsonArray("documents"))
            {
                var phrase = phrases.get(index++);
                retval.add(new SentimentAnalysisResult(phrase.speakerNumber, phrase.offsetInTicks, element.getAsJsonObject()));
            }
        }
        return retval;
    }

    private List<String> GetSentimentsForSimpleOutput(List<SentimentAnalysisResult> sentimentAnalysisResults) throws Exception
    {
        return sentimentAnalysisResults.stream()
        .sorted(Comparator.comparing(result -> result.offsetInTicks))
        .map(result -> result.document.getAsJsonObject().get("sentiment").getAsString())
        .collect(Collectors.toList());
    }

    private List<JsonObject> GetSentimentConfidenceScores(List<SentimentAnalysisResult> sentimentAnalysisResults) throws Exception
    {
        return sentimentAnalysisResults.stream()
        .sorted(Comparator.comparing(result -> result.offsetInTicks))
        .map(result -> result.document.getAsJsonObject().getAsJsonObject("confidenceScores"))
        .collect(Collectors.toList());
    }

    private JsonObject MergeSentimentConfidenceScoresIntoTranscription(JsonObject transcription, List<JsonObject> sentimentConfidenceScores)
    {
        for (var index = 0; index < transcription.getAsJsonArray("recognizedPhrases").size(); index++)
        {
            var phrase = transcription.getAsJsonArray("recognizedPhrases").get(index);
            for (var best_item : phrase.getAsJsonObject().getAsJsonArray("nBest"))
            {
                // Add the sentiment confidence scores to the JsonElement in the nBest array.
                // TODO2 We are adding the same sentiment data to each nBest item.
                // However, the sentiment data are based on the phrase from the first nBest item.
                // See GetTranscriptionPhrases() and GetSentimentAnalysis().
                best_item.getAsJsonObject().add("sentiment", sentimentConfidenceScores.get(index));
            }
        }
        return transcription;
    }

    private List<ConversationItem> TranscriptionPhrasesToConversationItems(List<TranscriptionPhrase> phrases)
    {
        return IntStream.range(0, phrases.size())
            .mapToObj(index ->
            {
                var phrase = phrases.get(index);
                return new ConversationItem(
                phrase.id,
                phrase.text,
                phrase.itn,
                phrase.lexical,
                // The first person to speak is probably the agent.
                (0 == phrase.speakerNumber) ? "Agent" : "Customer",
                phrase.speakerNumber);
            })
            .collect(Collectors.toList());
    }

    private String RequestConversationAnalysis(List<ConversationItem> conversationItems) throws Exception
    {
        // Analyze Conversation REST API request and response JSON sample and schema:
        // https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeConversation_SubmitJob
        String uri = _userConfig.getLanguageEndpoint() + String.format("%s/%s", conversationAnalysisPath, conversationAnalysisQuery);
        Conversation conversation = new Conversation("conversation1", _userConfig.getLanguage(), "transcript", conversationItems);
        Conversations analysisInput = new Conversations(new ArrayList<Conversation>(Arrays.asList(conversation)));
        ConversationalSummarizationTaskParameters task_1_parameters = new ConversationalSummarizationTaskParameters(new ArrayList<String>(Arrays.asList("Issue", "Resolution")));
        ConversationalSummarizationTask task1 = new ConversationalSummarizationTask("summary_1", task_1_parameters);
        ConversationalPIITaskParameters task_2_parameters = new ConversationalPIITaskParameters(new ArrayList<String>(Arrays.asList("All")), false, "text", false);
        ConversationalPIITask task2 = new ConversationalPIITask("PII_1", task_2_parameters);
        ConversationAnalysisRequest request = new ConversationAnalysisRequest(String.format("call_center_%s", java.time.LocalDateTime.now()), analysisInput, new ArrayList<ConversationAnalysisRequestTask>(Arrays.asList(task1, task2)));
        RestResult result = RestHelper.sendPost(uri, new Gson().toJson(request), _userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_ACCEPTED });
        return result.getHeaders().get("operation-location").get(0);
    }

    private boolean GetConversationAnalysisStatus(String conversationAnalysisUrl) throws Exception
    {
        // Get Conversation Analysis Status and Results REST API request and response JSON sample and schema:
        // https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeConversation_JobStatus
        RestResult result = RestHelper.sendGet(conversationAnalysisUrl, _userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
        String status = result.getJson().get("status").getAsString().toLowerCase();
        if (status.equals("failed"))
        {
            throw new Exception(String.format("Unable to analyze conversation. Response:%s%s", System.lineSeparator(), result.getText()));
        }
        else
        {
            return status.equals("succeeded");
        }
    }

    private void WaitForConversationAnalysis(String conversationAnalysisUrl) throws Exception
    {
        boolean done = false;
        while (!done)
        {
            System.out.println(String.format("Waiting %d seconds for conversation analysis to complete.", waitSeconds));
            Thread.sleep(waitSeconds * 1000);
            done = this.GetConversationAnalysisStatus(conversationAnalysisUrl);
        }
        return;
    }

    private RestResult GetConversationAnalysis(String conversationAnalysisUrl) throws Exception
    {
        return RestHelper.sendGet(conversationAnalysisUrl, _userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
    }

    private ConversationAnalysisForSimpleOutput GetConversationAnalysisForSimpleOutput(RestResult conversationAnalysis) throws Exception
    {
        Optional<JsonObject> summaryTask = Optional.empty();
        Optional<JsonObject> PIITask = Optional.empty();
        Iterator<JsonElement> iterator = conversationAnalysis.getJson().get("tasks").getAsJsonObject().getAsJsonArray("items").iterator();
        while (iterator.hasNext())
        {
            JsonObject value = (JsonObject)iterator.next().getAsJsonObject();
            if (value.get("taskName").getAsString().equals("summary_1"))
            {
                summaryTask = Optional.of(value);
            }
            else if (value.get("taskName").getAsString().equals("PII_1"))
            {
                PIITask = Optional.of(value);
            }
        }
        if (!summaryTask.isPresent() || !PIITask.isPresent())
        {
            throw new Exception (String.format("Unable to parse response from Get Conversation Analysis API:%s%s", System.lineSeparator(), conversationAnalysis.getText()));
        }
        Spliterator<JsonElement> summaries = summaryTask.get().get("results").getAsJsonObject().getAsJsonArray("conversations")
            .get(0).getAsJsonObject().getAsJsonArray("summaries").spliterator();
        List<ConversationAspectAndSummary> summary = 
            StreamSupport.stream(summaries, false)
            .map(aspectAndSummary -> new ConversationAspectAndSummary(aspectAndSummary.getAsJsonObject().get("aspect").getAsString(), aspectAndSummary.getAsJsonObject().get("text").getAsString()))
            .collect(Collectors.toList());
        Spliterator<JsonElement> conversationItems = PIITask.get().get("results").getAsJsonObject().getAsJsonArray("conversations")
            .get(0).getAsJsonObject().getAsJsonArray("conversationItems").spliterator();
        List<List<PIICategoryAndText>> PIIAnalysis =
            StreamSupport.stream(conversationItems, false)
            .map(document ->
            {
                Spliterator<JsonElement> entities = document.getAsJsonObject().getAsJsonArray("entities").spliterator();
                return StreamSupport.stream(entities, false)
                .map(entity -> new PIICategoryAndText(entity.getAsJsonObject().get("category").getAsString(), entity.getAsJsonObject().get("text").getAsString()))
                .collect(Collectors.toList());
            })
            .collect(Collectors.toList());
        return new ConversationAnalysisForSimpleOutput(summary, PIIAnalysis);
    }

    private String GetSimpleOutput(
        List<TranscriptionPhrase> phrases,
        List<String> sentiments,
        ConversationAnalysisForSimpleOutput conversationAnalysis)
    {
        var result = new StringBuilder();
        for (var index = 0; index < phrases.size(); index++)
        {
            result.append(String.format("Phrase: %s%s", phrases.get(index).text, System.lineSeparator()));
            result.append(String.format("Speaker: %d%s", phrases.get(index).speakerNumber, System.lineSeparator()));
            if (index < sentiments.size())
            {
                result.append(String.format("Sentiment: %s%s", sentiments.get(index), System.lineSeparator()));
            }
            if (index < conversationAnalysis.PIIAnalysis.size())
            {
                if (conversationAnalysis.PIIAnalysis.get(index).size() > 0)
                {
                    // reduce() needs a combiner when converting type. See:
                    // https://stackoverflow.com/questions/24308146/why-is-a-combiner-needed-for-reduce-method-that-converts-type-in-java-8
                    String entities = conversationAnalysis.PIIAnalysis.get(index).stream().reduce("", (acc, entity) -> String.format("%s    Category: %s. Text: %s.%s", acc, entity.category, entity.text, System.lineSeparator()), (s1, s2) -> s1 + s2);
                    result.append(String.format("Recognized entities (PII):%s%s", System.lineSeparator(), entities));
                }
                else
                {
                    result.append(String.format("Recognized entities (PII): none.%s", System.lineSeparator()));
                }
            }
            result.append(System.lineSeparator());
        }
        result.append(conversationAnalysis.summary.stream().reduce(String.format("Conversation summary:%s", System.lineSeparator()),
            (acc, item) -> String.format("%s    %s: %s.%s", acc, item.aspect, item.summary, System.lineSeparator()), (s1, s2) -> s1 + s2));
        return result.toString();
    }

    private void PrintSimpleOutput(List<TranscriptionPhrase> transcriptionPhrases, List<SentimentAnalysisResult> sentimentAnalysisResults, RestResult conversationAnalysis) throws Exception
    {
        List<String> sentiments = GetSentimentsForSimpleOutput(sentimentAnalysisResults);
        ConversationAnalysisForSimpleOutput conversation = GetConversationAnalysisForSimpleOutput(conversationAnalysis);
        System.out.println(GetSimpleOutput(transcriptionPhrases, sentiments, conversation));
    }

    private CombinedResults GetConversationAnalysisForFullOutput(List<TranscriptionPhrase> transcriptionPhrases, RestResult conversationAnalysis) throws Exception
    {
        // Get the conversation summary and conversation PII analysis task results.
        Optional<JsonObject> summaryTask = Optional.empty();
        Optional<JsonObject> PIITask = Optional.empty();
        Iterator<JsonElement> iterator = conversationAnalysis.getJson().get("tasks").getAsJsonObject().getAsJsonArray("items").iterator();
        while (iterator.hasNext())
        {
            JsonObject value = (JsonObject)iterator.next().getAsJsonObject();
            if (value.get("taskName").getAsString().equals("summary_1"))
            {
                summaryTask = Optional.of(value);
            }
            else if (value.get("taskName").getAsString().equals("PII_1"))
            {
                PIITask = Optional.of(value);
            }
        }
        if (!summaryTask.isPresent() || !PIITask.isPresent())
        {
            throw new Exception (String.format("Unable to parse response from Get Conversation Analysis API:%s%s", System.lineSeparator(), conversationAnalysis.getText()));
        }
        // There should be only one conversation.
        JsonObject conversation = PIITask.get().get("results").getAsJsonObject().getAsJsonArray("conversations").get(0).getAsJsonObject();
        Spliterator<JsonElement> conversationItems_1 = conversation.getAsJsonArray("conversationItems").spliterator();
        // Order conversation items by ID so they match the order of the transcription phrases.
        List<JsonObject> conversationItems_2 = StreamSupport.stream(conversationItems_1, false)
            .map(conversationItem -> conversationItem.getAsJsonObject())
            .sorted(Comparator.comparing(conversationItem -> conversationItem.get("id").getAsInt()))
            .collect(Collectors.toList());
        List<CombinedRedactedContent> combinedRedactedContent_1 = new ArrayList<CombinedRedactedContent>(Arrays.asList(new CombinedRedactedContent(0), new CombinedRedactedContent(1)));
        for (var index = 0; index < conversationItems_2.size(); index++)
        {
            // Get the channel and offset for this conversation item from the corresponding transcription phrase.
            var channel = transcriptionPhrases.get(index).speakerNumber;
            // Add channel and offset to conversation item JsonElement.
            var conversationItem = conversationItems_2.get(index);
            conversationItem.addProperty("channel", channel);
            conversationItem.addProperty("offset", transcriptionPhrases.get(index).offset);
            // Get the text, lexical, and itn fields from redacted content, and append them to the combined redacted content for this channel.
            var redactedContent = conversationItem.get("redactedContent").getAsJsonObject();
            combinedRedactedContent_1.get(channel).display.append(String.format("%s ", redactedContent.get("text").getAsString()));
            combinedRedactedContent_1.get(channel).lexical.append(String.format("%s ", redactedContent.get("lexical").getAsString()));
            combinedRedactedContent_1.get(channel).itn.append(String.format("%s ", redactedContent.get("itn").getAsString()));
        }
        JsonArray combinedRedactedContent_2 = new JsonArray();
        for (var index = 0; index < combinedRedactedContent_1.size(); index++)
        {
            CombinedRedactedContent content_1 = combinedRedactedContent_1.get(index);
            JsonObject content_2 = new JsonObject();
            content_2.addProperty("channel", content_1.channel);
            content_2.addProperty("display", content_1.display.toString());
            content_2.addProperty("itn", content_1.itn.toString());
            content_2.addProperty("lexical", content_1.lexical.toString());
            combinedRedactedContent_2.add(content_2);
        }
        JsonObject conversationPiiResults = new JsonObject();
        conversationPiiResults.add("combinedRedactedContent", combinedRedactedContent_2);
        JsonArray conversations = new JsonArray();
        conversations.add(conversation);
        conversationPiiResults.add("conversations", conversations);
        return new CombinedResults(summaryTask.get().get("results").getAsJsonObject(), conversationPiiResults);
    }

    private void PrintFullOutput(String outputFilePath, JsonObject transcription, List<JsonObject> sentimentConfidenceScores, List<TranscriptionPhrase> transcriptionPhrases, RestResult conversationAnalysis) throws Exception, IOException
    {
        var results = new FullOutput(MergeSentimentConfidenceScoresIntoTranscription(transcription, sentimentConfidenceScores), GetConversationAnalysisForFullOutput(transcriptionPhrases, conversationAnalysis));
        Gson gson = new GsonBuilder().disableHtmlEscaping().setPrettyPrinting().create();
        var outputFile = new FileWriter(_userConfig.getOutputFilePath().get(), true);
        outputFile.write(gson.toJson(results));
        outputFile.close();
    }

    public static void main(String[] args) throws Exception
    {
        final var usage = String.join(System.lineSeparator(),
            "Usage: java -cp .;target\\dependency\\* CallCenter [...]",
            "",
            "  HELP",
            "    --help                        Show this help and stop.",
            "",
            "  CONNECTION",
            "    --speechKey KEY                 Your Azure Speech service subscription key. Required unless --jsonInput is present.",
            "    --speechRegion REGION           Your Azure Speech service region. Required unless --jsonInput is present.",
            "                                    Examples: westus, eastus",
            "    --languageKey KEY               Your Azure Cognitive Language subscription key. Required.",
            "    --languageEndpoint ENDPOINT     Your Azure Cognitive Language endpoint. Required.",
            "",
            "  LANGUAGE",
            "    --language LANGUAGE             The language to use for sentiment analysis and conversation analysis.",
            "                                    This should be a two-letter ISO 639-1 code.",
            "                                    Default: en",
            "    --locale LOCALE                 The locale to use for batch transcription of audio.",
            "                                    Default: en-US",
            "",
            "  INPUT",
            "    --input URL                     Input audio from URL. Required unless --jsonInput is present.",
            "    --jsonInput FILE                Input JSON Speech batch transcription result from FILE. Overrides --input.",
            "    --stereo                        Use stereo audio format.",
            "                                    If this is not present, mono is assumed.",
            "  OUTPUT",
            "    --output FILE                   Output phrase list and conversation summary to text file."
        );

        try
        {
            List<String> argsList = Arrays.stream(args).collect(Collectors.toList());
            if (argsList.contains("--help"))
            {
                System.out.println(usage);
            }
            else
            {
                final var callCenter = new CallCenter();
                final UserConfig userConfig = UserConfig.UserConfigFromArgs(argsList, usage);
                callCenter.Initialize(userConfig);
                
                Optional<JsonObject> transcription = Optional.empty();
                if (userConfig.getInputFilePath().isPresent())
                {
                    var json = new String(Files.readAllBytes(Paths.get(userConfig.getInputFilePath().get())));
                    transcription = Optional.of(JsonParser.parseString(json).getAsJsonObject());
                }
                else if (userConfig.getInputAudioURL().isPresent())
                {
                    // How to use batch transcription:
                    // https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
                    final String transcriptionId = callCenter.CreateTranscription(userConfig.getInputAudioURL().get());
                    System.out.println(String.format("Transcription ID: %s", transcriptionId));
                    callCenter.WaitForTranscription(transcriptionId);
                    final RestResult transcriptionFiles = callCenter.GetTranscriptionFiles(transcriptionId);
                    final String transcriptionUri = callCenter.GetTranscriptionUri(transcriptionFiles);
                    System.out.println(String.format("Transcription URI: %s", transcriptionUri));
                    transcription = Optional.of(callCenter.GetTranscription(transcriptionUri));
                }
                // For stereo audio, the phrases are sorted by channel number, so resort them by offset.
                Spliterator<JsonElement> unsorted_phrases = transcription.get().getAsJsonArray("recognizedPhrases").spliterator();
                var sorted_phrases_1 = StreamSupport.stream(unsorted_phrases, false)
                    .sorted(Comparator.comparing(phrase -> phrase.getAsJsonObject().get("offsetInTicks").getAsDouble()))
                    .collect(Collectors.toList());
                var sorted_phrases_2 = new JsonArray(sorted_phrases_1.size());
                for (var phrase : sorted_phrases_1)
                {
                    sorted_phrases_2.add(phrase);
                }
                transcription.get().add("recognizedPhrases", sorted_phrases_2);
                
                final List<TranscriptionPhrase> phrases = callCenter.GetTranscriptionPhrases(transcription.get());
                final List<SentimentAnalysisResult> sentimentAnalysisResults = callCenter.GetSentimentAnalysis(phrases);
                final List<JsonObject> sentimentConfidenceScores = callCenter.GetSentimentConfidenceScores(sentimentAnalysisResults);
                final List<ConversationItem> conversationItems = callCenter.TranscriptionPhrasesToConversationItems(phrases);
                // NOTE: Conversation summary is currently in gated public preview. You can sign up here:
                // https://aka.ms/applyforconversationsummarization/
                final String conversationAnalysisUrl = callCenter.RequestConversationAnalysis(conversationItems);
                callCenter.WaitForConversationAnalysis(conversationAnalysisUrl);
                final RestResult conversationAnalysis = callCenter.GetConversationAnalysis(conversationAnalysisUrl);
                callCenter.PrintSimpleOutput(phrases, sentimentAnalysisResults, conversationAnalysis);
                if (userConfig.getOutputFilePath().isPresent())
                {
                    callCenter.PrintFullOutput(userConfig.getOutputFilePath().get(), transcription.get(), sentimentConfidenceScores, phrases, conversationAnalysis);
                }
            }
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
        }
    }
}

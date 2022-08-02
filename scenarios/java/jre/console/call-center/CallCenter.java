//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.*;
import java.net.*;
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
javac CallCenter.java -cp ".;target\dependency\*"
java -cp ".;target\dependency\*" CallCenter
*/

// TODO1 Convert to var.

public class CallCenter {
    private UserConfig m_userConfig;
    
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
    private static <T> List<List<T>> Chunk(List<T> xs, int size) {
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
        m_userConfig = userConfig;

        if (m_userConfig.getOutputFilePath().isPresent())
        {
            var outputFile = new File(m_userConfig.getOutputFilePath().get());
            if (outputFile.exists())
            {
                outputFile.delete();
            }
        }
    }

    private String CreateTranscription(String uriToTranscribe) throws Exception {
        // Create Transcription REST API request and response JSON sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
        // Notes:
        // - locale and displayName are required.
        // - diarizationEnabled should only be used with mono audio input.
        String uri = m_userConfig.getSpeechEndpoint() + speechTranscriptionPath;
        JsonObject properties = new JsonObject();
        properties.addProperty("diarizationEnabled", !m_userConfig.getUseStereoAudio());
        var request = new TranscriptionRequest(
            new String[] { uriToTranscribe },
            properties,
            m_userConfig.getLocale(),
            String.format("call_center_%s", java.time.LocalDateTime.now())
        );
        RestResult result = RestHelper.sendPost(uri, new Gson().toJson(request), m_userConfig.getSpeechSubscriptionKey(), new int[] { HttpURLConnection.HTTP_CREATED });
        String transcriptionUri_1 = result.getJson().get("self").getAsString();
        // The transcription ID is at the end of the transcription URI.
        String[] transcriptionUri_2 = transcriptionUri_1.split("/");
        String transcriptionId = transcriptionUri_2[transcriptionUri_2.length - 1];
        // Verify the transcription ID is a valid GUID.
        try {
            UUID uuid = UUID.fromString(transcriptionId);
        } catch (IllegalArgumentException exception) {
            throw new Exception(String.format("Unable to parse response from Create Transcription API:%s%s", System.lineSeparator(), result.getText()));
        }        
        return transcriptionId;
    }
    
    private boolean GetTranscriptionStatus(String transcriptionId) throws Exception {
        // Get Transcription REST API request and response JSON sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscription
        String uri = m_userConfig.getSpeechEndpoint() + String.format("%s/%s", speechTranscriptionPath, transcriptionId);
        RestResult result = RestHelper.sendGet(uri, m_userConfig.getSpeechSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
        String status = result.getJson().get("status").getAsString().toLowerCase();
        if (status.equals("failed")) {
            throw new Exception(String.format("Unable to transcribe audio input. Response:%s%s", System.lineSeparator(), result.getText()));
        }
        else {
            return status.equals("succeeded");
        }
    }

    private void WaitForTranscription(String transcriptionId) throws Exception {
        boolean done = false;
        while (!done) {
            System.out.println(String.format("Waiting %d seconds for transcription to complete.", waitSeconds));
            Thread.sleep(waitSeconds * 1000);
            done = this.GetTranscriptionStatus(transcriptionId);
        }
        return;
    }

    private RestResult GetTranscriptionFiles(String transcriptionId) throws Exception {
        String uri = m_userConfig.getSpeechEndpoint() + String.format("%s/%s/files", speechTranscriptionPath, transcriptionId);
        return RestHelper.sendGet(uri, m_userConfig.getSpeechSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });        
    }

    private String GetTranscriptionUri(RestResult transcriptionFiles) throws Exception {
        Optional<String> contentUri = Optional.empty();
        // Get Transcription Files REST API request and response JSON sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
        Iterator<JsonElement> iterator = transcriptionFiles.getJson().getAsJsonArray("values").iterator();
        while (iterator.hasNext()) {
            JsonObject value = (JsonObject)iterator.next().getAsJsonObject();
            if (value.get("kind").getAsString().equals("Transcription")) {
                contentUri = Optional.of(value.getAsJsonObject("links").get("contentUrl").getAsString());
                break;
            }
        }
        if (!contentUri.isPresent()) {
            throw new Exception (String.format("Unable to parse response from Get Transcription Files API:%s%s", System.lineSeparator(), transcriptionFiles.getText()));
        }
        return contentUri.get();
    }

    private JsonObject GetTranscription(String transcriptionUri) throws Exception {
        RestResult result = RestHelper.sendGet(transcriptionUri, "", new int[] { HttpURLConnection.HTTP_OK });
        return result.getJson();
    }

    private List<PhraseAndSpeaker> GetTranscriptionPhrasesAndSpeakers(JsonObject transcription) throws Exception {
        Spliterator<JsonElement> spliterator = transcription.getAsJsonArray("recognizedPhrases").spliterator();        
        Stream<PhraseAndSpeaker> phrases = StreamSupport.stream(spliterator, false).map(phrase -> {
            JsonElement best = phrase.getAsJsonObject().getAsJsonArray("nBest").get(0);
            // If the user specified stereo audio, and therefore we turned off diarization,
            // the speaker number is replaced by a channel number.
            var speakerNumber = m_userConfig.getUseStereoAudio() ? phrase.getAsJsonObject().get("channel").getAsInt() : phrase.getAsJsonObject().get("speaker").getAsInt();
            return new PhraseAndSpeaker(best.getAsJsonObject().get("display").getAsString(), speakerNumber, phrase.getAsJsonObject().get("offsetInTicks").getAsDouble());
        });
        return (m_userConfig.getUseStereoAudio()
            // For stereo audio, the phrases are sorted by channel number, so resort them by offset.
            ? phrases.sorted(Comparator.comparing(phrase -> phrase.offset))
            : phrases)
            .collect(Collectors.toList());
    }

    private void DeleteTranscription(String transcriptionId) throws Exception {
        String uri = m_userConfig.getSpeechEndpoint() + String.format("%s/%s", speechTranscriptionPath, transcriptionId);
        RestHelper.sendDelete(uri, m_userConfig.getSpeechSubscriptionKey(), new int[] { HttpURLConnection.HTTP_NO_CONTENT } );
        return;
    }

    private JsonObject GetSentimentAnalysis(List<PhraseAndSpeaker> phrasesAndSpeakers) throws Exception {
        // Analyze Text REST API request and response JSON sample and schema:
        // https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeText
        String uri = m_userConfig.getLanguageEndpoint() + String.format("%s/%s", sentimentAnalysisPath, sentimentAnalysisQuery);
        // Convert each transcription phrase to a "document" as expected by the Analyze Text REST API.
        // Include a counter to use as a document ID.
        List<Document> documentsToSend_1 = IntStream.range(0, phrasesAndSpeakers.size())
            // Use mapToObj because we are converting from int to Document.
            .mapToObj(i -> new Document (i, m_userConfig.getLanguage(), phrasesAndSpeakers.get(i).phrase))
            .collect(Collectors.toList());
        // We can only analyze sentiment for 10 documents per request.
        List<List<Document>> documentsToSend_2 = Chunk(documentsToSend_1, 10);
        // We can't call methods that throw exceptions inside a lambda, which means we can't use map.
        JsonArray documents = new JsonArray();
        for (List<Document> documentsToSend_3 : documentsToSend_2) {
            RestResult result = RestHelper.sendPost(uri, new Gson().toJson(new SentimentRequest(documentsToSend_3)), m_userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
            documents.addAll(result.getJson().getAsJsonObject("results").getAsJsonArray("documents"));
        }
        return new Gson().toJsonTree(new SentimentResponse(new SentimentResults(documents))).getAsJsonObject();
    }

    private List<String> GetSentiments(JsonObject sentimentAnalysis) throws Exception {
        return StreamSupport.stream(sentimentAnalysis.getAsJsonObject("results").getAsJsonArray("documents").spliterator(), false)
        .map(document -> new Sentiment(document.getAsJsonObject().get("id").getAsInt(), document.getAsJsonObject().get("sentiment").getAsString()))
        .sorted(Comparator.comparing(document -> document.id))
        .map(document -> document.sentiment)
        .collect(Collectors.toList());
    }

    private List<ConversationItem> TranscriptionPhrasesToConversationItems(List<PhraseAndSpeaker> phrasesAndSpeakers)
    {
        // Include a counter to use as a document ID.
        return IntStream.range(0, phrasesAndSpeakers.size())
            .mapToObj(i -> new ConversationItem(
                i,
                phrasesAndSpeakers.get(i).phrase,
                // The first person to speak is probably the agent.
                (1 == phrasesAndSpeakers.get(i).speakerNumber) ? "Agent" : "Customer",
                phrasesAndSpeakers.get(i).speakerNumber
            ))
            .collect(Collectors.toList());
    }

    private String RequestConversationAnalysis(List<ConversationItem> conversationItems) throws Exception
    {
        // Analyze Conversation REST API request and response JSON sample and schema:
        // https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeConversation_SubmitJob
        String uri = m_userConfig.getLanguageEndpoint() + String.format("%s/%s", conversationAnalysisPath, conversationAnalysisQuery);
        Conversation conversation = new Conversation("conversation1", m_userConfig.getLanguage(), "text", conversationItems);
        Conversations analysisInput = new Conversations(new ArrayList<Conversation>(Arrays.asList(conversation)));
        ConversationalSummarizationTaskParameters parameters = new ConversationalSummarizationTaskParameters(new ArrayList<String>(Arrays.asList("Issue", "Resolution")));
        ConversationalSummarizationTask task1 = new ConversationalSummarizationTask("summary_1", parameters);
        ConversationAnalysisRequestTask task2 = new ConversationAnalysisRequestTask("PII_1", "ConversationalPIITask");
        ConversationAnalysisRequest request = new ConversationAnalysisRequest(String.format("call_center_%s", java.time.LocalDateTime.now()), analysisInput, new ArrayList<ConversationAnalysisRequestTask>(Arrays.asList(task1, task2)));
        RestResult result = RestHelper.sendPost(uri, new Gson().toJson(request), m_userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_ACCEPTED });
        return result.getHeaders().get("operation-location").get(0);
    }

    private boolean GetConversationAnalysisStatus(String conversationAnalysisUrl) throws Exception {
        // Get Conversation Analysis Status and Results REST API request and response JSON sample and schema:
        // https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeConversation_JobStatus
        RestResult result = RestHelper.sendGet(conversationAnalysisUrl, m_userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
        String status = result.getJson().get("status").getAsString().toLowerCase();
        if (status.equals("failed")) {
            throw new Exception(String.format("Unable to analyze conversation. Response:%s%s", System.lineSeparator(), result.getText()));
        }
        else {
            return status.equals("succeeded");
        }
    }

    private void WaitForConversationAnalysis(String conversationAnalysisUrl) throws Exception {
        boolean done = false;
        while (!done) {
            System.out.println(String.format("Waiting %d seconds for conversation analysis to complete.", waitSeconds));
            Thread.sleep(waitSeconds * 1000);
            done = this.GetConversationAnalysisStatus(conversationAnalysisUrl);
        }
        return;
    }

    private RestResult GetConversationAnalysis(String conversationAnalysisUrl) throws Exception {
        return RestHelper.sendGet(conversationAnalysisUrl, m_userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
    }

    private ConversationAnalysisResult GetConversationAnalysisResult(RestResult conversationAnalysis) throws Exception {

        Optional<JsonObject> summaryTask = Optional.empty();
        Optional<JsonObject> PIITask = Optional.empty();
        Iterator<JsonElement> iterator = conversationAnalysis.getJson().get("tasks").getAsJsonObject().getAsJsonArray("items").iterator();
        while (iterator.hasNext()) {
            JsonObject value = (JsonObject)iterator.next().getAsJsonObject();
            if (value.get("taskName").getAsString().equals("summary_1")) {
                summaryTask = Optional.of(value);
            }
            else if (value.get("taskName").getAsString().equals("PII_1")) {
                PIITask = Optional.of(value);
            }
        }
        if (!summaryTask.isPresent() || !PIITask.isPresent()) {
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
            .map(document -> {
                Spliterator<JsonElement> entities = document.getAsJsonObject().getAsJsonArray("entities").spliterator();
                return StreamSupport.stream(entities, false)
                .map(entity -> new PIICategoryAndText(entity.getAsJsonObject().get("category").getAsString(), entity.getAsJsonObject().get("text").getAsString()))
                .collect(Collectors.toList());
            })
            .collect(Collectors.toList());
        return new ConversationAnalysisResult(summary, PIIAnalysis);
    }

    private String GetResults(
        List<PhraseAndSpeaker> phrasesAndSpeakers,
        List<String> sentiments,
        ConversationAnalysisResult conversationAnalysis)
    {
        var result = new StringBuilder();
        for (var index = 0; index < phrasesAndSpeakers.size(); index++)
        {
            result.append(String.format("Phrase: %s%s", phrasesAndSpeakers.get(index).phrase, System.lineSeparator()));
            result.append(String.format("Speaker: %d%s", phrasesAndSpeakers.get(index).speakerNumber, System.lineSeparator()));
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
            (acc, item) -> String.format("%s    Aspect: %s. Summary: %s.%s", acc, item.aspect, item.summary, System.lineSeparator()), (s1, s2) -> s1 + s2));
        return result.toString();
    }

    private void PrintResults(JsonObject transcription, JsonObject sentimentAnalysis, JsonObject conversationAnalysis)
    {
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        System.out.println(gson.toJson(new CombinedResults(transcription, sentimentAnalysis, conversationAnalysis)));
    }

    public static void main(String[] args) throws Exception {
        final var usage = String.join(System.lineSeparator(),
            "Usage: java -cp .;target\\dependency\\* CallCenter [...]",
            "",
            "  HELP",
            "    --help                        Show this help and stop.",
            "",
            "  CONNECTION",
            "    --speechKey KEY                 Your Azure Speech service subscription key. Required.",
            "    --speechRegion REGION           Your Azure Speech service region. Required.",
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
            "    --input URL                     Input audio from URL. Required.",
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
                
                // How to use batch transcription:
                // https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
                final String transcriptionId = callCenter.CreateTranscription(userConfig.getInputAudioURL());
                System.out.println(String.format("Transcription ID: %s", transcriptionId));
                callCenter.WaitForTranscription(transcriptionId);
                final RestResult transcriptionFiles = callCenter.GetTranscriptionFiles(transcriptionId);
                final String transcriptionUri = callCenter.GetTranscriptionUri(transcriptionFiles);
                final JsonObject transcription = callCenter.GetTranscription(transcriptionUri);
                final List<PhraseAndSpeaker> phrasesAndSpeakers = callCenter.GetTranscriptionPhrasesAndSpeakers(transcription);
                final JsonObject sentimentAnalysis = callCenter.GetSentimentAnalysis(phrasesAndSpeakers);
                final List<ConversationItem> conversationItems = callCenter.TranscriptionPhrasesToConversationItems(phrasesAndSpeakers);
                // NOTE: Conversation summary is currently in gated public preview. You can sign up here:
                // https://aka.ms/applyforconversationsummarization/
                final String conversationAnalysisUrl = callCenter.RequestConversationAnalysis(conversationItems);
                callCenter.WaitForConversationAnalysis(conversationAnalysisUrl);
                final RestResult conversationAnalysis = callCenter.GetConversationAnalysis(conversationAnalysisUrl);
                if (userConfig.getOutputFilePath().isPresent())
                {
                    List<String> sentiments = callCenter.GetSentiments(sentimentAnalysis);
                    ConversationAnalysisResult conversationAnalysisResult = callCenter.GetConversationAnalysisResult(conversationAnalysis);
                    var outputFile = new FileWriter(userConfig.getOutputFilePath().get(), true);
                    outputFile.write(callCenter.GetResults(phrasesAndSpeakers, sentiments, conversationAnalysisResult));
                    outputFile.close();                    
                }
                callCenter.PrintResults(transcription, sentimentAnalysis, conversationAnalysis.getJson());
                // Clean up resources.
                System.out.println("Deleting transcription.");
                callCenter.DeleteTranscription(transcriptionId);
            }
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
        }
    }
}

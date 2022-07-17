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
// TODO1 Replace X[] with List<X>.

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
    final private static int waitSeconds = 5;

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
        String transcriptionUri = result.getJson().get("self").getAsString();
        // The transcription ID is at the end of the transcription URI.
        String transcriptionId = transcriptionUri.split("/")[-1];
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
        String status = result.getJson().get("status").getAsString();
        return status.equals("Succeeded");
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

    private String GetTranscriptionUri(String transcriptionId) throws Exception {
        // Get Transcription Files REST API request and response JSON sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
        String uri = m_userConfig.getSpeechEndpoint() + String.format("%s/%s/files", speechTranscriptionPath, transcriptionId);
        RestResult result = RestHelper.sendGet(uri, m_userConfig.getSpeechSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
        Optional<String> contentUri = Optional.empty();
        Iterator<JsonElement> iterator = result.getJson().getAsJsonArray("values").iterator();
        while (iterator.hasNext()) {
            JsonObject value = (JsonObject)iterator.next().getAsJsonObject();
            if (value.get("kind").getAsString().equals("Transcription")) {
                contentUri = Optional.of(value.getAsJsonObject("links").get("contentUrl").getAsString());
                break;
            }
        }
        if (!contentUri.isPresent()) {
            throw new Exception (String.format("Unable to parse response from Get Transcription Files API:%s%s", System.lineSeparator(), result.getText()));
        }
        return contentUri.get();
    }

    private static PhraseAndSpeaker[] GetTranscriptionPhrasesAndSpeakers(String transcriptionUri) throws Exception {
        RestResult result = RestHelper.sendGet(transcriptionUri, "", new int[] { HttpURLConnection.HTTP_OK });
        Spliterator<JsonElement> spliterator = result.getJson().getAsJsonArray("recognizedPhrases").spliterator();
        return StreamSupport.stream(spliterator, false).map(phrase -> {
            JsonElement best = phrase.getAsJsonObject().getAsJsonArray("nBest").get(0);
            return new PhraseAndSpeaker(best.getAsJsonObject().get("display").getAsString(), phrase.getAsJsonObject().get("speaker").getAsInt());
        }).toArray(PhraseAndSpeaker[]::new);
    }

    private void DeleteTranscription(String transcriptionId) throws Exception {
        String uri = m_userConfig.getSpeechEndpoint() + String.format("%s/%s", speechTranscriptionPath, transcriptionId);
        RestHelper.sendDelete(uri, m_userConfig.getSpeechSubscriptionKey(), new int[] { HttpURLConnection.HTTP_NO_CONTENT } );
        return;
    }

    private Stream<SentimentResult> GetSentimentsHelper (String uri, List<Document> documents) throws Exception {
        RestResult result = RestHelper.sendPost(uri, new Gson().toJson(new SentimentRequest(documents)), m_userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
        return StreamSupport.stream(result.getJson().getAsJsonObject("results").getAsJsonArray("documents").spliterator(), false)
        .map(document -> new SentimentResult(document.getAsJsonObject().get("id").getAsInt(), document.getAsJsonObject().get("sentiment").getAsString()));
    }

    private List<String> GetSentiments(PhraseAndSpeaker[] phrasesAndSpeakers) throws Exception {
        // Analyze Text REST API request and response JSON sample and schema:
        // https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeText
        String uri = m_userConfig.getLanguageEndpoint() + String.format("%s/%s", sentimentAnalysisPath, sentimentAnalysisQuery);
        // Convert each transcription phrase to a "document" as expected by the Analyze Text REST API.
        // Include a counter to use as a document ID.
        List<Document> documentsToSend_1 = IntStream.range(0, phrasesAndSpeakers.length)
            // Use mapToObj because we are converting from int to Document.
            .mapToObj(i -> new Document (i, m_userConfig.getLanguage(), phrasesAndSpeakers[i].phrase))
            .collect(Collectors.toList());
        // We can only analyze sentiment for 10 documents per request.
        List<List<Document>> documentsToSend_2 = Chunk(documentsToSend_1, 10);
        // We can't call methods that throw exceptions inside a lambda, which means we can't use map.
        List<Stream<SentimentResult>> results = new ArrayList<Stream<SentimentResult>>();
        for (List<Document> documentsToSend_3 : documentsToSend_2) {
            results.add(GetSentimentsHelper(uri, documentsToSend_3));
        }
        return results
            .stream()
            .flatMap(x -> x)
            .sorted(Comparator.comparing(document -> document.id))
            .map(document -> document.sentiment)
            .collect(Collectors.toList());
    }

    private List<ConversationItem> TranscriptionPhrasesToConversationItems(PhraseAndSpeaker[] phrasesAndSpeakers)
    {
        // Include a counter to use as a document ID.
        return IntStream.range(0, phrasesAndSpeakers.length)
            .mapToObj(i -> new ConversationItem(
                i,
                phrasesAndSpeakers[i].phrase,
                // The first person to speak is probably the agent.
                (1 == phrasesAndSpeakers[i].speakerID) ? "Agent" : "Customer",
                phrasesAndSpeakers[i].speakerID
            ))
            .collect(Collectors.toList());
    }

    private String RequestConversationAnalysis(PhraseAndSpeaker[] phrasesAndSpeakers) throws Exception
    {
        // Analyze Conversation REST API request and response JSON sample and schema:
        // https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeConversation_SubmitJob
        List<ConversationItem> conversationItems = TranscriptionPhrasesToConversationItems(phrasesAndSpeakers);
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
        String status = result.getJson().get("status").getAsString();
        return status.equals("succeeded");
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

    private ConversationAnalysisResult GetConversationAnalysis(String conversationAnalysisUrl) throws Exception {
        RestResult result = RestHelper.sendGet(conversationAnalysisUrl, m_userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
        Optional<JsonObject> summaryTask = Optional.empty();
        Optional<JsonObject> PIITask = Optional.empty();
        Iterator<JsonElement> iterator = result.getJson().get("tasks").getAsJsonObject().getAsJsonArray("items").iterator();
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
            throw new Exception (String.format("Unable to parse response from Get Conversation Analysis API:%s%s", System.lineSeparator(), result.getText()));
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

    private void PrintResults(
        PhraseAndSpeaker[] phrasesAndSpeakers,
        List<String> sentiments,
        ConversationAnalysisResult conversationAnalysis)
    {
        for (var index = 0; index < phrasesAndSpeakers.length; index++)
        {
            System.out.println(String.format("Phrase: %s", phrasesAndSpeakers[index].phrase));
            System.out.println(String.format("Speaker: %d", phrasesAndSpeakers[index].speakerID));
            if (index < sentiments.size())
            {
                System.out.println(String.format("Sentiment: %s", sentiments.get(index)));
            }
            if (index < conversationAnalysis.PIIAnalysis.size())
            {
                if (conversationAnalysis.PIIAnalysis.get(index).size() > 0)
                {
                    // reduce() needs a combiner when converting type. See:
                    // https://stackoverflow.com/questions/24308146/why-is-a-combiner-needed-for-reduce-method-that-converts-type-in-java-8
                    String entities = conversationAnalysis.PIIAnalysis.get(index).stream().reduce("", (acc, entity) -> String.format("%s    Category: %s. Text: %s.%s", acc, entity.category, entity.text, System.lineSeparator()), (s1, s2) -> s1 + s2);
                    System.out.println(String.format("Recognized entities (PII):%s%s", System.lineSeparator(), entities));
                }
                else
                {
                    System.out.println("Recognized entities (PII): none.");
                }
            }
            System.out.println();
        }
        System.out.println(conversationAnalysis.summary.stream().reduce(String.format("Conversation summary:%s", System.lineSeparator()),
            (acc, item) -> String.format("%s    Aspect: %s. Summary: %s.%s", acc, item.aspect, item.summary, System.lineSeparator()), (s1, s2) -> s1 + s2));
        return;
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
            "                                    If this is not present, mono is assumed."
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
                //final String transcriptionId = callCenter.CreateTranscription(m_userConfig.getInputAudioURL());
                //callCenter.WaitForTranscription(transcriptionId);
                // TODO1 Fix this.
                final String transcriptionId = "fafe4af1-73ad-4658-b9c6-a0f3f753387d";
                final String transcriptionUri = callCenter.GetTranscriptionUri(transcriptionId);
                final PhraseAndSpeaker[] phrasesAndSpeakers = callCenter.GetTranscriptionPhrasesAndSpeakers(transcriptionUri);
                final List<String> sentiments = callCenter.GetSentiments(phrasesAndSpeakers);
                // NOTE: Conversation summary is currently in gated public preview. You can sign up here:
                // https://aka.ms/applyforconversationsummarization/
                var conversationAnalysisUrl = callCenter.RequestConversationAnalysis(phrasesAndSpeakers);
                callCenter.WaitForConversationAnalysis(conversationAnalysisUrl);
                var conversationAnalysisResult = callCenter.GetConversationAnalysis(conversationAnalysisUrl);
                callCenter.PrintResults(phrasesAndSpeakers, sentiments, conversationAnalysisResult);
                // TODO1 Uncomment.
                // Clean up resources.
                //DeleteTranscription(transcriptionId);
            }
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
        }
    }
}

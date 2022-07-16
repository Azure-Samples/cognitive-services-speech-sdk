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

class TranscriptionRequest {
    public String[] contentUrls;
    public JsonObject properties;
    public String locale;
    public String displayName;
    public TranscriptionRequest(String[] contentUrls, JsonObject properties, String locale, String displayName) {
        this.contentUrls = contentUrls;
        this.properties = properties;
        this.locale = locale;
        this.displayName = displayName;
    }
}

class PhraseAndSpeaker {
    public String phrase;
    public int speakerID;
    public PhraseAndSpeaker(String phrase, int speakerID) {
        this.phrase = phrase;
        this.speakerID = speakerID;
    }
}

class Document {
    public int id;
    public String language;
    public String text;
    public Document(int id, String language, String text) {
        this.id = id;
        this.language = language;
        this.text = text;
    }
}

class SentimentRequest {
    final public String kind = "SentimentAnalysis";
    public JsonObject analysisInput;
    public SentimentRequest(List<Document> documents) {
        this.analysisInput = new JsonObject();
        this.analysisInput.addProperty("documents", new Gson().toJson(documents));
    }
}

class SentimentResult {
    public int id;
    public String sentiment;
    public SentimentResult(int id, String sentiment) {
        this.id = id;
        this.sentiment = sentiment;
    }
}

public class CallCenter {
    private UserConfig m_userConfig;
    
    // This should not change unless you switch to a new version of the Speech REST API.
    final private static String speechTranscriptionPath = "/speechtotext/v3.0/transcriptions";

    // These should not change unless you switch to a new version of the Cognitive Language REST API.
    final private static String sentimentAnalysisPath = "/language/:analyze-text";
    final private static String sentimentAnalysisQuery = "?api-version=2022-05-01";
    final private static String conversationAnalysisPath = "/language/analyze-conversations/jobs";
    final private static String conversationAnalysisQuery = "?api-version=2022-05-15-preview";
    final private static String conversationSummaryModelVersion = "2022-05-15-preview";

    // How long to wait while polling batch transcription and conversation analysis status.
    final private static int waitSeconds = 5;

    private void Initialize(UserConfig userConfig) throws IOException
    {
        m_userConfig = userConfig;
    }

// TODO1 Convert to var.

    private String CreateTranscription(String uriToTranscribe) throws Exception {
        String uri = m_userConfig.getSpeechEndpoint() + speechTranscriptionPath;
        // Create Transcription API JSON request sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
        // Notes:
        // - locale and displayName are required.
        // - diarizationEnabled should only be used with mono audio input.
        JsonObject properties = new JsonObject();
        properties.addProperty("diarizationEnabled", !m_userConfig.getUseStereoAudio());
        var request = new TranscriptionRequest(
            new String[] { uriToTranscribe },
            properties,
            m_userConfig.getLocale(),
            String.format("call_center_%s", java.time.LocalDateTime.now())
        );
        RestResult result = RestHelper.sendPost(uri, new Gson().toJson(request), m_userConfig.getSpeechSubscriptionKey(), new int[] { HttpURLConnection.HTTP_CREATED });
        // Create Transcription API JSON response sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
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
    
    // Return true if the transcription has status "Succeeded".
    private boolean GetTranscriptionStatus(String transcriptionId) throws Exception {
        String uri = m_userConfig.getSpeechEndpoint() + String.format("%s/%s", speechTranscriptionPath, transcriptionId);
        RestResult result = RestHelper.sendGet(uri, m_userConfig.getSpeechSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
        String status = result.getJson().get("status").getAsString();
        return status.equals("Succeeded");
    }

    // Poll the transcription status until it has status "Succeeded".
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
        String uri = m_userConfig.getSpeechEndpoint() + String.format("%s/%s/files", speechTranscriptionPath, transcriptionId);
        RestResult result = RestHelper.sendGet(uri, m_userConfig.getSpeechSubscriptionKey(), new int[] { HttpURLConnection.HTTP_OK });
        // Get Transcription Files JSON response sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
        Optional<String> contentUri = Optional.empty();
        Iterator<JsonElement> iterator = result.getJson().getAsJsonArray("values").iterator();
        while (iterator.hasNext()) {
            JsonElement value_1 = (JsonElement)iterator.next();
            JsonObject value_2 = value_1.getAsJsonObject();
            if (value_2.get("kind").getAsString().equals("Transcription")) {
                contentUri = Optional.of(value_2.getAsJsonObject("links").get("contentUrl").getAsString());
                break;
            }
        }
        if (!contentUri.isPresent()) {
            throw new Exception (String.format("Unable to parse response from Get Transcription Files API:%s%s", System.lineSeparator(), result.getText()));
        }
        return contentUri.get();
    }

// TODO1 Replace X[] with List<X>?

    // Return transcription content as a list of phrases.
    private static PhraseAndSpeaker[] GetTranscriptionPhrasesAndSpeakers(String transcriptionUri) throws Exception {
        RestResult result = RestHelper.sendGet(transcriptionUri, "", new int[] { HttpURLConnection.HTTP_OK });
        // TODO1 Write helper method to convert JsonArray to array or List.
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

    private <T> List<List<T>> Chunk(List<T> xs, int size) {
        return IntStream.range(0, xs.size())
            .mapToObj(i -> new AbstractMap.SimpleEntry<Integer, T>(i, xs.get(i)))
            .collect(Collectors.groupingBy(x -> x.getKey() / size))
            .values()
            .stream()
            .map(chunk -> chunk.stream().map(x -> x.getValue()).collect(Collectors.toList()))
            .collect(Collectors.toList());
    }

    private Stream<SentimentResult> GetSentimentsHelper (String uri, List<Document> documents) throws Exception {
        RestResult result = RestHelper.sendPost(uri, new Gson().toJson(new SentimentRequest(documents)), m_userConfig.getLanguageSubscriptionKey(), new int[] { HttpURLConnection.HTTP_ACCEPTED });
        return StreamSupport.stream(result.getJson().getAsJsonObject("results").getAsJsonArray("documents").spliterator(), false)
        .map(document -> new SentimentResult(document.getAsJsonObject().get("id").getAsInt(), document.getAsJsonObject().get("sentiment").getAsString()));
    }

    private String[] GetSentiments(PhraseAndSpeaker[] phrasesAndSpeakers) throws Exception {
        String uri = m_userConfig.getLanguageEndpoint() + String.format("%s/%s", sentimentAnalysisPath, sentimentAnalysisQuery);

        // Convert each transcription phrase to a "document" as expected by the sentiment analysis REST API.
        // Include a counter to use as a document ID.
        List<Document> documentsToSend_1 = IntStream.range(0, phrasesAndSpeakers.length)
            // Use mapToObj because we are converting from int to Document.
            .mapToObj(i -> new Document (i, m_userConfig.getLanguage(), phrasesAndSpeakers[i].phrase))
            .collect(Collectors.toList());
        // We can only analyze sentiment for 10 documents per request.
        List<List<Document>> documentsToSend_2 = Chunk(documentsToSend_1, 10);
        
        List<Stream<SentimentResult>> results = new ArrayList<Stream<SentimentResult>>();
        for (List<Document> documentsToSend_3 : documentsToSend_2) {
            results.add(GetSentimentsHelper(uri, documentsToSend_3));
        }

        return new String[] {};
/*
        return tasks
            .Aggregate(new (int id, string sentiment)[] {}, (acc, sublist) => acc.Concat(sublist).ToArray()) // TODO1 flatten
            .OrderBy(x => x.id)
            .Select(x => x.sentiment)
            .ToArray();
*/
    }

// TODO1 Conv summary post is HTTP_ACCEPTED.

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
                //final String transcriptionId = CreateTranscription(uriToTranscribe);
                // TODO1 Fix this before uploading.
                final String transcriptionId = "fafe4af1-73ad-4658-b9c6-a0f3f753387d";
                final String transcriptionUri = callCenter.GetTranscriptionUri(transcriptionId);
                final PhraseAndSpeaker[] phrasesAndSpeakers = callCenter.GetTranscriptionPhrasesAndSpeakers(transcriptionUri);

                // TODO1 Remove
                System.out.println(new Gson().toJson(phrasesAndSpeakers).toString());

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

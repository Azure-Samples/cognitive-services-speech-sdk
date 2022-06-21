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
javac Call_Center.java -cp ".;target\dependency\*"
java -cp ".;target\dependency\*" Call_Center
*/

public class Call_Center {
    // Replace this value with the region for your Speech subscription.
    private static String speechRegion = "PASTE_YOUR_SPEECH_REGION_HERE";
    // Replace this value with the subscription key for your Speech subscription.
    private static String speechKey = "PASTE_YOUR_SPEECH_SUBSCRIPTION_KEY_HERE";

    // Replace this value with the endpoint for your Text Analytics subscription.
    private static String textAnalyticsHost = "PASTE_YOUR_TEXT_ANALYTICS_ENDPOINT_HERE";
    // Replace this value with the subscription key for your Text Analytics subscription.
    private static String textAnalyticsKey = "PASTE_YOUR_TEXT_ANALYTICS_SUBSCRIPTION_KEY_HERE";

    // Replace this value with the URL of the audio file you want to transcribe.
    private static String uriToTranscribe = "https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/master/sampledata/audiofiles/wikipediaOcelot.wav";

    // This should not change unless the Speech REST API changes.
    private static String speechHost = String.format ("https://%s.api.cognitive.microsoft.com", speechRegion);

    // This should not change unless you switch to a new version of the Speech REST API.
    private static String speechTranscriptionPath = "/speechtotext/v3.0/transcriptions";

    // These should not change unless you switch to a new version of the Text Analytics REST API.
    private static String detectLanguagePath = "/text/analytics/v3.1/languages";
    private static String entitiesRecognitionGeneralPath = "/text/analytics/v3.1/entities/recognition/general";
    private static String entitiesRecognitionPIIPath = "/text/analytics/v3.1/entities/recognition/pii";
    private static String keyPhrasesPath = "/text/analytics/v3.1/keyPhrases";
    private static String sentimentAnalysisPath = "/text/analytics/v3.1/sentiment";

    // How long to wait while polling transcription status.
    private static int waitSeconds = 5;

    private static JsonObject parseJSON (String json) {
        return JsonParser.parseString(json).getAsJsonObject();
    }

    private static String sendGet (String url_1, String key, int[] expectedStatusCodes) throws Exception {
        URL url_2 = new URL(url_1);
        HttpsURLConnection connection = (HttpsURLConnection) url_2.openConnection();
        connection.setRequestMethod("GET");
        connection.setRequestProperty("Ocp-Apim-Subscription-Key", key);

        StringBuilder response = new StringBuilder ();
        BufferedReader in = new BufferedReader(new InputStreamReader(connection.getInputStream()));
        String line;
        while ((line = in.readLine()) != null) {
            response.append(line);
        }
        in.close();

        int statusCode = connection.getResponseCode();
        if (!IntStream.of(expectedStatusCodes).anyMatch(x -> x == statusCode)) {
            throw new Exception(String.format("The response from %s has an unexpected status code: %d", url_1, statusCode));
        }

        return response.toString();
    }
    
    private static String sendPost (String url_1, String content, String key, int[] expectedStatusCodes) throws Exception {
        byte[] encodedContent = content.getBytes("UTF-8");

        URL url_2 = new URL(url_1);
        HttpsURLConnection connection = (HttpsURLConnection) url_2.openConnection();
        connection.setRequestMethod("POST");
        connection.setRequestProperty("Content-Type", "application/json");
        connection.setRequestProperty("Ocp-Apim-Subscription-Key", key);
        // Force POST request.
        connection.setDoOutput(true);

        DataOutputStream wr = new DataOutputStream(connection.getOutputStream());
        wr.write(encodedContent, 0, encodedContent.length);
        wr.flush();
        wr.close();

        StringBuilder response = new StringBuilder ();
        BufferedReader in = new BufferedReader(new InputStreamReader(connection.getInputStream()));
        String line;
        while ((line = in.readLine()) != null) {
            response.append(line);
        }
        in.close();

        int statusCode = connection.getResponseCode();
        if (!IntStream.of(expectedStatusCodes).anyMatch(x -> x == statusCode)) {
            throw new Exception(String.format("The response from %s has an unexpected status code: %d", url_1, statusCode));
        }

        return response.toString();
    }
    
    private static void sendDelete (String url_1, String key, int[] expectedStatusCodes) throws Exception {
        URL url_2 = new URL(url_1);
        HttpsURLConnection connection = (HttpsURLConnection) url_2.openConnection();
        connection.setRequestMethod("DELETE");
        connection.setRequestProperty("Ocp-Apim-Subscription-Key", key);

        int statusCode = connection.getResponseCode();
        if (!IntStream.of(expectedStatusCodes).anyMatch(x -> x == statusCode)) {
            throw new Exception(String.format("The response from %s has an unexpected status code: %d", url_1, statusCode));
        }

        return;
    }
    
    // Create a transcription of the specified audio.
    private static String CreateTranscription(String uriToTranscribe) throws Exception {
        String uri = speechHost + speechTranscriptionPath;
        String displayName = String.format("call_center_%s", java.time.LocalDateTime.now());
        // Create Transcription API JSON request sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
        // Note: locale and displayName are required.
        String content = "{" +
String.format("\"contentUrls\" : [%s],", uriToTranscribe) +
"\"locale\" : \"en-us\"," +
String.format("\"displayName\" : \"%s\"", displayName) +
"}";
        String response = sendPost(uri, content, speechKey, new int[] { HttpURLConnection.HTTP_CREATED });
        JsonObject document = parseJSON(response);
        // Create Transcription API JSON response sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
        String transcriptionUri = document.get("self").getAsString();
        // The transcription ID is at the end of the transcription URI.
        String transcriptionId = transcriptionUri.split("/")[-1];
        // Verify the transcription ID is a valid GUID.
        try {
            UUID uuid = UUID.fromString(transcriptionId);
        } catch (IllegalArgumentException exception) {
            throw new Exception(String.format("Unable to parse response from Create Transcription API:%s%s", System.lineSeparator(), response));
        }        
        return transcriptionId;
    }
    
    // Return true if the transcription has status "Succeeded".
    private static boolean GetTranscriptionStatus(String transcriptionId) throws Exception {
        String uri = speechHost + String.format("%s/%s", speechTranscriptionPath, transcriptionId);
        String response = sendGet(uri, speechKey, new int[] { HttpURLConnection.HTTP_OK });
        JsonObject document = parseJSON(response);
        String status = document.get("status").getAsString();
        return status.equals("Succeeded");
    }

    // Poll the transcription status until it has status "Succeeded".
    private static void WaitForTranscription(String transcriptionId) throws Exception {
        boolean done = false;
        while(!done) {
            System.out.println(String.format("Waiting %d seconds for transcription to complete.", waitSeconds));
            Thread.sleep(waitSeconds * 1000);
            done = GetTranscriptionStatus(transcriptionId);
        }
        return;
    }

    private static String GetTranscriptionUri(String transcriptionId) throws Exception {
        String uri = speechHost + String.format("%s/%s/files", speechTranscriptionPath, transcriptionId);
        String response = sendGet(uri, speechKey, new int[] { HttpURLConnection.HTTP_OK });
        // Get Transcription Files JSON response sample and schema:
        // https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/GetTranscriptionFiles
        JsonObject document = parseJSON(response);
        Optional<String> contentUri = Optional.empty();
        Iterator<JsonElement> iterator = document.getAsJsonArray("values").iterator();
        while (iterator.hasNext()) {
            JsonElement value_1 = (JsonElement)iterator.next();
            JsonObject value_2 = value_1.getAsJsonObject();
            if (value_2.get("kind").getAsString().equals("Transcription")) {
                contentUri = Optional.of(value_2.getAsJsonObject("links").get("contentUrl").getAsString());
            }
        }
        if (!contentUri.isPresent()) {
            throw new Exception (String.format("Unable to parse response from Get Transcription Files API:%s%s", System.lineSeparator(), response));
        }
        return contentUri.get();
    }

    // Return transcription content as a list of phrases.
    private static String[] GetTranscriptionPhrases(String transcriptionUri) throws Exception {
        String response = sendGet(transcriptionUri, "", new int[] { HttpURLConnection.HTTP_OK });
        JsonObject document = parseJSON(response);
        Spliterator<JsonElement> spliterator = document.getAsJsonArray("recognizedPhrases").spliterator();
        return StreamSupport.stream(spliterator, false).map(phrase -> {
            JsonElement best = phrase.getAsJsonObject().getAsJsonArray("nBest").get(0);
            return best.getAsJsonObject().get("display").getAsString();
        }).toArray(String[]::new);
    }

    private static void DeleteTranscription(String transcriptionId) throws Exception {
        String uri = speechHost + String.format("%s/%s", speechTranscriptionPath, transcriptionId);
        sendDelete(uri, speechKey, new int[] { HttpURLConnection.HTTP_NO_CONTENT } );
        return;
    }

    // Detect languages for a list of transcription phrases.
    private static String[] GetTranscriptionLanguages(String[] transcriptionPhrases) throws Exception {
        String uri = textAnalyticsHost + detectLanguagePath;

        // Convert each transcription phrase to a "document" as expected by the Text Analytics Detect Language REST API.
        // Include a counter to use as a document ID.
        // Detect Language API JSON request and response samples:
        // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/languages/languages
        String[] documents_1 = IntStream.range(0, transcriptionPhrases.length)
            // Use mapToObj because we are converting from int to String.
            .mapToObj(i -> {
                return "{" +
String.format("\"id\" : \"%d\",", i + 1) +
String.format("\"text\" : \"%s\"", transcriptionPhrases[i]) +
"}";
            }).toArray(String[]::new);
        
        String documents_2 = String.format("{ \"documents\" : [%s] }", String.join(",", documents_1));
        String response = sendPost(uri, documents_2, textAnalyticsKey, new int[] { HttpURLConnection.HTTP_OK });
        JsonObject document_1 = parseJSON(response);
        Spliterator<JsonElement> spliterator = document_1.getAsJsonArray("documents").spliterator();
        return StreamSupport.stream(spliterator, false)
            .map(document_2 -> document_2.getAsJsonObject().getAsJsonObject("detectedLanguage").get("iso6391Name").getAsString())
            .toArray(String[]::new);
    }

    // Convert a list of transcription phrases to "document" JSON elements as expected by various Text Analytics REST APIs.
    private static String TranscriptionPhrasesToDocuments(String[] transcriptionPhrases, String[] transcriptionLanguages) throws Exception {
        // Combine the list of transcription phrases with the corresponding list of transcription languages into a single
        // list of tuples. Convert each (phrase, language) tuple into a "document" as expected by various Text Analytics
        // REST APIs. For example, see the sample request body for the
        // Sentiment REST API:
        // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/sentiment/sentiment#examples
        // Include a counter to use as a document ID.
        if (transcriptionPhrases.length != transcriptionLanguages.length) {
            throw new Exception("TranscriptionPhrasesToDocuments: transcriptionPhrases and transcriptionLanguages must have the same length.");
        }
        String[] documents_1 = IntStream.range(0, transcriptionPhrases.length)
            // Use mapToObj because we are converting from int to String.
            .mapToObj(i -> {
                return "{" +
String.format("\"id\" : \"%d\",", i) +
String.format("\"language\" : \"%s\",", transcriptionLanguages[i]) +
String.format("\"text\" :  \"%s\"", transcriptionPhrases[i]) +
"}";
            }).toArray(String[]::new);
        return String.format("{ \"documents\" : [%s] }", String.join(",", documents_1));
    }

    // Get sentiments for transcription phrases.
    private static String[] GetSentiments(String transcriptionDocuments) throws Exception {
        String uri = textAnalyticsHost + sentimentAnalysisPath;
        // Sentiment JSON request and response samples:
        // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/sentiment/sentiment#examples
        String response = sendPost(uri, transcriptionDocuments, textAnalyticsKey, new int[] { HttpURLConnection.HTTP_OK });
        JsonObject document_1 = parseJSON(response);
        Spliterator<JsonElement> spliterator = document_1.getAsJsonArray("documents").spliterator();
        return StreamSupport.stream(spliterator, false)
            .map(document_2 -> document_2.getAsJsonObject().get("sentiment").getAsString())
            .toArray(String[]::new);
    }

    // Get key phrases for transcription phrases.
    private static String[][] GetKeyPhrases(String transcriptionDocuments) throws Exception {
        String uri = textAnalyticsHost + keyPhrasesPath;
        // Key phrases JSON request and response samples:
        // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/key-phrases/key-phrases#examples
        String response = sendPost(uri, transcriptionDocuments, textAnalyticsKey, new int[] { HttpURLConnection.HTTP_OK });
        JsonObject document_1 = parseJSON(response);
        Spliterator<JsonElement> documentsSpliterator = document_1.getAsJsonArray("documents").spliterator();
        return StreamSupport.stream(documentsSpliterator, false)
            .map(document_2 -> {
                Spliterator<JsonElement> phrasesSpliterator = document_2.getAsJsonObject().getAsJsonArray("keyPhrases").spliterator();
                return StreamSupport.stream(phrasesSpliterator, false)
                    .map(phrase -> phrase.getAsString())
                    .toArray(String[]::new);
            })
            .toArray(String[][]::new);
    }

    // Get recognized entities (general) for transcription phrases.
    private static List<List<AbstractMap.SimpleEntry<String, String>>> GetRecognizedEntitiesGeneral(String transcriptionDocuments) throws Exception {
        String uri = textAnalyticsHost + entitiesRecognitionGeneralPath;
        // Entities recognition JSON request and response samples:
        // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/entities-recognition-general/entities-recognition-general#examples
        String response = sendPost(uri, transcriptionDocuments, textAnalyticsKey, new int[] { HttpURLConnection.HTTP_OK });
        JsonObject document_1 = parseJSON(response);
        Spliterator<JsonElement> documentsSpliterator = document_1.getAsJsonArray("documents").spliterator();
        return StreamSupport.stream(documentsSpliterator, false)
            .map(document_2 -> {
                Spliterator<JsonElement> entitiesSpliterator = document_2.getAsJsonObject().getAsJsonArray("entities").spliterator();
                return StreamSupport.stream(entitiesSpliterator, false)
                .map(entity_1 -> {
                    JsonObject entity_2 = entity_1.getAsJsonObject();
                    return new AbstractMap.SimpleEntry<String, String>(entity_2.get("category").getAsString(), entity_2.get("text").getAsString());
                })
                .collect(Collectors.toList());
            })
            .collect(Collectors.toList());
    }

    // Get recognized entities (PII) for transcription phrases.
    private static List<List<AbstractMap.SimpleEntry<String, String>>> GetRecognizedEntitiesPII(String transcriptionDocuments) throws Exception {
        String uri = textAnalyticsHost + entitiesRecognitionPIIPath;
        // Entities recognition JSON request and response samples:
        // https://docs.microsoft.com/en-us/rest/api/cognitiveservices-textanalytics/3.1preview4/entities-recognition-pii/entities-recognition-pii#examples
        String response = sendPost(uri, transcriptionDocuments, textAnalyticsKey, new int[] { HttpURLConnection.HTTP_OK });
        JsonObject document_1 = parseJSON(response);
        Spliterator<JsonElement> documentsSpliterator = document_1.getAsJsonArray("documents").spliterator();
        return StreamSupport.stream(documentsSpliterator, false)
            .map(document_2 -> {
                Spliterator<JsonElement> entitiesSpliterator = document_2.getAsJsonObject().getAsJsonArray("entities").spliterator();
                return StreamSupport.stream(entitiesSpliterator, false)
                .map(entity_1 -> {
                    JsonObject entity_2 = entity_1.getAsJsonObject();
                    return new AbstractMap.SimpleEntry<String, String>(entity_2.get("category").getAsString(), entity_2.get("text").getAsString());
                })
                .collect(Collectors.toList());
            })
            .collect(Collectors.toList());
    }

    // Print each transcription phrase, followed by its language, sentiment, and so on.
    static void PrintResults(
        String[] transcriptionPhrases,
        String[] transcriptionLanguages,
        String[] transcriptionSentiments,
        String[][] keyPhrases_1,
        List<List<AbstractMap.SimpleEntry<String, String>>> transcriptionEntitiesGeneral,
        List<List<AbstractMap.SimpleEntry<String, String>>> transcriptionEntitiesPII) throws Exception {
        String[] phrases = IntStream.range(0, transcriptionPhrases.length)
            // Use mapToObj because we are converting from int to String.
            .mapToObj(i -> {
                String[] keyPhrases_2 = IntStream.range(0, keyPhrases_1[i].length)
                    .mapToObj(j -> String.format("    %s%s", keyPhrases_1[i][j], System.lineSeparator()))
                    .toArray(String[]::new);
                String[] entitiesGeneral = StreamSupport.stream(transcriptionEntitiesGeneral.get(i).spliterator(), false)
                    .map(entity -> String.format("    Category: %s. Text: %s.%s", entity.getKey(), entity.getValue(), System.lineSeparator()))
                    .toArray(String[]::new);
                String[] entitiesPII = StreamSupport.stream(transcriptionEntitiesPII.get(i).spliterator(), false)
                    .map(entity -> String.format("    Category: %s. Text: %s.%s", entity.getKey(), entity.getValue(), System.lineSeparator()))
                    .toArray(String[]::new);
                return String.format("Phrase: %s%s", transcriptionPhrases[i], System.lineSeparator()) +
                String.format("Language: %s%s", transcriptionLanguages[i], System.lineSeparator()) +
                String.format("Sentiment: %s%s", transcriptionSentiments[i], System.lineSeparator()) +
                String.format("Key phrases: %s%s", System.lineSeparator(), String.join ("", keyPhrases_2)) +
                String.format("Recognized entities (general): %s%s", System.lineSeparator(), String.join ("", entitiesGeneral)) +
                String.format("Recognized entities (PII): %s%s", System.lineSeparator(), String.join ("", entitiesPII));
            }).toArray(String[]::new);
        for (String phrase : phrases) {
            System.out.println(phrase);
        }
        return;
    }

    public static void main(String[] args) throws Exception {
        // How to use batch transcription:
        // https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/batch-transcription.md
        String transcriptionId = CreateTranscription(uriToTranscribe);
        String transcriptionUri = GetTranscriptionUri(transcriptionId);
        String[] phrases = GetTranscriptionPhrases(transcriptionUri);
        String[] languages = GetTranscriptionLanguages(phrases);
        String documents = TranscriptionPhrasesToDocuments(phrases, languages);
        String[] sentiments = GetSentiments(documents);
        String[][] key_phrases = GetKeyPhrases(documents);
        List<List<AbstractMap.SimpleEntry<String, String>>> entities_general = GetRecognizedEntitiesGeneral(documents);
        List<List<AbstractMap.SimpleEntry<String, String>>> entities_pii = GetRecognizedEntitiesPII(documents);
        PrintResults(phrases, languages, sentiments, key_phrases, entities_general, entities_pii);
        // Clean up resources.
        DeleteTranscription(transcription_id);
    }
}

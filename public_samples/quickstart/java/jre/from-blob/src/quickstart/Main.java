package quickstart;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Date;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.UUID;

import com.google.gson.Gson;

final class Transcription {
    public String name;
    public String description;
    public String locale;
    public URL recordingsUrl;
    public Hashtable<String, String> resultsUrls;
    public UUID id;
    public Date createdDateTime;
    public Date lastActionDateTime;
    public String status;
    public String statusMessage;
}

final class TranscriptionDefinition {
    private TranscriptionDefinition(String name, String description, String locale, URL recordingsUrl,
            ModelIdentity[] models) {
        this.Name = name;
        this.Description = description;
        this.RecordingsUrl = recordingsUrl;
        this.Locale = locale;
        this.Models = models;
        this.properties = new Hashtable<String, String>();
        this.properties.put("PunctuationMode", "DictatedAndAutomatic");
        this.properties.put("ProfanityFilterMode", "Masked");
        this.properties.put("AddWordLevelTimestamps", "True");
    }

    public String Name;
    public String Description;
    public URL RecordingsUrl;
    public String Locale;
    public ModelIdentity[] Models;
    public Dictionary<String, String> properties;

    public static TranscriptionDefinition Create(String name, String description, String locale, URL recordingsUrl) {
        return TranscriptionDefinition.Create(name, description, locale, recordingsUrl, new ModelIdentity[0]);
    }

    public static TranscriptionDefinition Create(String name, String description, String locale, URL recordingsUrl,
            ModelIdentity[] models) {
        return new TranscriptionDefinition(name, description, locale, recordingsUrl, models);
    }
}

final class ModelIdentity {
    private ModelIdentity(UUID id) {
        this.Id = id;
    }

    public UUID Id;

    public static ModelIdentity Create(UUID Id) {
        return new ModelIdentity(Id);
    }
}

class AudioFileResult {
    public String AudioFileName;
    public SegmentResult[] SegmentResults;
}

class RootObject {
    public AudioFileResult[] AudioFileResults;
}

class NBest {
    public double Confidence;
    public String Lexical;
    public String ITN;
    public String MaskedITN;
    public String Display;
}

class SegmentResult {
    public String RecognitionStatus;
    public String Offset;
    public String Duration;
    public NBest[] NBest;
}

public class Main {

    private static String region = "YourServiceRegion";
    private static String subscriptionKey = "YourSubscriptionKey";
    private static String Locale = "en-US";
    private static String RecordingsBlobUri = "YourFileUrl";
    private static String Name = "Simple transcription";
    private static String Description = "Simple transcription description";

    public static void main(String[] args) throws IOException, InterruptedException {
        System.out.println("Starting transcriptions client...");
        String url = "https://" + region + ".cris.ai/api/speechtotext/v2.0/Transcriptions/";
        URL serviceUrl = new URL(url);

        HttpURLConnection postConnection = (HttpURLConnection) serviceUrl.openConnection();
        postConnection.setDoOutput(true);
        postConnection.setRequestMethod("POST");
        postConnection.setRequestProperty("Content-Type", "application/json");
        postConnection.setRequestProperty("Ocp-Apim-Subscription-Key", subscriptionKey);

        TranscriptionDefinition definition = TranscriptionDefinition.Create(Name, Description, Locale,
                new URL(RecordingsBlobUri));

        Gson gson = new Gson();

        OutputStream stream = postConnection.getOutputStream();
        stream.write(gson.toJson(definition).getBytes());
        stream.flush();

        int statusCode = postConnection.getResponseCode();

        if (statusCode != HttpURLConnection.HTTP_ACCEPTED) {
            System.out.println("Unexpected status code " + statusCode);
            return;
        }

        String transcriptionLocation = postConnection.getHeaderField("location");

        System.out.println("Transcription is located at " + transcriptionLocation);

        URL transcriptionUrl = new URL(transcriptionLocation);

        boolean completed = false;
        while (!completed) {
            {
                HttpURLConnection getConnection = (HttpURLConnection) transcriptionUrl.openConnection();
                getConnection.setRequestProperty("Ocp-Apim-Subscription-Key", subscriptionKey);
                getConnection.setRequestMethod("GET");

                int responseCode = getConnection.getResponseCode();

                if (responseCode != HttpURLConnection.HTTP_OK) {
                    System.out.println("Fetching the transcription returned unexpected http code " + responseCode);
                    return;
                }

                Transcription t = gson.fromJson(new InputStreamReader(getConnection.getInputStream()),
                        Transcription.class);

                switch (t.status) {
                case "Failed":
                    completed = true;
                    System.out.println("Transcription has failed " + t.statusMessage);
                    break;
                case "Succeeded":
                    completed = true;
                    String result = t.resultsUrls.get("channel_0");
                    System.out.println("Transcription has completed. Results are at " + result);

                    System.out.println("Fetching results");
                    URL resultUrl = new URL(result);

                    HttpURLConnection resultConnection = (HttpURLConnection) resultUrl.openConnection();
                    resultConnection.setRequestProperty("Ocp-Apim-Subscription-Key", subscriptionKey);
                    resultConnection.setRequestMethod("GET");

                    responseCode = resultConnection.getResponseCode();

                    if (responseCode != HttpURLConnection.HTTP_OK) {
                        System.out.println("Fetching the transcription returned unexpected http code " + responseCode);
                        return;
                    }

                    RootObject root = gson.fromJson(new InputStreamReader(resultConnection.getInputStream()),
                            RootObject.class);

                    for (AudioFileResult af : root.AudioFileResults) {
                        System.out
                                .println("There were " + af.SegmentResults.length + " results in " + af.AudioFileName);
                        for (SegmentResult segResult : af.SegmentResults) {
                            System.out.println("Status: " + segResult.RecognitionStatus);
                            if (segResult.RecognitionStatus.equalsIgnoreCase("success") && segResult.NBest.length > 0) {
                                System.out.println("Best text result was: '" + segResult.NBest[0].Display + "'");
                            }
                        }
                    }

                    break;
                case "Running":
                    System.out.println("Transcription is running.");
                    break;
                case "NotStarted":
                    System.out.println("Transcription has not started.");
                    break;
                }

                if (!completed) {
                    Thread.sleep(5000);
                }
            }
        }
    }
}

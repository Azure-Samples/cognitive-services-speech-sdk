package com.microsoft.api.example;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.apache.http.HttpHeaders;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.util.EntityUtils;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import java.util.logging.Level;
import java.util.logging.Logger;

public class BatchSynthesis {

    private static final String SUBSCRIPTION_KEY = "your-key";
    private static  final  String REGION="eastus";

    private static final String SPEECH_ENDPOINT = System.getenv("SPEECH_ENDPOINT") != null ? System.getenv("SPEECH_ENDPOINT")
            : "https://"+REGION+".api.cognitive.microsoft.com";
    private static final String API_VERSION = "2024-04-01";
    private static final Logger logger = Logger.getLogger(BatchSynthesis.class.getName());

    private static UUID createJobId() {
        return UUID.randomUUID();
    }

    private static boolean submitSynthesis(UUID jobId,
                                           String voiceName,
                                           String text
    ) throws IOException {
        String url = String.format("%s/texttospeech/batchsyntheses/%s?api-version=%s", SPEECH_ENDPOINT, jobId, API_VERSION);
        CloseableHttpClient httpClient = HttpClients.createDefault();

        Map<String, Object> payload = new HashMap<>();
        payload.put("inputKind", "PlainText");
        //or ssml
        //payload.put("inputKind", "SSML");
        Map<String, String> voice = new HashMap<>();
        voice.put("voice", voiceName);
        payload.put("synthesisConfig", voice);
        payload.put("customVoices", new HashMap<>());
        Map<String, String> txt = new HashMap<>();
        txt.put("content", text);
        payload.put("inputs", new Object[]{txt});
        Map<String, Object> properties = new HashMap<>();
        properties.put("outputFormat", "audio-24khz-160kbitrate-mono-mp3");
        properties.put("wordBoundaryEnabled", true);
        properties.put("sentenceBoundaryEnabled", true);
        payload.put("properties", properties);

        ObjectMapper objectMapper = new ObjectMapper();
        String requestBody = objectMapper.writeValueAsString(payload);

        HttpPut request = new HttpPut(url);
        request.setHeader("Ocp-Apim-Subscription-Key", SUBSCRIPTION_KEY);
        request.setHeader(HttpHeaders.CONTENT_TYPE, "application/json");
        request.setEntity(new StringEntity(requestBody, StandardCharsets.UTF_8));

        try (CloseableHttpResponse response = httpClient.execute(request)) {
            int statusCode = response.getStatusLine().getStatusCode();
            if (statusCode < 400) {
                String responseBody = EntityUtils.toString(response.getEntity(), StandardCharsets.UTF_8);
                logger.log(Level.INFO, "Batch synthesis job submitted successfully");
                logger.log(Level.INFO, "Job ID: " + objectMapper.readValue(responseBody, Map.class).get("id"));
                return true;
            } else {
                logger.log(Level.SEVERE, "Failed to submit batch synthesis job: " + response.getStatusLine().getReasonPhrase());
                return false;
            }
        }
    }

    private static String getSynthesis(UUID jobId) throws IOException {
        String url = String.format("%s/texttospeech/batchsyntheses/%s?api-version=%s", SPEECH_ENDPOINT, jobId, API_VERSION);
        CloseableHttpClient httpClient = HttpClients.createDefault();

        HttpGet request = new HttpGet(url);
        request.setHeader("Ocp-Apim-Subscription-Key", SUBSCRIPTION_KEY);

        try (CloseableHttpResponse response = httpClient.execute(request)) {
            int statusCode = response.getStatusLine().getStatusCode();
            if (statusCode < 400) {
                String responseBody = EntityUtils.toString(response.getEntity(), StandardCharsets.UTF_8);
                logger.log(Level.INFO, "Get batch synthesis job successfully");
                return (String) new ObjectMapper().readValue(responseBody, Map.class).get("status");
            } else {
                logger.log(Level.SEVERE, "Failed to get batch synthesis job: " + response.getStatusLine().getReasonPhrase());
                return null;
            }
        }
    }

    public static void main(String[] args) throws IOException, InterruptedException, JsonProcessingException {
        UUID jobId = createJobId();
        String text="hello this is a test voice";
        String voiceName="en-US-AvaMultilingualNeural";
        if (submitSynthesis(jobId,voiceName,text)) {
            while (true) {
                String status = getSynthesis(jobId);
                if ("Succeeded".equals(status)) {
                    logger.log(Level.INFO, "batch synthesis job succeeded");
                    break;
                } else if ("Failed".equals(status)) {
                    logger.log(Level.SEVERE, "batch synthesis job failed");
                    break;
                } else {
                    logger.log(Level.INFO, "batch synthesis job is still running, status [" + status + "]");
                    Thread.sleep(5000);
                }
            }
        }
    }
}
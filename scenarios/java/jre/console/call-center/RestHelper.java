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

public class RestHelper {
    private static JsonObject parseJson (String json) {
        return JsonParser.parseString(json).getAsJsonObject();
    }
    
    public static RestResult sendGet (String url_1, String key, int[] expectedStatusCodes) throws Exception {
        URL url_2 = new URL(url_1);
        HttpsURLConnection connection = (HttpsURLConnection) url_2.openConnection();
        connection.setRequestMethod("GET");
        connection.setRequestProperty("Ocp-Apim-Subscription-Key", key);

        StringBuilder response = new StringBuilder();
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

        JsonObject json = null;
        if (response.length() > 0) {
            json = parseJson(response.toString());
        }
        return new RestResult(response.toString(), json, connection.getHeaderFields());
    }
    
    public static RestResult sendPost (String url_1, String content, String key, int[] expectedStatusCodes) throws Exception {
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

        StringBuilder response = new StringBuilder();
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

        JsonObject json = null;
        if (response.length() > 0) {
            json = parseJson(response.toString());
        }
        return new RestResult(response.toString(), json, connection.getHeaderFields());
    }
    
    public static void sendDelete (String url_1, String key, int[] expectedStatusCodes) throws Exception {
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
}
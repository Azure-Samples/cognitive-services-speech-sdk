// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

package tests;

import static org.junit.Assert.*;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;

import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.RecognitionResult;
import com.microsoft.cognitiveservices.speech.CancellationDetails;

public class TestHelper {

    public static void AssertConnectionCountMatching(int connectedEventCount, int disconnectedEventCount) {
        System.out.println(
                "connectedEventCount=" + connectedEventCount + ", disconnectedEventCount=" + disconnectedEventCount);
        assertTrue(connectedEventCount > 0);
        assertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount);
    }

    public static String getAuthorizationToken(String subscriptionKey, String region) throws IOException {
        // Guard against swapped parameters.
        if (subscriptionKey == null || subscriptionKey.length() != 32) {
            throw new IllegalArgumentException("subscriptionKey is null or has unexpected length");
        }

        URL url = new URL("https://" + region + ".api.cognitive.microsoft.com/sts/v1.0/issueToken");

        HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
        urlConnection.setRequestProperty("Content-Type", "application/json");
        urlConnection.setRequestProperty("Ocp-Apim-Subscription-Key", subscriptionKey);
        urlConnection.setDoOutput(true);

        OutputStream out = new BufferedOutputStream(urlConnection.getOutputStream());

        BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(out, "UTF-8"));
        writer.write("{}");
        writer.flush();
        writer.close();
        out.close();

        urlConnection.connect();

        InputStreamReader streamReader = new InputStreamReader(urlConnection.getInputStream());
        BufferedReader reader = new BufferedReader(streamReader);
        StringBuilder stringBuilder = new StringBuilder();
        String inputLine;
        while ((inputLine = reader.readLine()) != null) {
            stringBuilder.append(inputLine);
        }
        reader.close();
        streamReader.close();

        return stringBuilder.toString();
    }

    public static void OutputResult(RecognitionResult res)
    {
        System.out.println("Recognition Reason: " + res.getReason() + ". Text:" + res.getText());
        if (res.getReason() == ResultReason.Canceled)
        {
            CancellationDetails details = CancellationDetails.fromResult(res);
            System.out.println("Cancellation ErrorCode: " + details.getErrorCode() + ". ErrorDetails: " + details.getErrorDetails());
        }
    }

    public static double LevenshteinRatio(String s0, String s1) 
    {
        double percentage = 0;
        s0 = s0.trim().replaceAll("\\s+", " ");
        s1 = s1.trim().replaceAll("\\s+", " ");
        percentage=(double) (1 - (double) LevenshteinDistance(s0, s1) / (double) (s0.length() + s1.length()));
        return percentage;
    }

    public static int LevenshteinDistance(String s0, String s1) {

        int len0 = s0.length() + 1;
        int len1 = s1.length() + 1;  
        // the array of distances
        int[] cost = new int[len0];
        int[] newcost = new int[len0];

        // initial cost of skipping prefix in String s0
        for (int i = 0; i < len0; i++)
            cost[i] = i;

        // dynamically computing the array of distances

        // transformation cost for each letter in s1
        for (int j = 1; j < len1; j++) {

            // initial cost of skipping prefix in String s1
            newcost[0] = j - 1;

            // transformation cost for each letter in s0
            for (int i = 1; i < len0; i++) {

                // matching current letters in both strings
                int match = (s0.charAt(i - 1) == s1.charAt(j - 1)) ? 0 : 1;

                // computing cost for each transformation
                int cost_replace = cost[i - 1] + match;
                int cost_insert = cost[i] + 1;
                int cost_delete = newcost[i - 1] + 1;

                // keep minimum cost
                newcost[i] = Math.min(Math.min(cost_insert, cost_delete),
                        cost_replace);
            }

            // swap cost/newcost arrays
            int[] swap = cost;
            cost = newcost;
            newcost = swap;
        }

        // the distance is the cost for transforming all letters in both strings
        return cost[len0 - 1];
    }

}

package tests;
//

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

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
}

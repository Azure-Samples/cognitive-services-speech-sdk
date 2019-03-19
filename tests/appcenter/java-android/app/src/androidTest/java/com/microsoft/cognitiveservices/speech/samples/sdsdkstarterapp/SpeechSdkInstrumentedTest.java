//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.sdsdkstarterapp;

import android.content.res.AssetManager;
import android.content.Context;
import android.support.annotation.NonNull;
import android.support.test.InstrumentationRegistry;
import android.support.test.filters.LargeTest;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;

import com.microsoft.appcenter.espresso.Factory;
import com.microsoft.appcenter.espresso.ReportHelper;

import org.junit.After;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Calendar;
import java.util.Properties;

import tests.Settings;

import java.nio.charset.Charset;
import java.io.BufferedOutputStream;
import java.io.BufferedWriter;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withText;
import static org.junit.Assert.*;

import com.microsoft.azure.storage.CloudStorageAccount;
import com.microsoft.azure.storage.StorageCredentials;
import com.microsoft.azure.storage.StorageCredentialsSharedAccessSignature;
import com.microsoft.azure.storage.blob.CloudBlobClient;
import com.microsoft.azure.storage.blob.CloudBlobContainer;
import com.microsoft.azure.storage.blob.CloudBlockBlob;
import com.microsoft.azure.storage.blob.ListBlobItem;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
@LargeTest
public class SpeechSdkInstrumentedTest {

    @After
    public void TearDown(){
        reportHelper.label("Stopping App");
    }

    @Rule
    public ReportHelper reportHelper = Factory.getReportHelper();

    @Rule
    public ActivityTestRule<MainActivity> mActivityRule = new ActivityTestRule<>(MainActivity.class);

    @Test
    public void useAppContext() {
        // Context of the app under test.
        Context appContext = InstrumentationRegistry.getTargetContext();

        assertEquals("com.microsoft.cognitiveservices.speech.samples.sdsdkstarterapp", appContext.getPackageName());
    }

    @Test
    public void runSpeechSDKtests() throws ClassNotFoundException, IOException {
        reportHelper.label("Properties loading");
        prepareTestEnvironment("MainActivity.properties");
        reportHelper.label("Expecting wave file at " + Settings.WavFile);
        reportHelper.label("Properties loaded");

        // note: this property, if set, is picked up by the junit results
        System.setProperty("OS_DESCRIPTION", android.os.Build.MANUFACTURER + "-" + android.os.Build.HARDWARE + "-" + android.os.Build.VERSION.RELEASE + "-" + android.os.Build.DISPLAY);

        boolean successful = tests.runner.Runner.mainRunner("tests.AllTests");

        String xmlResults = System.getProperty("TestOutputFilename", null);
        FileInputStream fis = new FileInputStream(xmlResults);
        InputStreamReader isr = new InputStreamReader(fis);
        BufferedReader br = new BufferedReader(isr);

        // Note: the results output is used as exception message in
        //       case some test failed. That way, appcenter records the
        //       data as part of its logging.
        String errMessage = "";
        String line;
        while ((line = br.readLine()) != null && (line.length() > 0)){
            errMessage += "\r\n" + line;
        }

        if (System.getProperty("SasToken", "unconfigured") != "unconfigured") {
            // When upload is configured, it needs to succeeed.
            try {
                reportHelper.label("Unit tests executed, uploading results " + successful);
                uploadTestResults();
            }
            catch(Exception ex) {
                reportHelper.label("Unit tests uploading results failed with " + ex);
                errMessage += "\r\n" + ex;
                successful = false;
            }
        }

        reportHelper.label("Unit tests executed, state " + successful);
        if (!successful) {
            throw new IOException(errMessage);
        }
    }

    @Test
    public void makeSureWeTestTheExpectedApp() {
        // Make sure, our testing app is on the screen.
        onView(withText("Speech SDK Test Shell")).check(matches(isDisplayed()));
    }

    protected static String retrieveAuthorizationToken(String speechRegion, String speechSubscriptionKey) {
        try {
            URL url = new URL("https://"  + speechRegion + ".api.cognitive.microsoft.com/sts/v1.0/issueToken");

            HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
            urlConnection.setRequestProperty("Content-Type", "application/json");
            urlConnection.setRequestProperty("Ocp-Apim-Subscription-Key", speechSubscriptionKey);
            urlConnection.setDoOutput(true);

            OutputStream out = new BufferedOutputStream(urlConnection.getOutputStream());

            BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(out, "UTF-8"));
            writer.write("{}");
            writer.flush();
            writer.close();
            out.close();

            urlConnection.connect();

            InputStreamReader streamReader = new  InputStreamReader(urlConnection.getInputStream());
            BufferedReader reader = new BufferedReader(streamReader);
            StringBuilder stringBuilder = new StringBuilder();
            String inputLine;
            while ((inputLine = reader.readLine()) != null){
                stringBuilder.append(inputLine);
            }
            reader.close();
            streamReader.close();

            return stringBuilder.toString();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

        return null;
    }

    @NonNull
    private static void prepareTestEnvironment(String filename) {
        String tempDir = System.getProperty("java.io.tmpdir", "/data/local/tmp/");
        File file = new File(tempDir, "whatstheweatherlike.wav");
        String waveFileDefault = file.getPath();

        try {
            // Note: updating system properties from a provided asset, if available.
            try {
                Context targetContext = InstrumentationRegistry.getTargetContext();
                AssetManager assets = targetContext.getAssets();

                InputStream inputStream = assets.open(filename);
                Properties properties = new Properties();
                properties.load(inputStream);
                inputStream.close();

                for (Object name: properties.keySet()) {
                    String key = name.toString();
                    System.setProperty(key, properties.getProperty(key));
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            if (System.getProperty("TestOutputFilename", null) == null) {
                String unitTestsResults = new File(tempDir, "test-java-unittests.xml").getCanonicalPath();
                System.setProperty("TestOutputFilename", unitTestsResults);
            }

            waveFileDefault = file.getCanonicalPath();
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (!new File(waveFileDefault).exists()) {
            try {
                Context targetContext = InstrumentationRegistry.getTargetContext();
                AssetManager assets = targetContext.getAssets();

                InputStream inputStream = assets.open("whatstheweatherlike.wav");
                FileOutputStream os = new FileOutputStream(waveFileDefault);

                byte buf[]= new byte[1024];
                int len;
                while ((len = inputStream.read(buf, 0, buf.length))> 0) {
                    os.write(buf, 0, len);
                }

                os.close();
                inputStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        Settings.SpeechSubscriptionKey = System.getProperty("SpeechSubscriptionKey", "<enter your subscription info here>");
        Settings.SpeechRegion = System.getProperty("SpeechRegion", "westus");
        Settings.LuisSubscriptionKey = System.getProperty("LuisSubscriptionKey", "<enter your subscription info here>");
        Settings.LuisRegion = System.getProperty("LuisRegion", "westus2");
        Settings.LuisAppId = System.getProperty("LuisAppId", "<enter your LUIS AppId>");
        Settings.Keyword = System.getProperty("Keyword", "Computer");
        Settings.KeywordModel = System.getProperty("KeywordModel", "kws-computer.zip");

        Settings.WavFile = System.getProperty("SampleAudioInput", waveFileDefault);
        Settings.SpeechAuthorizationToken = retrieveAuthorizationToken(Settings.SpeechRegion, Settings.SpeechSubscriptionKey);

        directoryPrefix = System.getProperty("SasContainerPrefix", "unconfigured");
        sasToken = System.getProperty("SasToken", "unconfigured");
    }

    public static String directoryPrefix = "unconfigured";
    public static String sasToken = "unconfigured";

    public String uploadTestResults() throws Exception {
        String androidId = android.os.Build.DEVICE + "-" + android.os.Build.VERSION.RELEASE + "-" + Calendar.getInstance().getTimeInMillis();
        String imageName = directoryPrefix + (directoryPrefix.endsWith("/") ? "" : "/") + "test-" + androidId;

        String blobLocation = imageName + ".xml";
        reportHelper.label("Unit tests uploading to " + blobLocation);

        CloudBlockBlob imageBlob = new CloudBlockBlob(new java.net.URI(blobLocation), new StorageCredentialsSharedAccessSignature(sasToken));
        String xmlResults = System.getProperty("TestOutputFilename", null);
        if(xmlResults != null) {
            imageBlob.uploadFromFile(xmlResults);
        }

        return imageName;
    }
}

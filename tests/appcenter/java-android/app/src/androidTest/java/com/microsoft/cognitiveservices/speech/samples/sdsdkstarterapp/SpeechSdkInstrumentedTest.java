//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.sdsdkstarterapp;

import android.content.Context;
import android.support.test.InstrumentationRegistry;
import android.support.test.filters.LargeTest;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;

import com.microsoft.appcenter.espresso.Factory;
import com.microsoft.appcenter.espresso.ReportHelper;

import org.junit.After;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Calendar;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withText;
import static org.junit.Assert.*;

import com.microsoft.azure.storage.StorageCredentialsSharedAccessSignature;
import com.microsoft.azure.storage.blob.CloudBlockBlob;

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
        prepareTestEnvironment();

        Settings.LoadSettings();

        reportHelper.label("Expecting wave file at " + Settings.DefaultSettingsMap.get(DefaultSettingsKeys.WavFile));
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
        StringBuilder errMessage = new StringBuilder();
        String line;
        while ((line = br.readLine()) != null && (line.length() > 0)){
            errMessage.append("\r\n").append(line);
        }

        if (!System.getProperty("SasToken", "unconfigured").equals("unconfigured")) {
            // When upload is configured, it needs to succeeed.
            try {
                reportHelper.label("Unit tests executed, uploading results " + successful);
                uploadTestResults();
            }
            catch(Exception ex) {
                reportHelper.label("Unit tests uploading results failed with " + ex);
                errMessage.append("\r\n").append(ex);
                successful = false;
            }
        }

        reportHelper.label("Unit tests executed, state " + successful);
        if (!successful) {
            throw new IOException(errMessage.toString());
        }
    }

    @Ignore("This test will fail occasionally, due to screen is shut down etc.")
    @Test
    public void makeSureWeTestTheExpectedApp() {
        // Make sure, our testing app is on the screen.
        onView(withText("Speech SDK Test Shell")).check(matches(isDisplayed()));
    }

    private static void prepareTestEnvironment() {
        String tempDir = System.getProperty("java.io.tmpdir", "/data/local/tmp/");
        File file = new File(tempDir, "audio/whatstheweatherlike.wav");
        String waveFileDefault;

        if (System.getProperty("TestOutputFilename", null) == null) {
            try {
                String unitTestsResults = new File(tempDir, "test-java-unittests.xml").getCanonicalPath();
                System.setProperty("TestOutputFilename", unitTestsResults);
            } catch(IOException ex) {
                System.out.println("getCanonicalPath threw an IO exception: " + ex.getMessage());
            }
        }

        try {
            waveFileDefault = file.getCanonicalPath();
            System.out.println("Setting waveFileDefault to " + waveFileDefault);
        } catch(IOException ex) {
            System.out.println("getCanonicalPath threw an IO exception: " + ex.getMessage());
        }

        directoryPrefix = System.getProperty("SasContainerPrefix", "unconfigured");
        sasToken = System.getProperty("SasToken", "unconfigured");
    }

    private static String directoryPrefix = "unconfigured";
    private static String sasToken = "unconfigured";

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

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.sdsdkstarterapp;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.WindowManager;

import java.io.*;
import java.util.Properties;
import java.util.zip.*;
import java.nio.file.Paths;
import java.nio.charset.StandardCharsets;

import com.microsoft.appcenter.AppCenter;
import com.microsoft.appcenter.analytics.Analytics;
import com.microsoft.appcenter.crashes.Crashes;
import com.microsoft.cognitiveservices.speech.samples.sdsdkstarterapp.Settings;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        AppCenter.start(getApplication(), "7a6be3d4-8645-4fb6-ae6e-cf58dc3e04d8", Analytics.class, Crashes.class);

        // save the asset manager
        final AssetManager assets = this.getAssets();
        prepareEnvironment(assets, "MainActivity.properties");
    }

    private static void extractAll(InputStream zipStream, String outputBaseFolder) throws IOException {
        byte buf[] = new byte[1024];
        String baseDir = Paths.get(new File(outputBaseFolder).getCanonicalPath(), "audio").toString();

        new File(baseDir).mkdir();

        ZipInputStream zis = new ZipInputStream(zipStream);
        ZipEntry ze = zis.getNextEntry();
        while(ze != null) {
            String filename = ze.getName();

            // work-around non-standards conforming .zip created with older .NET versions.
            // See [https://docs.microsoft.com/dotnet/framework/migration-guide/mitigation-ziparchiveentry-fullname-path-separator](https://docs.microsoft.com/dotnet/framework/migration-guide/mitigation-ziparchiveentry-fullname-path-separator)
            if (filename != null) {
                filename = filename.replace('\\', '/');

                File outFile = new File(baseDir, filename);
                String fullPathName = outFile.getCanonicalPath();
                if(!fullPathName.startsWith(baseDir)) {
                    throw new SecurityException("illegal path");
                }

                File parent = new File(outFile.getParent());

                if(!parent.exists()) {
                    parent.mkdirs();
                }

                FileOutputStream fos = new FileOutputStream(outFile);

                int len;
                while ((len = zis.read(buf)) > 0) {
                    fos.write(buf, 0, len);
                }

                fos.close();
            }
            else {
                int len;
                while ((len = zis.read(buf)) > 0) {
                }
            }

            ze = zis.getNextEntry();
        }
    }

    private static void prepareEnvironment(AssetManager assets, String filename) {
        // Note: updating system properties from a provided asset, if available.
        try {
            Properties properties = new Properties();

            InputStream inputStream = assets.open(filename);
            if (inputStream != null) {
                properties.load(inputStream);
                inputStream.close();
            }

            for (Object name: properties.keySet()) {
                String key = name.toString();
                System.setProperty(key, properties.getProperty(key));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        String tempDir = System.getProperty("java.io.tmpdir", "/data/local/tmp/");

        System.setProperty("JsonConfigPath", tempDir);

        String AudioInputDirectory =  System.getProperty("AudioInputDirectory", tempDir);
        String SampleAudioInput = System.getProperty("SampleAudioInput", AudioInputDirectory + "/kws-computer.wav");
        if (!new File(SampleAudioInput).exists()) {
            System.setProperty("AudioInputDirectory", tempDir);

            try {
                InputStream inputStream = assets.open("testassets.zip");

                if (inputStream != null) {
                    extractAll(inputStream, tempDir);
                    inputStream.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        // Read in the json settings
        String defaultSettings = loadJson(assets, "test.defaults.json");
        System.out.println("Loaded defaultSettings with: " + defaultSettings);
        System.setProperty("defaultSettings", defaultSettings);

        String audioUtterances = loadJson(assets, "test.audio.utterances.json");
        System.out.println("Loaded audioUtterances with: " + audioUtterances);
        System.setProperty("audioUtterances", audioUtterances);

        String subscriptionsRegions = loadJson(assets, "test.subscriptions.regions.json");
        System.out.println("Loaded subscriptionsRegions with: " + subscriptionsRegions);
        System.setProperty("subscriptionsRegions", subscriptionsRegions);
    }

    private static String loadJson(AssetManager assets, String jsonFile) {
        InputStream inputStream = null;
        try {
            inputStream = assets.open(jsonFile);
        } catch (IOException ex) {
            System.out.println("assets.open threw an io exception: " + ex.getMessage());
        }

        String json = null;
        if (inputStream != null) {
            try {
                int size = inputStream.available();
                byte[] buffer = new byte[size];
                inputStream.read(buffer);
                inputStream.close();
                json = new String(buffer, "UTF-8");
            } catch (IOException ex) {
                ex.printStackTrace();
                System.out.println("loadJson threw an  io exception: " + ex.getMessage());
            }
        }

        return json;
    }
}

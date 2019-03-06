//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.sdsdkstarterapp;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

import com.microsoft.appcenter.AppCenter;
import com.microsoft.appcenter.analytics.Analytics;
import com.microsoft.appcenter.crashes.Crashes;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        AppCenter.start(getApplication(), "7a6be3d4-8645-4fb6-ae6e-cf58dc3e04d8", Analytics.class, Crashes.class);

        // save the asset manager
        final AssetManager assets = this.getAssets();

        prepareEnvironment(assets, "MainActivity.properties");
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

        String SampleAudioInput = System.getProperty("SampleAudioInput", "/data/keyword/kws-computer.wav");
        if (!new File(SampleAudioInput).exists()) {
            try {
                InputStream inputStream = assets.open("whatstheweatherlike.wav");

                if (inputStream != null) {
                    String tempDir = System.getProperty("java.io.tmpdir", "/data/data");
                    File file = new File(tempDir, "whatstheweatherlike.wav");
                    FileOutputStream os = new FileOutputStream(file);

                    if (os != null) {
                        byte buf[]= new byte[1024];
                        int len;
                        while ((len = inputStream.read(buf, 0, buf.length))> 0) {
                            os.write(buf, 0, len);
                        }

                        os.close();
                    }

                    inputStream.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}

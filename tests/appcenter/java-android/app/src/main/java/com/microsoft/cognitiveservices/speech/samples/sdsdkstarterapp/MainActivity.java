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
import java.util.zip.*;

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

    private static void extractAll(InputStream zipStream, String outputBaseFolder) throws IOException {
        byte buf[] = new byte[1024];
        String baseDir = new File(outputBaseFolder).getCanonicalPath();

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
    }
}

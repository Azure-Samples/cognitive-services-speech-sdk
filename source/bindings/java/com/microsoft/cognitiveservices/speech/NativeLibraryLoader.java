//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.BufferedReader;
import java.io.Closeable;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOError;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.file.Files;
import java.util.ArrayList;

/**
 * A helper class for loading native Speech SDK libraries from Java
 *
 * The jar with the Speech native libraries may contain a file name 'NATIVE_MANIFEST' that lists
 * all native files (one per line, full name) to be extracted.
 * The libraries will be loaded in the order specified in the manifest and are expected in the
 * folders named as the operating system they run on. Supported OSes are Linux, Windows, Mac.
 *
 * In case the above manifest does not exist, a fallback is implemented loading a fixed set
 * of libraries, i.e., the Speech.core, and the Speech.java.binding library (in that order).
 */
class NativeLibraryLoader {

    private static final String manifestName = "NATIVE_MANIFEST";
    private static String[] nativeList = new String[0];
    private static Boolean extractionDone = false;
    private static File tempDir;

    static {
        try {
            tempDir = Files.createTempDirectory("speech-sdk-native-").toFile();
            tempDir.deleteOnExit();
        }
        catch (IOException e) {
            throw new IOError(e);
        }
    }

    /**
     * Loads binding native libraries from the jar file, if the jar contains a plain text file
     * named 'NATIVE_MANIFEST'.
     *
     * The NATIVE_MANIFEST contains what libraries need to be extracted (one per line, full name)
     * and the order in which they should be loaded.
     *
     * If NATIVE_MANIFEST is not found, a platform specific set of libraries and their order
     * is assumed (comprising at least the core library and the java.binding library).
     *
     * The libraries must be located under the "/ASSETS/" root, followed by the operating system name.
     */
    public static void loadNativeBinding() {
        try {
            extractNativeLibraries();

            // Load the libraries in the order provided by the NATIVE_MANIFEST
            for (String libName: nativeList) {
                String fullPathName = new File(tempDir.getCanonicalPath(), libName).getCanonicalPath();
                if(!fullPathName.startsWith(tempDir.getCanonicalPath())) {
                    throw new SecurityException("illegal path");
                }

                System.load(fullPathName);
            }
        }
        catch (Exception e) {
            // If nothing worked, throw exception
            throw new UnsatisfiedLinkError(
                    String.format("Could not load all Speech SDK libraries because we encountered the following error: %s", e.getMessage()));
        }
    }

    private static void extractNativeLibraries() throws IOException {
        try {
            if (!extractionDone) {
                nativeList = getResourceLines(manifestName);

                // Extract all operatingSystem specific native libraries to temporary location
                for (String libName: nativeList) {
                    extractResourceFromPath(libName, getResourcesPath());
                }
            }
        }
        finally {
            extractionDone = true;
        }
    }

    private static String[] getResourceLines(String resourceName) throws IOException {
        // Read resource file if it exists
        InputStream inStream = SpeechConfig.class.getResourceAsStream(getResourcesPath() + resourceName);
        BufferedReader resourceReader = null;
        ArrayList<String> lines = new ArrayList<String>();

        try {
            // in case there is no embedded jni-manifest, use fixed names
            if (inStream == null) {
                String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();

                if (operatingSystem.contains("linux")) {
                    return new String[] {
                            "libMicrosoft.CognitiveServices.Speech.core.so",
                            "libMicrosoft.CognitiveServices.Speech.extension.kws.so",
                            "libMicrosoft.CognitiveServices.Speech.java.bindings.so"
                    };
                }
                else if (operatingSystem.contains("windows")) {
                    return new String[] {
                            "Microsoft.CognitiveServices.Speech.core.dll",
                            "Microsoft.CognitiveServices.Speech.extension.kws.dll",
                            "Microsoft.CognitiveServices.Speech.java.bindings.dll"
                    };
                }
                else if (operatingSystem.contains("mac") || operatingSystem.contains("darwin")) {
                    // Note: currently no KWS on macOS
                    return new String[] {
                            "libMicrosoft.CognitiveServices.Speech.core.dylib",
                            "libMicrosoft.CognitiveServices.Speech.java.bindings.jnilib"
                    };
                }

                throw new UnsatisfiedLinkError(
                        String.format("The Speech SDK doesn't currently have native support for operating system: %s", operatingSystem));
            }

            resourceReader = new BufferedReader(new InputStreamReader(inStream, "UTF-8"));

            int numLibs = 0;
            final int maxLibNameLength = 128;
            for (String line; (numLibs < 10) && (line = resourceReader.readLine()) != null; numLibs++) {
                // strip all whitespace at the endings.
                line = line.trim();

                // validate input
                if(line.isEmpty() || line.length() > maxLibNameLength || line.indexOf('\n') >= 0) {
                    continue;
                }

                lines.add(line);
            }
        }
        finally {
            safeClose(resourceReader);
            safeClose(inStream);
        }

        return lines.toArray(new String[lines.size()]);
    }

    private static String getResourcesPath() {
        String operatingSystem = System.getProperty("os.name");
        operatingSystem  = (operatingSystem != null) ? operatingSystem.toLowerCase() : "unknown";

        String speechPrefix = "/ASSETS/%s%s/";

        // determine if the VM runs on 64 or 32 bit
        String dataModelSize = System.getProperty("sun.arch.data.model");
        if(dataModelSize != null && dataModelSize.equals("64")) {
            dataModelSize = "64";
        }
        else {
            dataModelSize = "32";
        }

        if (operatingSystem.contains("linux")) {
            return String.format(speechPrefix, "linux", dataModelSize);
        }
        else if (operatingSystem.contains("windows")) {
            return String.format(speechPrefix, "windows", dataModelSize);
        }
        else if (operatingSystem.contains("mac")|| operatingSystem.contains("darwin")) {
            return String.format(speechPrefix, "mac", dataModelSize);
        }
        else {
            throw new UnsatisfiedLinkError(
                    String.format("The Speech SDK doesn't currently have native support for operating system: %s data model size %s", operatingSystem, dataModelSize));
        }
    }

    private static void extractResourceFromPath(String libName, String prefix) throws IOException {
        File temp = new File(tempDir.getCanonicalPath(), libName);
        if (!temp.getCanonicalPath().startsWith(tempDir.getCanonicalPath())) {
            throw new SecurityException("illegal name " + temp.getCanonicalPath());
        }

        temp.createNewFile();
        temp.deleteOnExit();

        if (!temp.exists()) {
            throw new FileNotFoundException(String.format(
                    "Temporary file %s could not be created. Make sure you can write to this location.",
                    temp.getCanonicalPath()));
        }

        String path = prefix + libName;
        InputStream inStream = SpeechConfig.class.getResourceAsStream(path);
        if (inStream == null) {
            throw new FileNotFoundException(String.format("Could not find resource %s in jar.", path));
        }

        FileOutputStream outStream = null;
        byte[] buffer = new byte[1024*1024];
        int bytesRead;

        try {
            outStream = new FileOutputStream(temp);
            while ((bytesRead = inStream.read(buffer)) >= 0) {
                outStream.write(buffer, 0, bytesRead);
            }
        } finally {
            safeClose(outStream);
            safeClose(inStream);
        }
    }

    private static void safeClose(Closeable is) {
        if (is != null) {
            try {
                is.close();
            } catch (IOException e) {
                // ignored.
            }
        }
    }
}

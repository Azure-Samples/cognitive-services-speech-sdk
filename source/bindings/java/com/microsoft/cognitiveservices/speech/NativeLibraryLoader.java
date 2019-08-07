//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOError;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

/**
 * A helper class for loading native Speech SDK libraries from Java
 *
 * - Native libraries are placed in an OS-specific package assets folder
 *   (/ASSETS/{mac,linux,window}64)
 * - From this folder, a known list of libraries is extracted into a temporary folder.
 * - On Windows and OSX, these libraries are loaded in the list order (bottom
 *   to top dependency), by full extracted path name.
 *   TODO this may need to be revisited on OSX
 * - On Linux, only the last of these libraries (top dependency) is loaded,
 *   which resolves static dependency via RPATH; dynamic dependencies will be
 *   loaded later, also via RPATH.
 * - On Windows, optional external native libraries (such as pma extension) are
 *   also copied from the folder where user's runnable jar is present to a temporary
 *   folder. If these optional external libraries are not found, no errors are reported
 *   and no exceptions are thrown.
 */
class NativeLibraryLoader {

    private static String[] nativeList = new String[0];
    private static String[] externalNativeList = new String[0];
    private static Boolean extractionDone = false;
    private static Boolean loadAll = false;
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
     * Extract and load OS-specific libraries from the JAR file.
     */
    public static void loadNativeBinding() {
        try {
            extractNativeLibraries();

            // Load extracted libraries (either all or the last one)
            for (int i = 0; i < nativeList.length; i++)
            {
                if (loadAll || (i == nativeList.length - 1)) {
                    String libName = nativeList[i];
                    String fullPathName = new File(tempDir.getCanonicalPath(), libName).getCanonicalPath();
                    if(!fullPathName.startsWith(tempDir.getCanonicalPath())) {
                        throw new SecurityException("illegal path");
                    }
                    System.load(fullPathName);
                }
            }
        }
        catch (Exception e) {
            // If nothing worked, throw exception
            throw new UnsatisfiedLinkError(
                    String.format("Could not extract/load all Speech SDK libraries because we encountered the following error: %s", e.getMessage()));
        }
    }

    private static void extractNativeLibraries() throws Exception {
        try {
            if (!extractionDone) {
                nativeList = getResourceLines();

                // Extract all operatingSystem specific native libraries to temporary location
                for (String libName: nativeList) {
                    extractResourceFromPath(libName, getResourcesPath());
                }

                externalNativeList = getExternalResourceLines();

                // For Windows, copy external native libraries to temporary location if they exist in current directory
                String osName = System.getProperty("os.name");
                if (osName != null && osName.toLowerCase().equals("windows")) {
                    String path = null;
                    try {
                        path = new File(ClassLoader.getSystemClassLoader().getResource(".").getPath()).getAbsolutePath();
                    }
                    catch (Exception e) {
                        // Ignore all exceptions.
                    }
                    if (path != null) {
                        for (String libName: externalNativeList) {
                            copyLibraryFromPath(libName, path);
                        }
                    }
                }
            }
        }
        finally {
            extractionDone = true;
        }
    }

    /**
     * Depending on the OS, return the list of libraries to be extracted, the
     * first of which is to be loaded as well.
     */
    private static String[] getResourceLines() throws IOException {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();

        if (operatingSystem.contains("linux")) {
            return new String[] {
                    "libMicrosoft.CognitiveServices.Speech.core.so",
                    "libMicrosoft.CognitiveServices.Speech.extension.kws.so",
                    "libMicrosoft.CognitiveServices.Speech.extension.codec.so",
                    "libMicrosoft.CognitiveServices.Speech.java.bindings.so"
            };
        }
        else if (operatingSystem.contains("windows")) {
            return new String[] {
                    "Microsoft.CognitiveServices.Speech.core.dll",
                    // Note: the Speech SDK core library loads extension DLLs
                    // relative to its location, so this one is currently only
                    // needed for extraction (TODO however due to 'loadAll ==
                    // true' below, we'll still load it later; should fix).
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

    /**
     * Depending on the OS, return the list of external optional libraries to be copied.
     */
    private static String[] getExternalResourceLines() {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();

        if (operatingSystem.contains("windows")) {
            return new String[] {
                    "Microsoft.CognitiveServices.Speech.extension.pma.dll"
            };
        }

        return new String[0];
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
            loadAll = true; // signal to load all libraries
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

    private static void copyLibraryFromPath(String libName, String sourcePath) throws Exception {
        File sourceFile = new File(sourcePath, libName);
        if (!sourceFile.exists()) {
            return;
        }

        File targetFile = new File(tempDir.getCanonicalPath(), libName);
        if (sourceFile.getCanonicalPath().equals(targetFile.getCanonicalPath())) {
            return;
        }
        targetFile.deleteOnExit();
        Files.copy(sourceFile.toPath(), targetFile.toPath(), StandardCopyOption.REPLACE_EXISTING);
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

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

    private static class NativeLibrary {
        private String name;
        private boolean required;

        public NativeLibrary(String name, boolean required) {
            this.name = name;
            this.required = required;
        }

        public String getName() { return name; }
        public boolean getRequired() { return required; }

        private void setName() { this.name = name; }
        private void setRequired() { this.required = required; }
    }

    private static NativeLibrary[] nativeList = new NativeLibrary[0];
    private static NativeLibrary[] externalNativeList = new NativeLibrary[0];
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
                    String libName = nativeList[i].getName();
                    String fullPathName = new File(tempDir.getCanonicalPath(), libName).getCanonicalPath();
                    if(!fullPathName.startsWith(tempDir.getCanonicalPath())) {
                        throw new SecurityException("illegal path");
                    }
                    try {
                        System.load(fullPathName);
                    }
                    catch (UnsatisfiedLinkError e) {
                        // Required library
                        if (nativeList[i].getRequired() == true) {
                            throw new UnsatisfiedLinkError(
                            String.format("Could not load a required Speech SDK library because of the following error: %s", e.getMessage()));
                        }
                        // Optional library; just print a message
                        else {
                            System.err.println(
                            String.format("Could not load an optional Speech SDK library because of the following error: %s", e.getMessage()));
                        }
                    }
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
                for (NativeLibrary library: nativeList) {
                    extractResourceFromPath(library, getResourcesPath());
                }

                externalNativeList = getExternalResourceLines();

                // For Windows, copy external native libraries to temporary location if they exist in current directory
                String osName = System.getProperty("os.name");
                if (osName != null && osName.toLowerCase().contains("windows")) {
                    String path = null;
                    try {
                        path = new File(ClassLoader.getSystemClassLoader().getResource(".").getPath()).getAbsolutePath();
                        if (path != null) {
                            for (NativeLibrary library: externalNativeList) {
                                copyLibraryFromPath(library, path);
                            }
                        }
                    }
                    catch (Exception e) {
                        System.err.println(
                        String.format("Could not copy external Speech SDK libraries because of the following error: %s", e.getMessage()));
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
    private static NativeLibrary[] getResourceLines() throws IOException {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();

        if (operatingSystem.contains("linux")) {
            return new NativeLibrary[] {
                    new NativeLibrary("libMicrosoft.CognitiveServices.Speech.core.so", true),
                    new NativeLibrary("libMicrosoft.CognitiveServices.Speech.extension.kws.so", false),
                    new NativeLibrary("libMicrosoft.CognitiveServices.Speech.extension.codec.so", false),
                    new NativeLibrary("libMicrosoft.CognitiveServices.Speech.java.bindings.so", true)
            };
        }
        else if (operatingSystem.contains("windows")) {
            return new NativeLibrary[] {
                    new NativeLibrary("Microsoft.CognitiveServices.Speech.core.dll", true),
                    // Note: the Speech SDK core library loads extension DLLs
                    // relative to its location, so this one is currently only
                    // needed for extraction (TODO however due to 'loadAll ==
                    // true' below, we'll still load it later; should fix).
                    new NativeLibrary("Microsoft.CognitiveServices.Speech.extension.kws.dll", false),
                    new NativeLibrary("Microsoft.CognitiveServices.Speech.java.bindings.dll", true)
            };
        }
        else if (operatingSystem.contains("mac") || operatingSystem.contains("darwin")) {
            // Note: currently no KWS on macOS
            return new NativeLibrary[] {
                    new NativeLibrary("libMicrosoft.CognitiveServices.Speech.core.dylib", true),
                    new NativeLibrary("libMicrosoft.CognitiveServices.Speech.java.bindings.jnilib", true)
            };
        }

        throw new UnsatisfiedLinkError(
                String.format("The Speech SDK doesn't currently have native support for operating system: %s", operatingSystem));
    }

    /**
     * Depending on the OS, return the list of external optional libraries to be copied.
     */
    private static NativeLibrary[] getExternalResourceLines() {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();

        if (operatingSystem.contains("windows")) {
            return new NativeLibrary[] {
                    new NativeLibrary("Microsoft.CognitiveServices.Speech.extension.pma.dll", false)
            };
        }

        return new NativeLibrary[0];
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
            String osArchitecture = System.getProperty("os.arch");
            if (osArchitecture.contains("aarch64")) {
                return String.format(speechPrefix, "linux-arm", dataModelSize);
            }
            else if (osArchitecture.contains("amd64") || osArchitecture.contains("x86_64")) {
                return String.format(speechPrefix, "linux-x", dataModelSize);
            }
        }
        else if (operatingSystem.contains("windows")) {
            loadAll = true; // signal to load all libraries
            return String.format(speechPrefix, "windows", dataModelSize);
        }
        else if (operatingSystem.contains("mac")|| operatingSystem.contains("darwin")) {
            return String.format(speechPrefix, "mac", dataModelSize);
        }

        throw new UnsatisfiedLinkError(
                String.format("The Speech SDK doesn't currently have native support for operating system: %s data model size %s", operatingSystem, dataModelSize));
    }

    private static void extractResourceFromPath(NativeLibrary library, String prefix) throws IOException {
        String libName = library.getName();
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
            if (library.getRequired() == true) {
                throw new FileNotFoundException(String.format("Could not find resource %s in jar.", path));
            }
            else {
                // Optional library; just print a message
                System.err.println(String.format("Could not find optional resource %s in jar.", path));
                return;
            }
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

    private static void copyLibraryFromPath(NativeLibrary library, String sourcePath) throws Exception {
        String libName = library.getName();
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

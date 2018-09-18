//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.BufferedInputStream;
import java.io.Closeable;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import com.microsoft.cognitiveservices.speech.util.Contracts;


/**
 * Represents a keyword recognition model for recognizing when
 * the user says a keyword to initiate further speech recognition.
 */
public class KeywordRecognitionModel implements Closeable
{
    // load the native library.
    static {
        @SuppressWarnings("unused")
        Class<?> speechFactorLoadTrigger = SpeechConfig.speechConfigClass;
    }

    /**
     * Creates a keyword recognition model using the specified filename.
     * Note: keyword spotting functionality is only available in the Cognitive Services Speech Devices SDK.
     * @param fileName A string that represents file name for the keyword recognition model.
     *                 Note, the file can point to a zip file in which case the model will be extracted from the zip.
     * @return The keyword recognition model being created.
     */
    public static KeywordRecognitionModel fromFile(String fileName) {
        Contracts.throwIfFileDoesNotExist(fileName, "fileName");

        try {
            File f = new File(fileName);
            InputStream inputStream = new BufferedInputStream(new FileInputStream(new File(fileName)));
            byte[] magic = new byte[2];

            // try to read the header, reset afterwards.
            inputStream.mark(4);
            int len = inputStream.read(magic);
            inputStream.reset();

            // PK indicates its a zip file
            boolean isZipped = (len == 2) && (magic[0] == 0x50) && (magic[1] == 0x4b);

            KeywordRecognitionModel ret = null;
            if(isZipped) {
                ret = fromStream(inputStream, f.getName(), isZipped);
                inputStream.close();
            }
            else {
                // if not zipped, just take the original file.
                inputStream.close();
                ret = new KeywordRecognitionModel(com.microsoft.cognitiveservices.speech.internal.KeywordRecognitionModel.FromFile(f.getAbsolutePath()));
            }

            return ret;
        }
        catch(FileNotFoundException ex) {
            throw new IllegalArgumentException("fileName not found");
        }
        catch(IOException ex) {
            throw new IllegalArgumentException("could not access file " + ex.toString());
        }
    }

    /**
     * Creates a keyword recognition model using the specified input stream.
     * @param inputStream A stream that represents data for the keyword recognition model.
     *                 Note, the file can be a zip file in which case the model will be extracted from the zip.
     * @param name The name of the kws.
     * @param isZipped If true, the input stream is treated as a zip. false, if the input is just the kws table file.
     * @return The keyword recognition model being created.
     * @throws IOException If the name of the kws contains a file separator
     */
    public static KeywordRecognitionModel fromStream(InputStream inputStream, String name, boolean isZipped) throws IOException {
        Contracts.throwIfNull(inputStream, "inputStream");
        Contracts.throwIfNullOrWhitespace(name, "name");

        if(name.contains(File.separator)) {
            throw new IOException("name must not contain separator");
        }

        String tempFolder = System.getProperty("java.io.tmpdir");
        Contracts.throwIfNullOrWhitespace(tempFolder, "tempFolder");

        File kwsRootDirectory = new File(tempFolder + File.separator + "speech-sdk-keyword-" + name);
        if(!kwsRootDirectory.exists()) {
            kwsRootDirectory.mkdirs();
            kwsRootDirectory.deleteOnExit();

            if(!kwsRootDirectory.isDirectory()) {
                throw new IllegalArgumentException("path is not a directory");
            }

            int len;
            byte[] buffer = new byte[1024*1024];
            if (isZipped) {
                ZipInputStream zip = new ZipInputStream(inputStream);
                ZipEntry zipEntry;

                while((zipEntry = zip.getNextEntry()) != null) {
                    if(zipEntry.isDirectory())
                        continue;

                    String zipEntryName = zipEntry.getName();
                    Contracts.throwIfNullOrWhitespace(zipEntryName, "zipEntry.name");

                    File outputFile = new File(kwsRootDirectory, zipEntryName);
                    outputFile.deleteOnExit();

                    FileOutputStream outputStream = new FileOutputStream(outputFile);
                    while((len = zip.read(buffer)) > 0) {
                        outputStream.write(buffer, 0, len);
                    }

                    outputStream.close();
                }

                zip.close();
            }
            else {
                FileOutputStream outputStream = new FileOutputStream(new File(kwsRootDirectory, "kws.table"));
                while((len = inputStream.read(buffer)) > 0) {
                    outputStream.write(buffer, 0, len);
                }

                outputStream.close();
            }
        }

        File kwsTableFile = new File(kwsRootDirectory, "kws.table");
        if (!kwsTableFile.exists() || !kwsTableFile.isFile()) {
            throw new IllegalArgumentException("zip did not contain kws.table");
        }

        return new KeywordRecognitionModel(com.microsoft.cognitiveservices.speech.internal.KeywordRecognitionModel.FromFile(kwsTableFile.getAbsolutePath()));
    }

    /**
     * Dispose of associated resources.
     */
    @Override
    public void close()
    {
        if (disposed)
        {
            return;
        }

        disposed = true;
    }

    private boolean disposed = false;

    KeywordRecognitionModel(com.microsoft.cognitiveservices.speech.internal.KeywordRecognitionModel model)
    {
        Contracts.throwIfNull(model, "model");

        modelImpl = model;
    }

    private com.microsoft.cognitiveservices.speech.internal.KeywordRecognitionModel modelImpl;
    /**
     * Returns the keyword recognition model.
     * @return The implementation of the model.
     */
    public com.microsoft.cognitiveservices.speech.internal.KeywordRecognitionModel getModelImpl()
    {
        return modelImpl;
    }
}

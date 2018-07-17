package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import com.microsoft.cognitiveservices.speech.util.Contracts;


/**
  * Represents language understanding model used for intent recognition.
  */
public class KeywordRecognitionModel implements Closeable
{ 
    // load the native library.
    static {
        // TODO name of library will depend on version
        System.loadLibrary("Microsoft.CognitiveServices.Speech.java.bindings");
    }

    /**
      * Creates an language understanding model using the specified endpoint.
      * @param fileName A string that represents file name for the keyword recognition model.
      * @return The keyword recognition model being created.
      */
    public static KeywordRecognitionModel fromFile(String fileName)
    {
        Contracts.throwIfNullOrWhitespace(fileName, "fileName");

        return new KeywordRecognitionModel(com.microsoft.cognitiveservices.speech.internal.KeywordRecognitionModel.FromFile(fileName));
    }

    /**
      * Dispose of associated resources.
      */
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
      * Returns the language understanding model.
      * @return The implementation of the model.
      */
    public com.microsoft.cognitiveservices.speech.internal.KeywordRecognitionModel getModelImpl()
    {
        return modelImpl;
    }
}

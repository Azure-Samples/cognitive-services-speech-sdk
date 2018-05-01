package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;


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
        return new KeywordRecognitionModel(com.microsoft.cognitiveservices.speech.internal.KeywordRecognitionModel.fromFile(fileName));
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
        modelImpl = model;
    }

    public com.microsoft.cognitiveservices.speech.internal.KeywordRecognitionModel modelImpl;
    com.microsoft.cognitiveservices.speech.internal.KeywordRecognitionModel getmodelImpl()
    {
        return modelImpl;
    }
}

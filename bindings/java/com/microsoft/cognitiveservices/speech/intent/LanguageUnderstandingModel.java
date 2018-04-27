package com.microsoft.cognitiveservices.speech.intent;

import java.io.Closeable;


/**
  * Represents language understanding model used for intent recognition.
  */
public class LanguageUnderstandingModel implements Closeable
{ 
    /**
      * Creates an language understanding model using the specified endpoint.
      * @param uri A String that represents the endpoint of the language understanding model.
      * @return The language understanding model being created.
      */
    public static LanguageUnderstandingModel from(String uri)
    {
        return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.from(uri));
    }

    /**
      * Creates an language understanding model using subscription key and application id of Language Understanding service.
      * @param subscriptionKey A String that represents the subscription key.
      * @param appId A String that represents the application id of Language Understanding service.
      * @return The language understanding model being created.
      */
    public static LanguageUnderstandingModel from(String subscriptionKey, String appId)
    {
        return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.from(subscriptionKey, appId));
    }

    /**
      * Creates an language understanding model using hostname, subscription key and application id of Language Understanding service.
      * @param hostName A String that represents the host name of of Language Understanding service.
      * @param subscriptionKey A String that represents the subscription key of Language Understanding service.
      * @param appId A String that represents the application id of Language Understanding service.
      * @return The language understanding model being created.
      */
    public static LanguageUnderstandingModel from(String hostName, String subscriptionKey, String appId)
    {
        return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.from(hostName, subscriptionKey, appId));
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

    LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel model)
    {
        modelImpl = model;
    }

    public com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel modelImpl;
    com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel getmodelImpl()
    {
        return modelImpl;
    }
}

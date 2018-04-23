package com.microsoft.cognitiveservices.speech.recognition;

import java.io.Closeable;


/**
  * Represents intent model used for intent recognition.
  */
public class IntentModel implements Closeable
{ 
    /**
      * Creates an intent model using the specified endpoint.
      * @param uri A String that represents the endpoint of the intent model.
      * @return The intent model being created.
      */
    public static IntentModel from(String uri)
    {
        return new IntentModel(com.microsoft.cognitiveservices.speech.internal.LuisModel.from(uri));
    }

    /**
      * Creates an intent model using subscription key and application id of Language Understanding service.
      * @param subscriptionKey A String that represents the subscription key.
      * @param appId A String that represents the application id of Language Understanding service.
      * @return The intent model being created.
      */
    public static IntentModel from(String subscriptionKey, String appId)
    {
        return new IntentModel(com.microsoft.cognitiveservices.speech.internal.LuisModel.from(subscriptionKey, appId));
    }

    /**
      * Creates an intent model using hostname, subscription key and application id of Language Understanding service.
      * @param hostName A String that represents the host name of of Language Understanding service.
      * @param subscriptionKey A String that represents the subscription key of Language Understanding service.
      * @param appId A String that represents the application id of Language Understanding service.
      * @return The intent model being created.
      */
    public static IntentModel from(String hostName, String subscriptionKey, String appId)
    {
        return new IntentModel(com.microsoft.cognitiveservices.speech.internal.LuisModel.from(hostName, subscriptionKey, appId));
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

    IntentModel(com.microsoft.cognitiveservices.speech.internal.LuisModel model)
    {
        modelImpl = model;
    }

    public com.microsoft.cognitiveservices.speech.internal.LuisModel modelImpl;
    com.microsoft.cognitiveservices.speech.internal.LuisModel getmodelImpl()
    {
        return modelImpl;
    }
}

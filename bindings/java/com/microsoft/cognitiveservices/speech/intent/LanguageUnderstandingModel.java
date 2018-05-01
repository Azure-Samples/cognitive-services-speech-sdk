package com.microsoft.cognitiveservices.speech.intent;


/**
  * Represents language understanding model used for intent recognition.
  */
public final class LanguageUnderstandingModel
{ 
    /**
      * Creates an language understanding model using the specified endpoint.
      * @param uri A String that represents the endpoint of the language understanding model.
      * @return The language understanding model being created.
      */
    public static LanguageUnderstandingModel fromEndpoint(String uri)
    {
        return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.fromEndpoint(uri));
    }

    /**
      * Creates an language understanding model using hostname, subscription key and application id of Language Understanding service.
      * @param subscriptionKey A String that represents the subscription key of Language Understanding service.
      * @param appId A String that represents the application id of Language Understanding service.
      * @param region A String that represents the region of the Language Understanding service.
      * @return The language understanding model being created.
      */
    public static LanguageUnderstandingModel fromSubscription(String subscriptionKey, String appId, String region)
    {
        return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.fromSubscription(subscriptionKey, appId, region));
    }

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

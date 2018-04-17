package Carbon.Recognition;

import java.io.Closeable;


/// <summary>
/// Represents intent model used for intent recognition.
/// </summary>
public class IntentModel implements Closeable
{ 
    /// <summary>
    /// Creates an intent model using the specified endpoint.
    /// </summary>
    /// <param name="uri">A String that represents the endpoint of the intent model.</param>
    /// <returns>The intent model being created.</returns>
    public static IntentModel from(String uri)
    {
        return new IntentModel(Carbon.Internal.LuisModel.from(uri));
    }

    /// <summary>
    /// Creates an intent model using subscription key and application id of Language Understanding service.
    /// </summary>
    /// <param name="subscriptionKey">A String that represents the subscription key.</param>
    /// <param name="appId">A String that represents the application id of Language Understanding service.</param>
    /// <returns>The intent model being created.</returns>
    public static IntentModel from(String subscriptionKey, String appId)
    {
        return new IntentModel(Carbon.Internal.LuisModel.from(subscriptionKey, appId));
    }

    /// <summary>
    /// Creates an intent model using hostname, subscription key and application id of Language Understanding service.
    /// </summary>
    /// <param name="hostName">A String that represents the host name of of Language Understanding service.</param>
    /// <param name="subscriptionKey">A String that represents the subscription key of Language Understanding service.</param>
    /// <param name="appId">A String that represents the application id of Language Understanding service.</param>
    /// <returns>The intent model being created.</returns>
    public static IntentModel from(String hostName, String subscriptionKey, String appId)
    {
        return new IntentModel(Carbon.Internal.LuisModel.from(hostName, subscriptionKey, appId));
    }

    /// <summary>
    /// Dispose of associated resources.
    /// </summary>
    public void close()
    {
        if (disposed)
        {
            return;
        }

        disposed = true;
    }

    private boolean disposed = false;


    IntentModel(Carbon.Internal.LuisModel model)
    {
        modelImpl = model;
    }

    public Carbon.Internal.LuisModel modelImpl;
    Carbon.Internal.LuisModel getmodelImpl()
    {
        return modelImpl;
    }
}

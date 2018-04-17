package com.microsoft.cognitiveservices.speech.recognition;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.ParameterCollection;
import com.microsoft.cognitiveservices.speech.recognition.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.recognition.speech.SpeechRecognizer;

 /// <summary>
 /// Factory methods to create recognizers.
 /// </summary>
 public final class RecognizerFactory implements Closeable //: IDisposable
 {
    /// <summary>
    /// Creates an instance of recognizer factory.
    /// </summary>
    public RecognizerFactory() throws UnsupportedOperationException
    {
        initInternal();
    }

    /// <summary>
    /// Creates an instance of recognizer factory with specified subscription key and region (optional).
    /// </summary>
    /// <param name="subscriptionKey">The subscription key.</param>
    /// <param name="region">The region name.</param>
    public RecognizerFactory(String subscriptionKey, String region) throws UnsupportedOperationException
    {
        initInternal();
        setSubscriptionKey(subscriptionKey);
        if (region != null)
        {
            setRegion(region);
        }
    }

    /// <summary>
    /// Gets/sets the subscription key.
    /// </summary>
    public String getSubscriptionKey()
    {
            return getParameters.getString(ParameterNames.SpeechSubscriptionKey);
        }

    public void setSubscriptionKey(String value)
        {
            // factoryImpl.SetSubscriptionKey(value);
            getParameters.set(ParameterNames.SpeechSubscriptionKey, value);
        }

    /// <summary>
    /// Gets/sets the authorization token.
    /// If this is set, subscription key is ignored.
    /// User needs to make sure the provided authorization token is valid and not expired.
    /// </summary>
    public String getAuthorizationToken()
    {
            return getParameters.getString(ParameterNames.SpeechAuthToken);
        }

    public void setAuthorizationToken(String value)
        {
            // factoryImpl.SetSubscriptionKey(value);
            getParameters.set(ParameterNames.SpeechAuthToken, value);
        }

    /// <summary>
    /// Gets/sets the region name of the service to be connected.
    /// </summary>
    public String getRegion()
    {
            return getParameters.getString(ParameterNames.Region);
        }

    public void setRegion(String value)
        {
            getParameters.set(ParameterNames.Region, value);
        }

    /// <summary>
    /// Gets/sets the service endpoint.
    /// </summary>
    public String getEndpoint()
    {
            return getParameters.getString(ParameterNames.SpeechEndpoint);
        }

    public void setEndpoint(String value)
        {
            // factoryImpl.SetSubscriptionKey(value);
            getParameters.set(ParameterNames.SpeechEndpoint, value);
        }

    /// <summary>
    /// The collection of parameters and their values defined for this <see cref="RecognizerFactory"/>.
    /// </summary>
    public ParameterCollection<RecognizerFactory> getParameters;// { get; private set; }

    /// <summary>
    /// Creates a translation recognizer, using the default microphone input.
    /// </summary>
    /// <returns>A translation recognizer instance.</returns>
    public SpeechRecognizer createSpeechRecognizer() throws UnsupportedOperationException
    {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizer());
    }

    /// <summary>
    /// Creates a translation recognizer, using the specified file as audio input.
    /// </summary>
    /// <param name="audioFile">Specifies the audio input file.</param>
    /// <returns>A translation recognizer instance.</returns>
    public SpeechRecognizer createSpeechRecognizer(String audioFile) throws UnsupportedOperationException
    {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithFileInput(audioFile));
    }

    /// <summary>
    /// Creates a translation recognizer, using the specified input stream as audio input.
    /// </summary>
    /// <param name="audioStream">Specifies the audio input stream.</param>
    /// <returns>A translation recognizer instance.</returns>
    public SpeechRecognizer createSpeechRecognizer(AudioInputStream audioStream) throws UnsupportedOperationException
    {
        throw new UnsupportedOperationException();
    }

    /// <summary>
    /// Creates an intent recognizer, using the specified file as audio input.
    /// </summary>
    /// <returns>An intent recognizer instance.</returns>
    public IntentRecognizer createIntentRecognizer()
    {
        return new IntentRecognizer(factoryImpl.createIntentRecognizer());
    }

    /// <summary>
    /// Creates an intent recognizer, using the specified file as audio input.
    /// </summary>
    /// <param name="audioFile">Specifies the audio input file.</param>
    /// <returns>An intent recognizer instance</returns>
    public IntentRecognizer createIntentRecognizer(String audioFile)
    {
        return new IntentRecognizer(factoryImpl.createIntentRecognizerWithFileInput(audioFile));
    }

    /// <summary>
    /// Creates an intent recognizer, using the specified input stream as audio input.
    /// </summary>
    /// <param name="audioStream">Specifies the audio input stream.</param>
    /// <returns>An intent recognizer instance.</returns>
    public IntentRecognizer createIntentRecognizer(AudioInputStream audioStream) throws UnsupportedOperationException
    {
        throw new UnsupportedOperationException();
    }

    /// <summary>
    /// Creates a translation recognizer, using the default microphone input.
    /// </summary>
    /// <returns>A translation recognizer instance.</returns>
    public TranslationRecognizer createTranslationRecognizer() throws UnsupportedOperationException
    {
        throw new UnsupportedOperationException();
    }

    /// <summary>
    /// Creates a translation recognizer, using the specified file as audio input.
    /// </summary>
    /// <param name="audioFile">Specifies the audio input file.</param>
    /// <returns>A translation recognizer instance.</returns>
    public TranslationRecognizer createTranslationRecognizer(String audioFile) throws UnsupportedOperationException
    {
        throw new UnsupportedOperationException();
    }

    /// <summary>
    /// Creates a translation recognizer, using the specified input stream as audio input.
    /// </summary>
    /// <param name="audioStream">Specifies the audio input stream.</param>
    /// <returns>A translation recognizer instance.</returns>
    public TranslationRecognizer createTranslationRecognizer(AudioInputStream audioStream) throws UnsupportedOperationException
    {
        throw new UnsupportedOperationException();
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

        getParameters.close();
        factoryImpl.delete();
        disposed = true;
    }

    private com.microsoft.cognitiveservices.speech.internal.IRecognizerFactory factoryImpl;
    private boolean disposed = false;

    private void initInternal() throws UnsupportedOperationException
    {
        getParameters = new ParameterCollection<RecognizerFactory>(this);
        factoryImpl = com.microsoft.cognitiveservices.speech.internal.RecognizerFactory.getDefault();
    }
}

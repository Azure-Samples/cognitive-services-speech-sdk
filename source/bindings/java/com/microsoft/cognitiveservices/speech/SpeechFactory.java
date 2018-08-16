package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import java.net.URI;
import java.util.ArrayList;
import java.io.File;
import java.io.IOException;

import com.microsoft.cognitiveservices.speech.ParameterCollection;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;
import com.microsoft.cognitiveservices.speech.util.Contracts;

 /**
   * Factory methods to create recognizers.
   */
 public final class SpeechFactory implements Closeable {

    // load the native library. Hold the class active so the
    // class GC does not reclaim it (and the local variables!)
    static Class<?> speechFactoryClass = null;
    static {

        // First choice is to load the binaries from
        // the resources attached to the jar file. On Android,
        // this is not possible, so falling back to loading it
        // from disk.
        try {
            // This is a loose coupling since the class might not be available
            // when running on Android. Therefore, use reflection to find the
            // helper class and call the load function.
            // Fall back to standard loadLibrary, in case that fails.
            Class<?> ncl = Class.forName("com.microsoft.cognitiveservices.speech.NativeLibraryLoader");
            java.lang.reflect.Method nclm = ncl.getMethod("loadNativeBinding");
            nclm.invoke(null); // static.
        }
        catch(java.lang.Error ex) {
            // In case, we cannot load the helper class, fall back to loading
            // the binding just by binding name.
            // TODO name of library will depend on version
            System.loadLibrary("Microsoft.CognitiveServices.Speech.java.bindings");
        }
        catch(java.lang.Exception ex2) {
            // In case, we cannot load the helper class, fall back to loading
            // the binding just by binding name.
            // TODO name of library will depend on version
            System.loadLibrary("Microsoft.CognitiveServices.Speech.java.bindings");
        }

        // one-time init
        configureNativePlatformBinding(""/*useDefaults*/);

        // prevent classgc from freeing this class
        speechFactoryClass = SpeechFactory.class;
    }

    /**
      * Configures the binding for this target, e.g., passing SSL configuration
      * information.
      *
      * @param bindingConfig The platform specific binding configuration.
      */
    static void configureNativePlatformBinding(String bindingConfig) {
        Contracts.throwIfNull(bindingConfig, "bindingConfig");

        com.microsoft.cognitiveservices.speech.internal.carbon_javaJNI.SetupNativeLibraries(bindingConfig);
    }

    /**
     * Configures the directory in which certificates are stored and placed
     * the default root certificate for speech services in that directory.
     *
     * Takes the default tempdir + "/speech-sdk-certs"
     */
    public static void configureNativePlatformBindingWithDefaultCertificate() {
        String filePathRoot = System.getProperty("java.io.tmpdir");
        Contracts.throwIfNullOrWhitespace(filePathRoot, "java.io.tmpdir");

        // out default is /speech-sdk-certs
        filePathRoot += File.separator + "speech-sdk-certs";

        File directoryRoot = new File(filePathRoot);
        directoryRoot.mkdirs();
        directoryRoot.deleteOnExit();

        Contracts.throwIfDirectoryDoesNotExist(filePathRoot, "path is not a directory");
        SpeechFactory.configureNativePlatformBindingWithDefaultCertificate(directoryRoot.getAbsolutePath());
    }

    /**
      * Configures the directory in which certificates are stored and placed
      * the default root certificate for speech services in that directory.
      *
      * @param path The platform specific path where to store the certificate.
      */
    static void configureNativePlatformBindingWithDefaultCertificate(String path) {
        Contracts.throwIfNullOrWhitespace(path, "path");

        java.io.File cacheFile = new java.io.File(path, defaultCertFilenameHash );

        if (!cacheFile.exists()) {
            try {
                java.io.FileOutputStream outputStream = new java.io.FileOutputStream(cacheFile);
                try {
                    byte[] buf = defaultCertString.getBytes("UTF-8");
                    outputStream.write(buf, 0, buf.length);
                } finally {
                    outputStream.close();
                }
            } catch (java.io.IOException e) {
                throw new IllegalArgumentException("Could not open " + cacheFile.toString(), e);
            }
        }

        String certPath = new java.io.File(path).getAbsolutePath();
        com.microsoft.cognitiveservices.speech.internal.carbon_javaJNI.SetupNativeLibraries(certPath);
    }

    // Baltimore_CyberTrust_Root
    private static String defaultCertString = "-----BEGIN CERTIFICATE-----\n" +
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"+
    "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"+
    "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"+
    "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"+
    "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"+
    "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"+
    "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"+
    "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"+
    "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"+
    "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"+
    "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"+
    "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"+
    "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"+
    "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"+
    "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"+
    "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"+
    "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"+
    "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"+
    "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"+
    "-----END CERTIFICATE-----";

    // Baltimore_CyberTrust_Root
    private static String defaultCertFilenameHash = "653b494a.0";
    
    /**
      * Creates an instance of recognizer factory.
      */
    private SpeechFactory(com.microsoft.cognitiveservices.speech.internal.ICognitiveServicesSpeechFactory factoryImpl) {
        Contracts.throwIfNull(factoryImpl, "factoryImpl");

        this.factoryImpl = factoryImpl;

        // connect the native properties with the swig layer.
        _Parameters = new ParameterCollection<SpeechFactory>(this);
    }

    /**
      * Static instance of SpeechFactory returned by passing subscriptionKey and service region.
      * @param subscriptionKey The subscription key.
      * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
      * @return The speech factory
      */
    public static SpeechFactory fromSubscription(String subscriptionKey, String region) {
        Contracts.throwIfIllegalSubscriptionKey(subscriptionKey, "subscriptionKey");
        Contracts.throwIfNullOrWhitespace(region, "region");

        return new SpeechFactory(com.microsoft.cognitiveservices.speech.internal.SpeechFactory.FromSubscription(subscriptionKey, region));
    }

    /**
      * Static instance of SpeechFactory returned by passing authorization token and service region.
      * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
      * expipres, the caller needs to refresh it by setting the property `AuthorizationToken` with a new valid token.
      * Otherwise, all the recognizers created by this SpeechFactory instance will encounter errors during recognition.
      * @param authorizationToken The authorization token.
      * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
      * @return The speech factory
      */
    public static SpeechFactory fromAuthorizationToken(String authorizationToken, String region) {
        Contracts.throwIfNullOrWhitespace(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrWhitespace(region, "region");

        return new SpeechFactory(com.microsoft.cognitiveservices.speech.internal.SpeechFactory.FromAuthorizationToken(authorizationToken, region));
    }

    /**
      * Creates an instance of the speech factory with specified endpoint and subscription key.
      * This method is intended only for users who use a non-standard service endpoint or paramters.
      * Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
      * For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
      * the language setting in uri takes precedence, and the effective language is "de-DE".
      * Only the parameters that are not specified in the endpoint URL can be set by other APIs.
      * @param endpoint The service endpoint to connect to.
      * @param subscriptionKey The subscription key.
      * @return A speech factory instance.
      */
    public static SpeechFactory fromEndPoint(java.net.URI endpoint, String subscriptionKey) {
        Contracts.throwIfNull(endpoint, "endpoint");
        Contracts.throwIfIllegalSubscriptionKey(subscriptionKey, "subscriptionKey");

        return new SpeechFactory(com.microsoft.cognitiveservices.speech.internal.SpeechFactory.FromEndpoint(endpoint.toString(), subscriptionKey));
    }

    /**
      * Gets the subscription key.
      * @return the subscription key.
      */
    public String getSubscriptionKey() {
        return _Parameters.getString(FactoryParameterNames.SubscriptionKey);
    }

    /**
      * Gets the authorization token.
      * If this is set, subscription key is ignored.
      * User needs to make sure the provided authorization token is valid and not expired.
      * @return Gets the authorization token.
      */
    public String getAuthorizationToken() {
        return _Parameters.getString(FactoryParameterNames.AuthorizationToken);
    }

    /**
      * Sets the authorization token.
      * If this is set, subscription key is ignored.
      * User needs to make sure the provided authorization token is valid and not expired.
      * @param value the authorization token.
      */
    public void setAuthorizationToken(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        _Parameters.set(FactoryParameterNames.AuthorizationToken, value);
    }

    /**
      * Gets the region name of the service to be connected.
      * @return the region name of the service to be connected.
      */
    public String getRegion() {
        return _Parameters.getString(FactoryParameterNames.Region);
    }

    /**
      * The collection of parameters and their values defined for this RecognizerFactory.
      * @return The collection of parameters and their values defined for this RecognizerFactory.
      */
    public ParameterCollection<SpeechFactory> getParameters() {
        return _Parameters;
    }

    private final ParameterCollection<SpeechFactory> _Parameters;

    /**
      * Creates a speech recognizer, using the default microphone input.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer() {
        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(), null);
    }

    /**
      * Creates a speech recognizer, using the default microphone input.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer(String language) {
        Contracts.throwIfIllegalLanguage(language, "language");

        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(language), null);
    }

    /**
      * Creates a speech recognizer, using the default microphone input.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @param format Output format, simple or detailed.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer(String language, OutputFormat format) {
        Contracts.throwIfIllegalLanguage(language, "language");
        Contracts.throwIfNull(format, "format");

        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(language,
            format == OutputFormat.Simple ?
                    com.microsoft.cognitiveservices.speech.internal.OutputFormat.Simple :
                    com.microsoft.cognitiveservices.speech.internal.OutputFormat.Detailed
                    ), null);
    }

    /**
      * Creates a speech recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizerWithFileInput(String audioFile) {
        Contracts.throwIfNullOrWhitespace(audioFile, "audioFile");

        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile), null);
    }

    /**
     * Creates a speech recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A speech recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizerWithFileInput(String audioFile, String language) {
        Contracts.throwIfNullOrWhitespace(audioFile, "audioFile");
        Contracts.throwIfIllegalLanguage(language, "language");

        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile, language), null);
    }
    /**
     * Creates a speech recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @param format. Output format, simple or detailed.
     * @return A speech recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizerWithFileInput(String audioFile, String language, OutputFormat format) {
        Contracts.throwIfNullOrWhitespace(audioFile, "audioFile");
        Contracts.throwIfIllegalLanguage(language, "language");
        Contracts.throwIfNull(format, "format");

        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile, language,
                    format == OutputFormat.Simple ?
                        com.microsoft.cognitiveservices.speech.internal.OutputFormat.Simple :
                        com.microsoft.cognitiveservices.speech.internal.OutputFormat.Detailed
                    ), null);
    }

    /**
      * Creates a speech recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizerWithStream(AudioInputStream audioStream) {
        Contracts.throwIfNull(audioStream, "audioStream");

        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithStreamImpl(audioStream.getAdapter()), audioStream);
    }

    /**
     * Creates a speech recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A speech recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizerWithStream(AudioInputStream audioStream, String language) {
        Contracts.throwIfNull(audioStream, "audioStream");
        Contracts.throwIfIllegalLanguage(language, "language");

       return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithStreamImpl(audioStream.getAdapter(), language), audioStream);
    }

    /**
     * Creates a speech recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @param format. Output format, simple or detailed.
     * @return A speech recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizerWithStream(AudioInputStream audioStream, String language, OutputFormat format) {
        Contracts.throwIfNull(audioStream, "audioStream");
        Contracts.throwIfIllegalLanguage(language, "language");
        Contracts.throwIfNull(format, "format");

       return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithStreamImpl(audioStream.getAdapter(), language,
                   format == OutputFormat.Simple ?
                    com.microsoft.cognitiveservices.speech.internal.OutputFormat.Simple :
                    com.microsoft.cognitiveservices.speech.internal.OutputFormat.Detailed
                    ), audioStream);
    }
    
    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizer() {
        return new IntentRecognizer(factoryImpl.CreateIntentRecognizer(), null);
    }

    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizer(String language) {
        Contracts.throwIfIllegalLanguage(language, "language");

        return new IntentRecognizer(factoryImpl.CreateIntentRecognizer(language), null);
    }

    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @return An intent recognizer instance
      */
    public IntentRecognizer createIntentRecognizerWithFileInput(String audioFile) {
        Contracts.throwIfNullOrWhitespace(audioFile, "audioFile");

        return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithFileInput(audioFile), null);
    }

    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return An intent recognizer instance
      */
    public IntentRecognizer createIntentRecognizerWithFileInput(String audioFile, String language) {
        Contracts.throwIfNullOrWhitespace(audioFile, "audioFile");
        Contracts.throwIfIllegalLanguage(language, "language");

        return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithFileInput(audioFile, language), null);
    }

    /**
      * Creates an intent recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizerWithStream(AudioInputStream audioStream) {
        Contracts.throwIfNull(audioStream, "audioStream");

        return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithStreamImpl(audioStream.getAdapter()), audioStream);
    }

    /**
      * Creates an intent recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizerWithStream(AudioInputStream audioStream, String language) {
        Contracts.throwIfNull(audioStream, "audioStream");
        Contracts.throwIfIllegalLanguage(language, "language");

        return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithStreamImpl(audioStream.getAdapter(), language), audioStream);
    }

    /**
      * Creates a translation recognizer, using the default microphone input.
      * @param sourceLanguage The spoken language that needs to be translated.
      * @param targetLanguages The language of translation.
      * @return A translation recognizer instance.
      */
    public TranslationRecognizer createTranslationRecognizer(String sourceLanguage, ArrayList<String> targetLanguages) {
        Contracts.throwIfIllegalLanguage(sourceLanguage, "sourceLanguage");
        Contracts.throwIfNull(targetLanguages, "targetLanguages");

        com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
        
        for(String element : targetLanguages) {
            Contracts.throwIfIllegalLanguage(element, "targetLanguages");
            v.add(element);
        }

        return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizer(sourceLanguage, v), null);
    }

    /**
     * Creates a translation recognizer, using the default microphone input.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The language of translation.
     * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer createTranslationRecognizer(String sourceLanguage, ArrayList<String> targetLanguages, String voice) {
        Contracts.throwIfIllegalLanguage(sourceLanguage, "sourceLanguage");
        Contracts.throwIfNull(targetLanguages, "targetLanguages");
        Contracts.throwIfNullOrWhitespace(voice, "voice");

        com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
        for(String element : targetLanguages) {
            Contracts.throwIfIllegalLanguage(element, "targetLanguages");
            v.add(element);
        }

        return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizer(sourceLanguage, v, voice), null);
   }
    
   /**
     * Creates a translation recognizer using the specified file as audio input.
     * @param audioFile Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer createTranslationRecognizerWithFileInput(String audioFile, String sourceLanguage, ArrayList<String> targetLanguages) {
        Contracts.throwIfNullOrWhitespace(audioFile, "audioFile");
        Contracts.throwIfIllegalLanguage(sourceLanguage, "sourceLanguage");
        Contracts.throwIfNull(targetLanguages, "targetLanguages");

        com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           Contracts.throwIfIllegalLanguage(element, "targetLanguages");
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithFileInput(audioFile, sourceLanguage, v), null);
   }

    /**
     * Creates a translation recognizer using the specified file as audio input.
     * @param audioFile Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer createTranslationRecognizerWithFileInput(String audioFile, String sourceLanguage, ArrayList<String> targetLanguages, String voice) {
        Contracts.throwIfNullOrWhitespace(audioFile, "audioFile");
        Contracts.throwIfIllegalLanguage(sourceLanguage, "sourceLanguage");
        Contracts.throwIfNull(targetLanguages, "targetLanguages");
        Contracts.throwIfNullOrWhitespace(voice, "voice");

       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           Contracts.throwIfIllegalLanguage(element, "targetLanguages");
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithFileInput(audioFile, sourceLanguage, v, voice), null);
   }

   /**
    * Creates a translation recognizer using the specified input stream as audio input.
    * @param audioStream Specifies the audio input stream.
    * @param sourceLanguage The spoken language that needs to be translated.
    * @param targetLanguages The target languages of translation.
    * @return A translation recognizer instance.
    */
   public TranslationRecognizer createTranslationRecognizerWithStream(AudioInputStream audioStream, String sourceLanguage, ArrayList<String> targetLanguages) {
        Contracts.throwIfNull(audioStream, "audioStream");
        Contracts.throwIfIllegalLanguage(sourceLanguage, "sourceLanguage");
        Contracts.throwIfNull(targetLanguages, "targetLanguages");

       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           Contracts.throwIfIllegalLanguage(element, "targetLanguages");
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithStreamImpl(audioStream.getAdapter(), sourceLanguage, v), audioStream);
   }

   /**
    * Creates a translation recognizer using the specified input stream as audio input.
    * @param audioStream Specifies the audio input stream.
    * @param sourceLanguage The spoken language that needs to be translated.
    * @param targetLanguages The target languages of translation.
    * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
    * @return A translation recognizer instance.
    */
   public TranslationRecognizer createTranslationRecognizerWithStream(AudioInputStream audioStream, String sourceLanguage, ArrayList<String> targetLanguages, String voice) {
        Contracts.throwIfNull(audioStream, "audioStream");
        Contracts.throwIfIllegalLanguage(sourceLanguage, "sourceLanguage");
        Contracts.throwIfNull(targetLanguages, "targetLanguages");
        Contracts.throwIfNullOrWhitespace(voice, "voice");

       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           Contracts.throwIfIllegalLanguage(element, "targetLanguages");
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithStreamImpl(audioStream.getAdapter(), sourceLanguage, v, voice), audioStream);
   }

    /**
      * Dispose of associated resources.
      */
    public void close() {
        if (disposed) {
            return;
        }

        _Parameters.close();
        factoryImpl.delete();
        disposed = true;
    }

    // TODO should only visible to property collection.
    public com.microsoft.cognitiveservices.speech.internal.ICognitiveServicesSpeechFactory getFactoryImpl() {
        return factoryImpl;
    }

    private com.microsoft.cognitiveservices.speech.internal.ICognitiveServicesSpeechFactory factoryImpl;
    private boolean disposed = false;
}

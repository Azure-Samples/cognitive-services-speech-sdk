//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Defines constants used in speech recognition and translation.
 * @class
 */
 export class RecognizerParameterNames {
     /**
      * The name of parameter `SpeechRpsToken`.
      * @property
      */
     public static SpeechRpsToken: string = "SPEECH-RpsToken";

     /**
      * The name of parameter `SpeechModelId`.
      * @property
      */
     public static SpeechModelId: string = "SPEECH-ModelId";

     /**
      * The name of parameter `Language`.
      * @property
      */
     public static SpeechRecognitionLanguage: string = "SPEECH-RecoLanguage";

     /**
      * The name of parameter `SpeechRecognitionMode`.
      * @property
      */
     public static SpeechRecognitionMode: string = "SPEECH-RecoMode";

     /**
      * The name of parameter `OutputFormat`.
      * @property
      */
     public static OutputFormat: string = "SPEECH-OutputFormat";

     /**
      * The name of parameter `TranslationFromLanguage` which describes the source language of audio.
      * @property
      */
     public static TranslationFromLanguage: string = "TRANSLATION-FromLanguage";

     /**
      * The name of parameter `TranslationToLanguage` which describes target languages for translation.
      * @property
      */
     public static TranslationToLanguage: string = "TRANSLATION-ToLanguage";

    /**
     * The name of parameter `TranslationVoice` which describes the name of output voice in translation.
     * @property
     */
    public static TranslationVoice: string = "TRANSLATION-Voice";

    /**
     * The name of parameter `Region`.
     * @property
     */
    public static Region: string = "SPEECH-Region";

    /**
     * The name of parameter `SubscriptionKey`.
     * @property
     */
    public static SubscriptionKey: string = "SPEECH-SubscriptionKey";

    /**
     * The name of parameter `AuthorizationToken`
     * @property
     */
    public static AuthorizationToken: string = "SPEECH-AuthToken";

    /**
     * The name of parameter `SpeechEndpoint`.
     * @property
     */
    public static Endpoint: string = "SPEECH-Endpoint";
 }

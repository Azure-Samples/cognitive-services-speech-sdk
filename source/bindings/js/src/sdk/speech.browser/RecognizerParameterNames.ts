//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Defines constants used in speech recognition and translation.
 */
 export class RecognizerParameterNames {
     /**
      * The name of parameter `SpeechRpsToken`
      */
     public static SpeechRpsToken: string = "SPEECH-RpsToken";

     /**
      * The name of parameter `SpeechModelId`
      */
     public static SpeechModelId: string = "SPEECH-ModelId";

     /**
      * The name of parameter `Language`.
      */
     public static SpeechRecognitionLanguage: string = "SPEECH-RecoLanguage";

     /**
      * The name of parameter `RecognitionMode`.
      */
     public static SpeechRecognitionMode: string = "SPEECH-RecoMode";

     /**
      * The name of parameter `OutputFormat`.
      */
     public static OutputFormat: string = "SPEECH-OutputFormat";

     /**
      * The name of parameter `TRANSLATION-FromLanguage` which describes the source language of audio.
      */
     public static TranslationFromLanguage: string = "TRANSLATION-FromLanguage";

     /**
      * The name of parameter `TRANSLATION-ToLanguage` which describes target languages for translation.
      */
     public static TranslationToLanguage: string = "TRANSLATION-ToLanguage";

     /**
      * The name of parameter `TRANSLATION-Voice` which describes the name of output voice in translation.
      */
     public static TranslationVoice: string = "TRANSLATION-Voice";
 }

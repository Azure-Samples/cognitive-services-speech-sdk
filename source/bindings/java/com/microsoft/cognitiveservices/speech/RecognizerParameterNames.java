package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Defines constants used in speech recognition and translation.
  */
public final class RecognizerParameterNames
{
    /**
      * The name of parameter `SpeechRpsToken`
      */
    public static final  String SpeechRpsToken = "SPEECH-RpsToken";

    /**
      * The name of parameter `SpeechModelId`
      */
    public static final  String SpeechModelId = "SPEECH-ModelId";

    /**
      * The name of parameter `Language`.
      */
    public static final  String SpeechRecognitionLanguage = "SPEECH-RecoLanguage";

    /**
      * The name of parameter `RecognitionMode`.
      */
    public static final  String SpeechRecognitionMode = "SPEECH-RecoMode";
    
    /**
     * The name of parameter `TRANSLATION-FromLanguage` which describes the source language of audio.
     */
   public static final  String TranslationFromLanguage = "TRANSLATION-FromLanguage";

   /**
     * The name of parameter `TRANSLATION-ToLanguage` which describes target languages for translation.
     */
   public static final  String TranslationToLanguage = "TRANSLATION-ToLanguage";
}

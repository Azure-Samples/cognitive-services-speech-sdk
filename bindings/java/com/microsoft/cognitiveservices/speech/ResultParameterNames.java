package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Defines constants used in speech recognition and translation.
  */
public final class ResultParameterNames
{
    /**
     * The resulting language understanding json output.
     */
    public static final String LanguageUnderstandingJson = "RESULT-LanguageUnderstandingJson";
    
    /**
     * The raw json output.
     */
    public static final String Json = "RESULT-Json";
    
    /**
     *  Error details, if the call failed.
     */
    public static final String ErrorDetails = "RESULT-ErrorDetails";
}

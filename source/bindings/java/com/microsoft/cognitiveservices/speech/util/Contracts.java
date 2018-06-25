package com.microsoft.cognitiveservices.speech.util;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


/**
  * Contract helper
  */
public class Contracts {
    /**
      * Checks if the argument is null, if yes, throws a NullPointerException
      * @param obj The object to test for null.
      * @param message The error message in case obj is null.
      */
    public static void throwIfNull(Object obj, String message) {
        if(obj == null) {
            throw new NullPointerException(message);
        }
    }

    /**
      * Checks if the argument is null or whitespace, if yes, throws a IllegalArgumentException
      * or NullPointerException.
      * @param obj The object to test for null.
      * @param message The error message in case obj is null.
      */
    public static void throwIfNullOrWhitespace(String obj, String message) {
        if(obj == null) {
            throw new NullPointerException(message);
        }

        if(obj.length() == 0 || obj.trim().length() == 0) {
            throw new IllegalArgumentException(message);
        }
    }

    /**
      * Checks if the language is a valid identifier.
      * @param language The language identifier to test, e.g. "en-US"
      * @param message The error message in case obj is null.
      */
    public static void throwIfIllegalLanguage(String language, String message) {
        throwIfNullOrWhitespace(language, message);

        // TODO: implement more tests
    }

    /**
      * Checks if the subscription is a valid identifier.
      * @param subscription The subscription key to test, usually in the form of a guid
      * @param message The error message in case obj is null.
      */
    public static void throwIfIllegalSubscriptionKey(String subscription, String message) {
        throwIfNullOrWhitespace(subscription, message);

        // TODO: implement more tests
    }
}

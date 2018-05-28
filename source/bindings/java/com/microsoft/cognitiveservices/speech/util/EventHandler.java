package com.microsoft.cognitiveservices.speech.util;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


/**
  * The EventHandler interface used for all recognition events.
  */
public interface EventHandler<T> {

    /**
      * Calles when the subscribed event occurred.
      * @param sender The origin of the event.
      * @param e The event.
      */
    void onEvent(Object sender, T e);
}

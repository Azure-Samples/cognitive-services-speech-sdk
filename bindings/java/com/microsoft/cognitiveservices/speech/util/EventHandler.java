package com.microsoft.cognitiveservices.speech.util;


public interface EventHandler<T> {
    void onEvent(Object sender, T e);
}

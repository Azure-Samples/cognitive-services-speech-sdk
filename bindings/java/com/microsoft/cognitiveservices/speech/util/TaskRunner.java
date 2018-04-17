package com.microsoft.cognitiveservices.speech.util;

public interface TaskRunner<T>  extends java.lang.Runnable {
    @Override
    void run();
    
    T result();
}

package com.microsoft.cognitiveservices.speech.util;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Defines the Task runner interface a task worker has to implement.
  */
public interface TaskRunner<T>  extends java.lang.Runnable {
    /**
      * Runnable that implements the Task body.
      */
    @Override
    void run();
    
    /**
      * The result of the TaskRunner
      * @return The result of the TaskRunner
      */
    T result();
}

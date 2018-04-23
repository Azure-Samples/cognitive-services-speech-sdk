package com.microsoft.cognitiveservices.speech.util;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Executes a TaskRunner.
  */
public class Task<T extends Object> {
    /**
      * Executes a TaskRunner.
      * @param run The TaskRunner to execute.
      */
    public Task(TaskRunner<T> run)
    {
        _run = run;
        _thread = new java.lang.Thread(run);
        _thread.start();
    }
    
    /**
      * The result of the TaskRunner
      * @return The result of the TaskRunner
      */
    public T get() {
        if(_thread.isAlive())
        {
            try {
                _thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        
        return _run.result();
    }

    private java.lang.Thread _thread;
    private TaskRunner<T> _run;
}

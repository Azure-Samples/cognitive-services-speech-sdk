//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.util;

import java.util.concurrent.atomic.*;

/**
 * CloseGuard helper
 */
public class CloseGuard {
    public CloseGuard(){
        isClosed.set(false);
        inUseCount.set(0);
    }
    
    /**
     * Marks the guarded object as closed. Cannot be undone.
     */
    public void closeObject(){
        isClosed.set(true);
        int closeAttempts = 0;

        while(!inUseCount.compareAndSet(0, -1)) {
            if(inUseCount.get() == -1){
                return;
            }

            if(closeAttempts++ == 100){
                closeAttempts = 0;
                Thread.yield();
            }
        }
    }

    /**
     * Locks the object for non-exclusive use.
     */
    public void enterUseObject(){
        if(isClosed.get()) {
            throw new IllegalStateException("Attempt to use closed object rejected.");
        }

        // Optimizing for the case where the object has only a single user.
        int currentVal = 0;        
        int attempts = 0;

        while(!inUseCount.compareAndSet(currentVal, currentVal + 1)){
            if(attempts++ == 100){
                attempts = 0;
                Thread.yield();
            }

            currentVal = inUseCount.get();
            if(currentVal == -1) {
                throw new IllegalStateException("Attempt to use closed object rejected.");
            }
        }
    }

    /**
     * Exits the lock.
     */
    public void exitUseObject(){
        inUseCount.decrementAndGet();
    }

    private AtomicBoolean isClosed = new AtomicBoolean();
    private AtomicInteger inUseCount = new AtomicInteger();

}
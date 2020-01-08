//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import static org.junit.Assert.*;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.atomic.AtomicBoolean;

import com.microsoft.cognitiveservices.speech.util.CloseGuard;

import org.junit.Test;

public class CloseGuardTests {
    @Test
    public void testEnterExit() throws InterruptedException, ExecutionException {
        CloseGuard cg = new CloseGuard();
        cg.enterUseObject();
        cg.exitUseObject();
    }

    @Test
    public void testEnterExitTwice() throws InterruptedException, ExecutionException {
        CloseGuard cg = new CloseGuard();
        cg.enterUseObject();
        cg.exitUseObject();
        cg.enterUseObject();
        cg.exitUseObject();
    }

    @Test
    public void testMultipleEnterExit() throws InterruptedException, ExecutionException {
        CloseGuard cg = new CloseGuard();
        cg.enterUseObject();
        cg.enterUseObject();
        cg.exitUseObject();
        cg.exitUseObject();
    }

    @Test
    public void testEnterCloseBlocked() throws InterruptedException {
        CloseGuard cg = new CloseGuard();
        cg.closeObject();
        try {
            cg.enterUseObject();
            fail("Should have thrown");
        } catch( IllegalStateException e){
        }
    }

    @Test
    public void testClosingTwiceOK() throws InterruptedException {
        CloseGuard cg = new CloseGuard();
        cg.closeObject();
        cg.closeObject();
        
        try {
            cg.enterUseObject();
            fail("Should have thrown");
        } catch( IllegalStateException e){
        }
    }

    @Test
    public void testCloseWaitsForUseExit() throws InterruptedException {
        CloseGuard cg = new CloseGuard();
        AtomicBoolean closeEntered = new AtomicBoolean();
        closeEntered.set(false);

        cg.enterUseObject();

        Thread t = new Thread(new Runnable() { 
                public void run() { 
                    cg.closeObject();
                    closeEntered.set(true);            
                }
            });
        t.start();

        Thread.sleep(1000);
        assertFalse(closeEntered.get());
        cg.exitUseObject();
        Thread.sleep(100);
        assertTrue(closeEntered.get());
        
        try {
            cg.enterUseObject();
            fail("Should have thrown");
        } catch( IllegalStateException e){
        }
    }

}

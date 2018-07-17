package tests.endtoend;
//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Ignore;

@SuppressWarnings("unused")
public class MicrophoneAudioInputStreamTest {

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        // TODO name of library will depend on version and needs an update once we have semantic dll names.
        System.loadLibrary("Microsoft.CognitiveServices.Speech.java.bindings");
    }

    @Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
    @Test
    public void test() {
        fail("Not yet implemented");
    }

}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Test;
import tests.Settings;
import tests.TestHelper;
import tests.endtoend.SampleSimpleRecognize;

public class SampleSimpleRecognizeTest {
    SampleSimpleRecognize test;

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
    }

    @Test
    public void test() {
        test = new SampleSimpleRecognize();

        test.run();

        assertEquals("What's the weather like?", test.getResult());
        TestHelper.AssertConnectionCountMatching(test.getConnectedEventCount(), test.getDisconnectedEventCount());
    }

}

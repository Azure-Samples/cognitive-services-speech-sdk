package tests;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import static org.junit.Assert.*;

public class TestHelper {

    public static void AssertConnectionCountMatching(int connectedEventCount, int disconnectedEventCount)
    {
        System.out.println("connectedEventCount=" + connectedEventCount + ", disconnectedEventCount=" + disconnectedEventCount);
        assertTrue(connectedEventCount > 0);
        assertTrue(connectedEventCount == disconnectedEventCount + 1 || connectedEventCount == disconnectedEventCount);
    }
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.fail;

import java.util.concurrent.ExecutionException;

import com.microsoft.cognitiveservices.speech.*;

import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.Rule;

import tests.BaseJUnit;
import tests.Settings;
import tests.Retry;

public class GrammarTests extends BaseJUnit {

    @Rule
    public Retry retry = new Retry(Settings.TestRetryCount);

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore
    @Test
    public void testDispose() {
        // TODO: make dispose method public
        fail("dispose not yet public");
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testConjureGrammarFromStorageId() throws InterruptedException, ExecutionException {
        Grammar g = Grammar.fromStorageId("71C8B766-2746-4DC4-8825-8968F5F1B25A");
        assertNotNull(g);
        g.close();
    }

    @Test
    public void testConjureGrammarFromStorageIdNonGuid() throws InterruptedException, ExecutionException {
        Grammar g = Grammar.fromStorageId("foo");
        assertNotNull(g);
        g.close();
    }

}

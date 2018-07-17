//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package tests.runner;

import org.junit.runner.JUnitCore;
import org.junit.runner.Result;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.List;

@SuppressWarnings("rawtypes")
public class Runner {

    public static void main(String... args) {
        JUnitCore junitCoreRunner = new JUnitCore();

        PrintStream outputStream = null;
        String testOutputFilename = System.getProperty("TestOutputFilename");
        if (testOutputFilename != null && testOutputFilename.trim().length() > 0) {
            try {
                System.out.println("Found test output name: " + testOutputFilename);
                
                outputStream = new PrintStream(new FileOutputStream(new File(testOutputFilename)));
            } catch (FileNotFoundException e) {
                // report and ignore. will use System.out
                e.printStackTrace();
            }
        }
        else {
            System.out.println("No filename given in TestOutputFilename system property. Using System.out.");
        }
        
        AntXmlRunListener xmlWriter = new AntXmlRunListener(outputStream != null ? outputStream : System.out);
        junitCoreRunner.addListener(xmlWriter);
        
        Class[] classes = resolveTestClasses(args);
        Result result = junitCoreRunner.run(classes);
        
        if (outputStream != null) {
            outputStream.close();
        }
        
        System.exit(result.wasSuccessful() ? 0 : 1);
    }

    private static Class[] resolveTestClasses(String[] args) {
        List<Class> testClasses = new ArrayList<Class>(args.length);
        for (String arg : args) {
            try {
                testClasses.add(Class.forName(arg));
            } catch (ClassNotFoundException e) {
                System.out.println("Could not find class " + e.getMessage());
            }
        }
        return testClasses.toArray(new Class[testClasses.size()]);
    }
}

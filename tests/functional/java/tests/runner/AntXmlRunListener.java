/* Schmant, the build tool, http://www.schmant.org
 * Copyright (C) 2007, 2008 Karl Gustafsson, Holocene Software, 
 * http://www.holocene.se
 *
 * Schmant is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Schmant is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/* This class contains code copied from Apache Ant's
 * org.apache.tools.ant.taskdefs.optional.junit.XMLJUnitResultFormatter class
 * from Ant verson 1.7.1.
 */
package tests.runner;

import org.junit.Ignore;
import org.junit.runner.Description;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;
import org.junit.runner.notification.RunListener;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Text;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.*;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

public class AntXmlRunListener extends RunListener {
    private Document document;
    private Element rootElement;

    private OutputStream outputStream;

    private Map<Description, Long> startedTests = new HashMap<Description, Long>();
    private Map<Serializable, Serializable> failedTests = new HashMap<Serializable, Serializable>();
    private Map<Description, Element> testElements = new HashMap<Description, Element>();

    private int numSkipped = 0;
    private int numErrors = 0;

    public AntXmlRunListener(OutputStream outputStream) {
        this.outputStream = outputStream;
    }

    /** {@inheritDoc}. */
    @Override
    public void testRunStarted(Description description) {
        String testsuiteName = description.getDisplayName();
        if(testsuiteName == "null") testsuiteName = null;
        
        this.document = getDocumentBuilder().newDocument();
        this.rootElement = this.document.createElement("testsuite");
        this.rootElement.setAttribute("name", testsuiteName == null ? "Java Test Run (" + System.getProperty("os.name") + ")" : testsuiteName);
        this.rootElement.setAttribute("timestamp", DateUtils.format(new Date(), DateUtils.ISO8601_DATETIME_PATTERN));
        this.rootElement.setAttribute("hostname", getHostname());
        this.rootElement.appendChild(this.document.createElement("properties"));
    }

    /** {@inheritDoc}. */
    @Override
    public void testStarted(Description description) {
        this.startedTests.put(description, new Long(System.currentTimeMillis()));
    }

    /** {@inheritDoc}. */
    @Override
    public void testFailure(Failure failure) {
        this.numErrors++;

        appendFailedTest("failure", failure);
    }

    /** {@inheritDoc}. */
    @Override
    public void testAssumptionFailure(Failure failure) {
        this.numErrors++;

        appendFailedTest("failure", failure);
    }

    /** {@inheritDoc}. */
    @Override
    public void testIgnored(Description description) throws Exception {
        this.numSkipped++;

        if (!this.startedTests.containsKey(description)) {
            testStarted(description);
        }

        // let the framework fill out the currentTest data.
        testFinished(description);

        String displayName = description.getDisplayName();
        Element skippedElement = this.document.createElement("skipped");
        Element currentTestElement = (Element) this.testElements.get(description);

        currentTestElement.appendChild(skippedElement);

        String message = null;
        Ignore annotation = description.getAnnotation(Ignore.class);
        if(annotation != null) {
            message = annotation.value();
        }

        if (message != null && message.length() > 0) {
            skippedElement.setAttribute("message", message);
        }
        
        skippedElement.setAttribute("type", description.getDisplayName());
        this.failedTests.put(displayName, description);
    }

    /** {@inheritDoc}. */
    @Override
    public void testFinished(Description description) {
        if (!this.startedTests.containsKey(description)) {
            testStarted(description);
        }

        Element currentTestElement = null;
        if (!this.failedTests.containsKey(description)) {
            currentTestElement = this.document.createElement("testcase");
            
            String testCaseName = getTestCaseName(description.getDisplayName());
            currentTestElement.setAttribute("name", testCaseName == null ? "unknown" : testCaseName);

            currentTestElement.setAttribute("classname", getTestCaseClassName(description.getDisplayName()));
            this.rootElement.appendChild(currentTestElement);

            this.testElements.put(description, currentTestElement);
        } else {
            currentTestElement = (Element) this.testElements.get(description);
        }

        Long timestamp = (Long)this.startedTests.get(description);
        currentTestElement.setAttribute("time", "" + ((System.currentTimeMillis() - timestamp.longValue()) / 1000.0));
    }

    /** {@inheritDoc}. */
    @Override
    public void testRunFinished(Result result) {
        try {
            OutputStream o = this.outputStream == null ? System.out : this.outputStream;
            
            this.rootElement.setAttribute("tests", "" + result.getRunCount());
            this.rootElement.setAttribute("failures", "" + result.getFailureCount());
            this.rootElement.setAttribute("errors", "" + numErrors);
            this.rootElement.setAttribute("skipped", "" + numSkipped);
            this.rootElement.setAttribute("time", "" + (result.getRunTime() / 1000.0));
            
            Writer writer = new BufferedWriter(new OutputStreamWriter(o, "UTF8"));
            writer.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
            
            (new DOMElementWriter()).write(this.rootElement, writer, 0, "  ");
            
            writer.flush();
            o.flush();
        } catch (IOException ex) {
            throw new RuntimeException(ex.toString());
        }
    }


    private String getHostname() {
     String hostname = "localhost";
        try {
            final InetAddress localHost = InetAddress.getLocalHost();
            if (localHost != null) {
                hostname = localHost.getHostName();
            }
        } catch (final UnknownHostException e) {
            // fall back to default 'localhost'
        }
        return hostname;
    }

    private void appendFailedTest(String failureType, Failure faillure) {
        testFinished(faillure.getDescription());
        this.failedTests.put(faillure.getDescription(), faillure);

        Element currentTestElement = (Element)this.testElements.get(faillure.getDescription());
        Element failureElement = this.document.createElement(failureType);
        currentTestElement.appendChild(failureElement);

        String failureMessage = faillure.getMessage();
        if (failureMessage != null && failureMessage.length() > 0) {
            failureElement.setAttribute("message", failureMessage);
        }
        failureElement.setAttribute("type", faillure.getDescription().getDisplayName());

        String stackTrace = getStackTrace(faillure.getException());
        Text stackTraceElement = this.document.createTextNode(stackTrace);
        failureElement.appendChild(stackTraceElement);
    }

    private static String getStackTrace(Throwable throwable) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw, true);
        
        throwable.printStackTrace(pw);
        
        pw.flush();
        pw.close();
        return sw.toString();
    }

    private static String getTestCaseName(String testName) {
        if (testName == null) {
            return "test-name-unknown";
        }

        if (testName.endsWith(")")) {
            int idx = testName.lastIndexOf('(');
            return testName.substring(0, idx);
        } else {
            return testName;
        }
    }

    private static String getTestCaseClassName(String testName) {
        if (testName == null) {
            return "test-class-unknown";
        }

        if (testName.endsWith(")")) {
            int idx = testName.lastIndexOf('(');
            return testName.substring(idx + 1, testName.length() - 1);
        } else {
            return testName;
        }
    }

    private static DocumentBuilder getDocumentBuilder() {
        try {
            return DocumentBuilderFactory.newInstance().newDocumentBuilder();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}

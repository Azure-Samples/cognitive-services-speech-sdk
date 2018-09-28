package tests.runner;

import org.junit.runner.Description;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;
import org.junit.runner.notification.RunListener;
import java.io.*;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Date;

public class AntPrintRunListener extends RunListener {
    private PrintStream outputStream;

    public AntPrintRunListener(PrintStream outputStream) {
        this.outputStream = outputStream;
    }

    /** {@inheritDoc}. */
    @Override
    public void testRunStarted(Description description) {
        String testsuiteName = description.getDisplayName();
        if (testsuiteName == "null") testsuiteName = null;

        StringBuffer sb = new StringBuffer();
        sb.append("testsuite => ");
        sb.append("name " + testsuiteName == null ? "Java Test Run (" + System.getProperty("os.name") + ")" : testsuiteName);
        sb.append(", timestamp " + DateUtils.format(new Date(), DateUtils.ISO8601_DATETIME_PATTERN));
        sb.append(", hostname " + getHostname());
        this.outputStream.println(sb.toString());
        this.outputStream.flush();
    }

    /** {@inheritDoc}. */
    @Override
    public void testStarted(Description description) {
        StringBuffer sb = new StringBuffer();
        sb.append(System.currentTimeMillis() + ": testing " + description.getDisplayName() + " STARTED");
        this.outputStream.println(sb.toString());
        this.outputStream.flush();
    }

    /** {@inheritDoc}. */
    @Override
    public void testFailure(Failure failure) {
        StringBuffer sb = new StringBuffer();
        sb.append(System.currentTimeMillis() + ": testing " + failure.getDescription() + " FAILED");
        this.outputStream.println(sb.toString());
        this.outputStream.flush();
    }

    /** {@inheritDoc}. */
    @Override
    public void testAssumptionFailure(Failure failure) {
        StringBuffer sb = new StringBuffer();
        sb.append(System.currentTimeMillis() + ": testing " + failure.getDescription() + " ASSUMPTION FAILED");
        this.outputStream.println(sb.toString());
        this.outputStream.flush();
    }

    /** {@inheritDoc}. */
    @Override
    public void testIgnored(Description description) throws Exception {
        StringBuffer sb = new StringBuffer();
        sb.append(System.currentTimeMillis() + ": testing " + description.getDisplayName() + " IGNORED");
        this.outputStream.println(sb.toString());
        this.outputStream.flush();
    }

    /** {@inheritDoc}. */
    @Override
    public void testFinished(Description description) {
        StringBuffer sb = new StringBuffer();
        sb.append(System.currentTimeMillis() + ": testing " + description.getDisplayName() + " FINISHED");
        this.outputStream.println(sb.toString());
        this.outputStream.flush();
    }

    /** {@inheritDoc}. */
    @Override
    public void testRunFinished(Result result) {
        StringBuffer sb = new StringBuffer();
        sb.append("test finished: failed " + result.getFailureCount());
        sb.append(", ignored " + result.getIgnoreCount());
        sb.append(", total " + result.getRunCount());
        this.outputStream.println(sb.toString());
        this.outputStream.flush();
    }


    private String getHostname() {
        String hostname = null;

        try {
            final InetAddress localHost = InetAddress.getLocalHost();
            if (localHost != null) {
                hostname = localHost.getHostName();
            }
        } catch (final UnknownHostException e) {
            hostname = "localhost";
        }

        return hostname;
    }
}

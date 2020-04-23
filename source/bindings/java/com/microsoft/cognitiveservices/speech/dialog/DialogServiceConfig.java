//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.ServicePropertyChannel;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;

/**
 * Class that defines base configurations for dialog service connector.
 */
public class DialogServiceConfig extends SpeechConfig implements Closeable {

    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
    }

    /*! \cond PROTECTED */
    /**
     * Creates an instance of dialog service config.
     */
    protected DialogServiceConfig(long handleValue) {
        super(handleValue);
    }
    /*! \endcond */

    /**
     * Sets proxy configuration
     * Note: Proxy functionality is not available on macOS. This function will have no effect on this platform.
     * @param proxyHostName the host name of the proxy server, without the protocol scheme (http://)
     * @param proxyPort the port number of the proxy server.
     * @param proxyUserName the user name of the proxy server. Use empty string if no user name is needed.
     * @param proxyPassword the password of the proxy server. Use empty string if no user password is needed.
     */
    public void setProxy(String proxyHostName, int proxyPort, String proxyUserName, String proxyPassword) {
        super.setProxy(proxyHostName, proxyPort, proxyUserName, proxyPassword);
    }

    /**
     * Sets a named property as value.
     * @param name the name of the property.
     * @param value the value.
     */
    public void setProperty(String name, String value) {
        super.setProperty(name, value);
    }

    /**
     * Sets the property by propertyId.
     * @param id PropertyId of the property.
     * @param value The value.
     */
    public void setProperty(PropertyId id, String value) {
        super.setProperty(id, value);
    }

    /**
     * Gets a named property as value.
     * @param name the name of the property.
     * @return The value.
     */
    public String getProperty(String name) {
        return super.getProperty(name);
    }

    /**
     * Gets the property by propertyId.
     * @param id PropertyId of the property.
     * @return The value.
     */
    public String getProperty(PropertyId id) {
        return super.getProperty(id);
    }

    /**
     *  Sets a property value that will be passed to service using the specified channel.
     *  @param name the property name.
     *  @param value the property value.
     *  @param channel the channel used to pass the specified property to service.
     */
    public void setServiceProperty(String name, String value, ServicePropertyChannel channel) {
        super.setServiceProperty(name, value, channel);
    }

    /**
     * Sets the language
     * @param value the language identifier in BCP-47 format.
     */
    public void setLanguage(String value) {
        setSpeechRecognitionLanguage(value);
    }

    /**
     * Gets the language
     * @return Returns the language.
     */
    public String getLanguage() {
        return getSpeechRecognitionLanguage();
    }

    /**
     * Dispose of associated resources.
     */
    @Override
    public void close() {
        if (disposed) {
            return;
        }
        super.close();
        disposed = true;
    }

    /*! \cond INTERNAL */

    /**
      * Returns the dialog service configuration.
      * @return The implementation of the config.
      */
    public SafeHandle getImpl()
    {
        return super.getImpl();
    }
    
    /*! \endcond */

    private boolean disposed = false;
}

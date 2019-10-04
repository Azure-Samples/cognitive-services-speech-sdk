//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.ServicePropertyChannel;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Class that defines base configurations for dialog service connector.
 */
public abstract class DialogServiceConfig implements Closeable {

    /*! \cond PROTECTED */

    // load the native library. Hold the class active so the
    // class GC does not reclaim it (and the local variables!)
    static Class<?> dialogServiceConfigClass = null;
    static {

        // First choice is to load the binaries from
        // the resources attached to the jar file. On Android,
        // this is not possible, so falling back to loading it
        // from disk.
        try {
            // This is a loose coupling since the class might not be available
            // when running on Android. Therefore, use reflection to find the
            // helper class and call the load function.
            // Fall back to standard loadLibrary, in case that fails.
            Class<?> ncl = Class.forName("com.microsoft.cognitiveservices.speech.NativeLibraryLoader");

            // Note: in case the class exists, we call it, ignoring ANY error it raises.
            //       This is on purpose as ONLY the native loader is responsible for loading
            //       the native binding.
            //       In case the loader does not exist, fall back to depending on the runtime
            //       to locate the library for us. This is e.g. necessary on Android.
            if (ncl != null) {
                try {
                    java.lang.reflect.Method nclm = ncl.getMethod("loadNativeBinding");
                    nclm.invoke(null); // static.
                }
                catch (Exception ex) {
                    // ignored.
                    // in particular, we DON'T want to fallback to the platform
                    // loader here as the native loader has already failed and
                    // we don't want to pick up some random library.
                }
            }
            else {
                // trigger an exception so the handler below calls the
                // default loader.
                throw new NullPointerException("no native loader available");
            }
        }
        catch(java.lang.Error ex) {
            // In case, we cannot load the helper class, fall back to loading
            // the binding just by binding name.
            // TODO name of library will depend on version
            System.loadLibrary("Microsoft.CognitiveServices.Speech.java.bindings");
        }
        catch(java.lang.Exception ex2) {
            // In case, we cannot load the helper class, fall back to loading
            // the binding just by binding name.
            // TODO name of library will depend on version
            System.loadLibrary("Microsoft.CognitiveServices.Speech.java.bindings");
        }

        // setup native tmpdir
        com.microsoft.cognitiveservices.speech.internal.carbon_javaJNI.SetTempDirectory(System.getProperty("java.io.tmpdir"));

        // prevent classgc from freeing this class
        dialogServiceConfigClass = DialogServiceConfig.class;
    }

    /**
     * Creates an instance of dialog service config.
     */
    protected DialogServiceConfig(com.microsoft.cognitiveservices.speech.internal.DialogServiceConfig dialogServiceConfigImpl) {
        Contracts.throwIfNull(dialogServiceConfigImpl, "dialogServiceConfigImpl");

        this.configImpl = dialogServiceConfigImpl;
        this.configImpl.SetProperty("SPEECHSDK-SPEECH-CONFIG-SYSTEM-LANGUAGE", "Java");
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
        Contracts.throwIfNullOrWhitespace(proxyHostName, "proxyHostName");
        Contracts.throwIfNull(proxyUserName, "proxyUserName");
        Contracts.throwIfNull(proxyPassword, "proxyPassword");
        if (proxyPort <= 0) {
            throw new IllegalArgumentException("invalid proxy port");
        }
        configImpl.SetProxy(proxyHostName, proxyPort, proxyUserName, proxyPassword);
    }

    /**
     * Sets a named property as value.
     * @param name the name of the property.
     * @param value the value.
     */
    public void setProperty(String name, String value) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        Contracts.throwIfNullOrWhitespace(value, "value");
        configImpl.SetProperty(name, value);
    }

    /**
     * Sets the property by propertyId.
     * @param id PropertyId of the property.
     * @param value The value.
     */
    public void setProperty(PropertyId id, String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        configImpl.SetProperty(id.getValue(), value);
    }

    /**
     * Gets a named property as value.
     * @param name the name of the property.
     * @return The value.
     */
    public String getProperty(String name) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        return configImpl.GetProperty(name);
    }

    /**
     * Gets the property by propertyId.
     * @param id PropertyId of the property.
     * @return The value.
     */
    public String getProperty(PropertyId id) {
        return configImpl.GetProperty(id.getValue());
    }

    /**
     *  Sets a property value that will be passed to service using the specified channel.
     *  @param name the property name.
     *  @param value the property value.
     *  @param channel the channel used to pass the specified property to service.
     */
    public void setServiceProperty(String name, String value, ServicePropertyChannel channel) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        Contracts.throwIfNullOrWhitespace(value, "value");
        configImpl.SetServiceProperty(name, value, channel.getValue());
    }

    /**
     * Sets the language
     * @param value the language identifier in BCP-47 format.
     */
    public void setLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        configImpl.SetLanguage(value);
    }

    /**
     * Gets the language
     * @return Returns the language.
     */
    public String getLanguage() {
        return configImpl.GetLanguage();
    }

    /**
     * Dispose of associated resources.
     */
    @Override
    public void close() {
        if (disposed) {
            return;
        }
        configImpl.delete();
        disposed = true;
    }

    /**
      * Returns the dialog service configuration.
      * @return The implementation of the config.
      */
    public com.microsoft.cognitiveservices.speech.internal.DialogServiceConfig getConfigImpl()
    {
        return configImpl;
    }

    private com.microsoft.cognitiveservices.speech.internal.DialogServiceConfig configImpl;
    private boolean disposed = false;
}

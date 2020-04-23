//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Defines payload for connection events like Connected/Disconnected.
 * Added in version 1.2.0
 */
public final class ConnectionEventArgs extends SessionEventArgs
{
    /*! \cond INTERNAL */

    /**
     * Constructs an instance of a ConnectionEventArgs object.
     * @param eventArgs internal connection event handle.
     */
    public ConnectionEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);
    }

    ConnectionEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }

    private void storeEventData(boolean disposeNativeResources) {
        if (disposeNativeResources == true) {
            super.close();
        }
    }

    /*! \endcond */
}

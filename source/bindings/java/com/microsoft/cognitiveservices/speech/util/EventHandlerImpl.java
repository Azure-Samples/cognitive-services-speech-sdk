//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.util;

import java.util.ArrayList;

/**
 * Implements the binding between the Speech SDK core event handler and Java event handling.
 */
public class EventHandlerImpl<T> {
    /**
     * Registers a new event handler with the associated event.
     * @param handler The handler.
     */
    public void addEventListener(EventHandler<T> handler)
    {
        eventHandlerClients.add(handler);
    }

    /**
     * Unregisters the event handler from the associated event.
     * @param handler The handler.
     */
    public void removeEventListener(EventHandler<T> handler)
    {
        eventHandlerClients.remove(handler);
    }

    /**
     * Fires an event by calling sequentially all registered handlers.
     * @param sender The origin of the event.
     * @param e The event.
     */
    public void fireEvent(Object sender, T e)
    {
        for(EventHandler<T> a : eventHandlerClients)
        {
            a.onEvent(sender, e);
        }
    }

    private ArrayList<EventHandler<T>> eventHandlerClients = new ArrayList<EventHandler<T>> ();
}

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
     * Creates and initializes a new instance of the EventHandlerImpl.
     */
    public EventHandlerImpl()
    {
        this.notifyConnectedOnceFired = false;
    }

     /**
     * Creates and initializes a new instance of the EventHandlerImpl.
     * @param notifyConnectedOnce The handler to be called when the first client subscribes to the event.
     */
    public EventHandlerImpl(Runnable notifyConnectedOnce)
    {
        this();
        this.notifyConnectedOnce = notifyConnectedOnce;
    }

     /**
     * Updates the handler to be notified when the first client subscribes to the event.
     * @param notifyConnectedOnce The handler to be called when the first client subscribes to the event.
     *  If there is already a client subscribed, the callback is immediately invoked.
     */
    public void updateNotificationOnConnected(Runnable notifyConnectedOnce)
    {
        synchronized(this)
        {
            if (this.notifyConnectedOnceFired)
            {
                notifyConnectedOnce.run();
            }

            this.notifyConnectedOnce = notifyConnectedOnce;
        }
    }

    /**
     * Checks if a client has subscribed to events.
     * @return True if a client has subscribed to events, false otherwise.
     */
    public boolean isUpdateNotificationOnConnectedFired()
    {
        return this.notifyConnectedOnceFired;
    }

    /**
     * Registers a new event handler with the associated event.
     * @param handler The handler.
     */
    public void addEventListener(EventHandler<T> handler)
    {
        synchronized(this)
        {
            if (!this.notifyConnectedOnceFired)
            {
                this.notifyConnectedOnceFired = true;

                if (this.notifyConnectedOnce != null)
                {
                    this.notifyConnectedOnce.run();
                }
            }
        }

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
    private Runnable notifyConnectedOnce;
    private boolean notifyConnectedOnceFired;
}

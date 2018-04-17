package com.microsoft.cognitiveservices.speech.util;

import java.util.ArrayList;

public class EventHandlerImpl<T> {
    public void addEventListener(EventHandler<T> handler)
    {
        eventHandlerClients.add(handler);
    }

    public void removeEventListener(EventHandler<T> handler)
    {
        eventHandlerClients.remove(handler);
    }
    
    public void fireEvent(Object sender, T e)
    {
        for(EventHandler<T> a : eventHandlerClients)
        {
            a.onEvent(sender, e);
        }
    }
    
    private ArrayList<EventHandler<T>> eventHandlerClients = new ArrayList<EventHandler<T>> ();
}

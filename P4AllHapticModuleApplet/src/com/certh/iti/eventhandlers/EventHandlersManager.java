package com.certh.iti.eventhandlers;

/**
 *
 * @author Nick
 */
public class EventHandlersManager {
    
    public boolean hapticDeviceFound;
    public boolean spaceKeyHasBeenPressed;
    
    private static EventHandlersManager instance = null;
    
    protected EventHandlersManager()
    {
        hapticDeviceFound = false;
        spaceKeyHasBeenPressed = false;
    }
    
    public static EventHandlersManager getInstance()
    {
        if(instance == null)
            instance = new EventHandlersManager();
        return instance;
    }
    
    public void init()
    {
        hapticDeviceFound = false;
        spaceKeyHasBeenPressed = false;
    }
            
    
}

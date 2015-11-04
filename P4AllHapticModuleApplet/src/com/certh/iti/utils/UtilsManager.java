package com.certh.iti.utils;

import com.google.gson.Gson;
import java.io.*;
import java.nio.channels.FileChannel;
import java.util.ArrayList;

/**
 *
 * @author Nick
 */
public class UtilsManager {
    
    public Gson gson;
    
    private static UtilsManager instance = null;

    protected UtilsManager() {
        gson = new Gson();
    }

    public static UtilsManager getInstance() {
        if (instance == null) {
            instance = new UtilsManager();
        }
        return instance;
    }

    public void copyStreams(InputStream input, OutputStream output) throws IOException {
        // if both are file streams, use channel IO
        if ((output instanceof FileOutputStream) && (input instanceof FileInputStream)) {
            try {
                FileChannel target = ((FileOutputStream) output).getChannel();
                FileChannel source = ((FileInputStream) input).getChannel();

                source.transferTo(0, Integer.MAX_VALUE, target);

                source.close();
                target.close();

                return;
            } catch (Exception e) { /*
                 * failover to byte stream version
                 */

            }
        }

        byte[] buf = new byte[8192];
        while (true) {
            int length = input.read(buf);
            if (length < 0) {
                break;
            }
            output.write(buf, 0, length);
        }

        try {
            input.close();
        } catch (IOException ignore) {
        }
        try {
            output.close();
        } catch (IOException ignore) {
        }
    }
    
    public String readFile(String pathname) throws IOException 
    {
        String fileContents = "";
        FileReader fileReader = new FileReader(pathname);   
        int i;
        while((i =  fileReader.read())!=-1)
        {
            char ch = (char)i;
            fileContents = fileContents + ch; 
        }
        return fileContents;
    }

    public MyScene readObjectsFromFile(String configFilepath) throws IOException 
    {
        String jsonStr = readFile(configFilepath);
        MyScene myScene = gson.fromJson(jsonStr, MyScene.class);
        
        return myScene;
    }
}

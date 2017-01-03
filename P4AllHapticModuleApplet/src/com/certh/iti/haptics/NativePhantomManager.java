/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.certh.iti.haptics;

import com.certh.iti.utils.UtilsManager;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Nick
 */
public class NativePhantomManager
{
    native public static int Initialize();
    native public static int close();
    native public static float[] Update();
    native public static int LoadModel(String objFilename, double scaleFactor, double stiffness, double friction);
    native public static int moveModel(String objFilename, double newPosX, double newPosY, double newPosZ);
    native public static int removeModel(String objFilename);
    native public static String increaseModelSize(String objFilename);
    native public static String decreaseModelSize(String objFilename);
    native public static int clearModels();
    native public static String moveMapLeft();
    native public static String moveMapRight();
    native public static String moveMapUp();
    native public static String moveMapDown();
    native public static String moveMapForward();
    native public static String moveMapBack();
    native public static String testRotMatrix();
    native public static String getObjNameInCollisionWithPhantom();
    native public static int resetPhantomPosition(double fromZPos, double toZPos);

    native public static int createObj(double[][] vertices, int verticesNum, int[][] verticesLinks, int verticesLinksNum, String objFilename, double roadWidth, int buildingsVerticesClustersNum, int[] sizesOfBuildingsVerticesClusters, int[][] buildingsVerticesClusters);

    
//GIA NA KANW ALLAGES STO DLL, XRHSIMOPOIW TO PARAKATW COMMENTED STATIC!!!!!
//    static
//    {
//        //System.load("C:/Program Files/Novint/Falcon/HDAL/bin/hdl.dll");
//        System.loadLibrary("NativePhantomManager");
//    }
    
 /*   static
    {
        try {
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\bass.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("bass.dll failed to load.\n" + e);
        }

        try {
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\borlndmm.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("borlndmm.dll failed to load.\n" + e);
        }
        
        try {
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\cc3260mt.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("cc3260mt.dll failed to load.\n" + e);
        }
        
        try {
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\DLPORTIO.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("DLPORTIO.dll failed to load.\n" + e);
        }
        
        try {
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\glut32.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("glut32.dll failed to load.\n" + e);
        }
        
        try {    
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\hdFalcon.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("hdFalcon.dll failed to load.\n" + e);
        }
        
        try {    
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\hdPhantom.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("hdPhantom.dll failed to load.\n" + e);
        }
        
        try {    
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\msvcp71.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("msvcp71.dll failed to load.\n" + e);
        }
        
        try {    
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\msvcr71.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("msvcr71.dll failed to load.\n" + e);
        }
        
        try {    
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\ode_double.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("ode_double.dll failed to load.\n" + e);
        }
        
        try {    
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\portaudio_x86.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("portaudio_x86.dll failed to load.\n" + e);
        }
        
        try {    
            System.load("F:\\IPTHL\\Prosperity4all\\Haptics\\P4AllHapticModule\\P4AllHapticModuleApplet\\src\\com\\certh\\iti\\haptics\\NativePhantomManager.dll");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("NativePhantomManager.dll failed to load.\n" + e);
        }
    
    }
*/
    
    static
    {
        //load portaudio_x86.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/portaudio_x86.dll");
            File tempFile = File.createTempFile("portaudio_x86", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("portaudio_x86")) + "portaudio_x86.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("portaudio_x86.dll failed to load.\n" + e);
        }

        



        //load NativePhantomManager.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/NativePhantomManager.dll");
            File tempFile = File.createTempFile("NativePhantomManager", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("NativePhantomManager")) + "NativePhantomManager.dll";



            //delete previous NativePhantomManager.dll
            File previousDownloadedDll = new File(newFilename);
            // Attempt to delete it
            previousDownloadedDll.delete();



            oldFile.renameTo(new File(newFilename));

            System.out.println("fileName1 = " + fileName);
            System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("NativePhantomManager.dll failed to load.\n" + e);
        }













        //TestSnd.wav
/*        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/resources/TestSnd.wav");
            File tempFile = File.createTempFile("TestSnd", ".wav");
            OutputStream out = new FileOutputStream(tempFile);
            IOUtils.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("TestSnd")) + "TestSnd.wav";

            //delete previous NativePhantomManager.dll
            File previousDownloadedDll = new File(newFilename);
            // Attempt to delete it
            previousDownloadedDll.delete();

            oldFile.renameTo(new File(newFilename));





            // File to be moved
            File file = new File(newFilename);
            // Destination directory
            File codeBaseDir = new File(MyAudioManager.getInstance().codeBaseStr);
            // Move file to new directory
            boolean success = file.renameTo(new File(codeBaseDir, file.getName()));
            if (!success)
            {
                // File was not successfully moved
                System.out.println("A problem occured during file move....!!!");
            }



        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("TestSnd.wav loading problem!\n" + e);
        }
*/










        //load bass.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/bass.dll");
            File tempFile = File.createTempFile("bass", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("bass")) + "bass.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("bass.dll failed to load.\n" + e);
        }

        //load borlndmm.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/borlndmm.dll");
            File tempFile = File.createTempFile("borlndmm", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("borlndmm")) + "borlndmm.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("borlndmm.dll failed to load.\n" + e);
        }

        //load cc3260mt.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/cc3260mt.dll");
            File tempFile = File.createTempFile("cc3260mt", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("cc3260mt")) + "cc3260mt.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("cc3260mt.dll failed to load.\n" + e);
        }

        //load DLPORTIO.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/DLPORTIO.dll");
            File tempFile = File.createTempFile("DLPORTIO", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("DLPORTIO")) + "DLPORTIO.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("DLPORTIO.dll failed to load.\n" + e);
        }

        //load glut32.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/glut32.dll");
            File tempFile = File.createTempFile("glut32", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("glut32")) + "glut32.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("glut32.dll failed to load.\n" + e);
        }

        //load msvcp71.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/msvcp71.dll");
            File tempFile = File.createTempFile("msvcp71", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("msvcp71")) + "msvcp71.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("msvcp71.dll failed to load.\n" + e);
        }

        //load msvcr71.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/msvcr71.dll");
            File tempFile = File.createTempFile("msvcr71", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("msvcr71")) + "msvcr71.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("msvcr71.dll failed to load.\n" + e);
        }

        //load ode_double.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/ode_double.dll");
            File tempFile = File.createTempFile("ode_double", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("ode_double")) + "ode_double.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("ode_double.dll failed to load.\n" + e);
        }

        //load hdPhantom.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/hdPhantom.dll");
            File tempFile = File.createTempFile("hdPhantom", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("hdPhantom")) + "hdPhantom.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("hdPhantom.dll failed to load.\n" + e);
        }

        //load hdFalcon.dll
        try
        {
            InputStream in = NativePhantomManager.class.getResourceAsStream("/com/certh/iti/haptics/hdFalcon.dll");
            File tempFile = File.createTempFile("hdFalcon", ".dll");
            OutputStream out = new FileOutputStream(tempFile);
            UtilsManager.getInstance().copyStreams(in, out);
            tempFile.deleteOnExit();
            String fileName = tempFile.getAbsolutePath();
            in.close();
            out.close();

            //Obtain the reference of the existing file
            File oldFile = new File(fileName);
            //Now invoke the renameTo() method on the reference, oldFile in this case
            String newFilename = fileName.substring(0, fileName.indexOf("hdFalcon")) + "hdFalcon.dll";
            oldFile.renameTo(new File(newFilename));

            //System.out.println("fileName1 = " + fileName);
            //System.out.println("newFilename = " + newFilename);

            System.load(newFilename);
        } catch (IOException ex) {
            Logger.getLogger(NativePhantomManager.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UnsatisfiedLinkError e) {
          System.err.println("hdFalcon.dll failed to load.\n" + e);
        }

        

    }
}

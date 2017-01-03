package com.certh.iti.applet;

import com.certh.iti.eventhandlers.EventHandlersManager;
import com.certh.iti.haptics.NativePhantomManager;
import com.certh.iti.utils.MyObject;
import com.certh.iti.utils.MyScene;
import com.certh.iti.utils.UtilsManager;
import java.applet.Applet;
import java.awt.BorderLayout;
import java.awt.GraphicsConfiguration;
import java.io.FileNotFoundException;
import java.net.MalformedURLException;
import java.net.URL;

import javax.media.j3d.Alpha;
import javax.media.j3d.AmbientLight;
import javax.media.j3d.Background;
import javax.media.j3d.BoundingSphere;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.Canvas3D;
import javax.media.j3d.DirectionalLight;
import javax.media.j3d.RotationInterpolator;
import javax.media.j3d.Transform3D;
import javax.media.j3d.TransformGroup;
import javax.vecmath.Color3f;
import javax.vecmath.Point3d;
import javax.vecmath.Vector3f;

import com.sun.j3d.loaders.IncorrectFormatException;
import com.sun.j3d.loaders.ParsingErrorException;
import com.sun.j3d.loaders.Scene;
import com.sun.j3d.loaders.objectfile.ObjectFile;
import com.sun.j3d.utils.applet.MainFrame;
import com.sun.j3d.utils.behaviors.vp.OrbitBehavior;
import com.sun.j3d.utils.geometry.Primitive;
import com.sun.j3d.utils.geometry.Sphere;
import com.sun.j3d.utils.universe.PlatformGeometry;
import com.sun.j3d.utils.universe.SimpleUniverse;
import com.sun.j3d.utils.universe.ViewingPlatform;
import java.awt.MouseInfo;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;
import javax.media.j3d.*;

/**
 *
 * @author Nick
 */
public class P4AllHapticModuleApplet extends java.applet.Applet implements Runnable, KeyListener, ActionListener {

    Thread hapticsThread;
    boolean hapticsThreadRunning;
    private boolean spin = false;
    private boolean noTriangulate = false;
    private boolean noStripify = false;
    private double creaseAngle = 60.0;
    private SimpleUniverse u;
    private BoundingSphere bounds;
    public Sphere my3dCursor;
    public Transform3D my3DCursorTransform;
    public TransformGroup my3DCursorTransformGroup;
    public Vector3f my3DCursorPosition;
    public Scene my3DScene;
    public Transform3D t3d;
    public TransformGroup objTrans;
    public static Vector3f myScenePosition;// = new Vector3f(-0.2f, -0.68f, -0.3f);
    
    MyScene myScene;
    //objects to add
    public ArrayList<URL> objFilenames;
    public ArrayList<ObjectFile> my3DObjects;
    public ArrayList<Shape3D> myShapes3D;
    public ArrayList<Vector3f> my3dObjectsPosition;
    public static ArrayList<String> objNames;// = "sphere.obj";
    public static ArrayList<Double> chai3dObjScaleFactors;// = 0.7; 
    //-objects to add
    
    public double SCALE_FACTOR = 0.62;
    //cube_small.obj        scale: 1.0
    //sphere.obj            scale: 0.92
    //single_airboat        scale: 0.98
    

    /*
     * init: This method is intended for whatever initialization is needed for your applet. 
     * It is called after the param tags inside the applet tag have been processed.
     */
    public void init() {
        
        //initialization
        objFilenames = new ArrayList<URL>();
        my3DObjects = new ArrayList<ObjectFile>();
        myShapes3D = new ArrayList<Shape3D>();
        my3dObjectsPosition = new ArrayList<Vector3f>();
        objNames = new ArrayList<String>();
        chai3dObjScaleFactors = new ArrayList<Double>();
        
        //load objects from config file
        System.out.println("HOME DIR: " + System.getProperty("user.home"));
        try
        {
            myScene = UtilsManager.getInstance().readObjectsFromFile(System.getProperty("jnlp.descriptionOfObjectsToShow"));
            for(int i=0; i<myScene.allSceneObjects.size(); i++)
            {
                MyObject tmpObject = myScene.allSceneObjects.get(i);
                //System.out.println(tmpObject.toString());
                objNames.add(tmpObject.filename);
                chai3dObjScaleFactors.add(tmpObject.scaleFactor);
                
                if(tmpObject.cameraFocus)
                {
                    myScenePosition = new Vector3f(tmpObject.positionX, 
                                                   tmpObject.positionY, 
                                                   tmpObject.positionZ);
                }
            }
            
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
        
        for(int i=0; i<objNames.size(); i++)
        {
            try {
                //URL path = getCodeBase();
                //objFilenames.add(new URL(path.toString() + "sampleModels/" + objNames.get(i)));
                objFilenames.add(new URL("file:/" + objNames.get(i)));
                System.out.println("filename (" + Integer.toString(i) + ") : " + objFilenames.get(i));
            } catch (MalformedURLException e) {
                e.printStackTrace();
                System.err.println(e);
                System.exit(1);
            }
        }

        setLayout(new BorderLayout());
        GraphicsConfiguration config = SimpleUniverse.getPreferredConfiguration();

        Canvas3D c = new Canvas3D(config);
        add("Center", c);

        // Create a simple scene and attach it to the virtual universe
        BranchGroup scene = createSceneGraph();
        u = new SimpleUniverse(c);

        // add mouse behaviors to the ViewingPlatform
        ViewingPlatform viewingPlatform = u.getViewingPlatform();

        PlatformGeometry pg = new PlatformGeometry();

        // Set up the ambient light
        Color3f ambientColor = new Color3f(0.1f, 0.1f, 0.1f);
        AmbientLight ambientLightNode = new AmbientLight(ambientColor);
        ambientLightNode.setInfluencingBounds(bounds);
        pg.addChild(ambientLightNode);

        // Set up the directional lights
        Color3f light1Color = new Color3f(1.0f, 1.0f, 0.9f);
        Vector3f light1Direction = new Vector3f(1.0f, 1.0f, 1.0f);
        Color3f light2Color = new Color3f(1.0f, 1.0f, 1.0f);
        Vector3f light2Direction = new Vector3f(-1.0f, -1.0f, -1.0f);

        DirectionalLight light1 = new DirectionalLight(light1Color,
                light1Direction);
        light1.setInfluencingBounds(bounds);
        pg.addChild(light1);

        DirectionalLight light2 = new DirectionalLight(light2Color,
                light2Direction);
        light2.setInfluencingBounds(bounds);
        pg.addChild(light2);

        viewingPlatform.setPlatformGeometry(pg);

        // This will move the ViewPlatform back a bit so the
        // objects in the scene can be viewed.
        viewingPlatform.setNominalViewingTransform();

        if (!spin) {
            OrbitBehavior orbit = new OrbitBehavior(c,
                    OrbitBehavior.REVERSE_ALL);
            BoundingSphere bounds = new BoundingSphere(new Point3d(0.0, 0.0,
                    0.0), 100.0);
            orbit.setSchedulingBounds(bounds);
            viewingPlatform.setViewPlatformBehavior(orbit);
        }

        u.addBranchGraph(scene);

        EventHandlersManager.getInstance().init();
        hapticsThreadRunning = false;
        initHaptics();

        setFocusable(true);
        addKeyListener(this);
    }

    public BranchGroup createSceneGraph() {
        // Create the root of the branch graph
        BranchGroup objRoot = new BranchGroup();

        // Create a ball to demonstrate 3D cursor
        my3dCursor = new Sphere(myScene.pointerSize, Sphere.GENERATE_NORMALS, createAppearance());
        //objRoot.addChild(my3dCursor);  
        my3DCursorTransform = new Transform3D();
        my3DCursorTransformGroup = new TransformGroup();
        my3DCursorTransformGroup.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
        my3DCursorTransformGroup.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
        my3DCursorPosition = new Vector3f(0, 0, 0);
        my3DCursorTransform.setTranslation(my3DCursorPosition);
        my3DCursorTransformGroup.setTransform(my3DCursorTransform);
        my3DCursorTransformGroup.addChild(my3dCursor);
        objRoot.addChild(my3DCursorTransformGroup);

        // Create a Transformgroup to scale all objects so they
        // appear in the scene.
        TransformGroup objScale = new TransformGroup();
        t3d = new Transform3D();
        t3d.setScale(SCALE_FACTOR);
        objScale.setTransform(t3d);
        objRoot.addChild(objScale);

        
        //load 3D visual objects
        for(int i=0; i<objFilenames.size(); i++)
        {
            // Create the transform group node and initialize it to the
            // identity. Enable the TRANSFORM_WRITE capability so that
            // our behavior code can modify it at runtime. Add it to the
            // root of the subgraph.
            objTrans = new TransformGroup();
            objTrans.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
            objTrans.setCapability(TransformGroup.ALLOW_TRANSFORM_READ);
            objScale.addChild(objTrans);

            int flags = ObjectFile.RESIZE;
            if (!noTriangulate) {
                flags |= ObjectFile.TRIANGULATE;
            }
            if (!noStripify) {
                flags |= ObjectFile.STRIPIFY;
            }

            my3DScene = null;
            
            my3DObjects.add(new ObjectFile(flags,
                (float) (creaseAngle * Math.PI / 180.0)));
            
            try {
                my3DScene = my3DObjects.get(i).load(objFilenames.get(i));
            } catch (FileNotFoundException e) {
                System.err.println(e);
                System.exit(1);
            } catch (ParsingErrorException e) {
                System.err.println(e);
                System.exit(1);
            } catch (IncorrectFormatException e) {
                System.err.println(e);
                System.exit(1);
            }

            objTrans.addChild(my3DScene.getSceneGroup());

            //get .obj position
            Map<String, Shape3D> nameMap = my3DScene.getNamedObjects();

            Set<String> keySet = nameMap.keySet();
            Set entrySet = nameMap.entrySet();

            String firstKey = "";
            for ( String key : nameMap.keySet() ) {
                firstKey = key;
                //System.out.println("key: " + key);
                break;
            }

            Iterator entrySet_it = entrySet.iterator();
            System.out.println("Hashtable entries:");
            int counter = 0;

            /*while (entrySet_it.hasNext()) {
                Object myObject = entrySet_it.next();
                System.out.println(Integer.toString(counter) + ") " + myObject);
                counter++;
            }*/
            myShapes3D.add(nameMap.get(firstKey));

            //SET SCENE POSITION
            t3d.setTranslation(/*myScenePosition*/ new Vector3f(myScene.allSceneObjects.get(i).positionX, myScene.allSceneObjects.get(i).positionY, myScene.allSceneObjects.get(i).positionZ));
            objTrans.setTransform(t3d);

            if (myShapes3D != null) {
                Transform3D my3dObjTransform = new Transform3D();
                my3dObjectsPosition.add(new Vector3f(myScene.allSceneObjects.get(i).positionX, myScene.allSceneObjects.get(i).positionY, myScene.allSceneObjects.get(i).positionZ));
                myShapes3D.get(i).getLocalToVworld(my3dObjTransform);
                my3dObjTransform.get(my3dObjectsPosition.get(i));
                System.out.println("my Obj pos : (" + Float.toString(my3dObjectsPosition.get(i).x)
                        + ", " + Float.toString(my3dObjectsPosition.get(i).y)
                        + ", " + Float.toString(my3dObjectsPosition.get(i).z) + ")");
                
                
                //my3DCursorPosition = new Vector3f(curCursorPos[1], curCursorPos[2], curCursorPos[0]);
                //my3DCursorTransform.setTranslation(my3DCursorPosition);
                //my3DCursorTransformGroup.setTransform(my3DCursorTransform);
                
                my3dObjTransform.setTranslation(new Vector3f(myScene.allSceneObjects.get(i).positionX, myScene.allSceneObjects.get(i).positionY, myScene.allSceneObjects.get(i).positionZ));
                
            }
            else
            {
                System.out.println("myShape3D is NULL! firstKey: " + firstKey);
            }


            bounds = new BoundingSphere(new Point3d(0.0, 0.0, 0.0), 100.0);

            if (spin) {
                Transform3D yAxis = new Transform3D();
                Alpha rotationAlpha = new Alpha(-1, Alpha.INCREASING_ENABLE, 0, 0,
                        4000, 0, 0, 0, 0, 0);

                RotationInterpolator rotator = new RotationInterpolator(
                        rotationAlpha, objTrans, yAxis, 0.0f,
                        (float) Math.PI * 2.0f);
                rotator.setSchedulingBounds(bounds);
                objTrans.addChild(rotator);
            }
        }

        Background bgNode = null;
        //background image
        if(myScene.backgoundImage != null
                && myScene.backgoundImage.length() > 0)
        {
            BufferedImage myBufImg = null;
            try
            {
                myBufImg = ImageIO.read(new File(myScene.backgoundImage));
            }
            catch(Exception e)
            {
                e.printStackTrace();
            }
            bgNode = new Background(new ImageComponent2D(ImageComponent2D.FORMAT_RGB, myBufImg, true, false));
        }
        else //background color
        {
            Color3f bgColor = new Color3f(myScene.backgroundColor.R, myScene.backgroundColor.G, myScene.backgroundColor.B);
            bgNode = new Background(bgColor);
        }
        
        bgNode.setApplicationBounds(bounds);
        objRoot.addChild(bgNode);

        return objRoot;
    }

    Appearance createAppearance() {
        Appearance appear = new Appearance();
        Material material = new Material();
        material.setDiffuseColor(myScene.pointerColor.R, myScene.pointerColor.G, myScene.pointerColor.B);
        material.setShininess(50.0f);
        // make modifications to default material properties
        appear.setMaterial(material);

        //      ColoringAttributes colorAtt = new ColoringAttributes();
        //      colorAtt.setShadeModel(ColoringAttributes.SHADE_FLAT);
        //      appear.setColoringAttributes(colorAtt);

        return appear;
    }

    /**
     * This method is called when the applet becomes visible on the screen.
     * Create a thread and start it.
     */
    @Override
    public void start() {
        hapticsThread = new Thread(this);
        //hapticsThread.setPriority(9);

        hapticsThreadRunning = false;
        hapticsThread.start();
    }

    /**
     * This method is called when the applet is no longer visible. Set the
     * animator variable to null so that the thread will exit before displaying
     * the next frame.
     */
    @Override
    public void stop() {
        //mainThread = null;
        hapticsThread = null;

        if (EventHandlersManager.getInstance().hapticDeviceFound) {
            NativePhantomManager.close();
        }
    }
    
    public void destroy() {
        u.cleanup();

        hapticsThreadRunning = false;
        if (hapticsThread != null) {
            hapticsThread.stop();
            hapticsThread = null;
        }
        System.exit(0);
    }

    /**
     * This method is called by the thread that was created in the start method.
     * It does the main animation.
     */
    public void run() {
        while (Thread.currentThread() == hapticsThread) {
            if (hapticsThreadRunning) {
                if (EventHandlersManager.getInstance().hapticDeviceFound) //a haptic device found
                {
                    float[] curCursorPos = {(float) 0.0, (float) 0.0, (float) 0.0};
                    curCursorPos = NativePhantomManager.Update();
                    //System.out.println("CHAID cursor pos: (" + Float.toString(curCursorPos[0]) + 
                    //        ", " + Float.toString(curCursorPos[1]) + ", " + Float.toString(curCursorPos[2]) + ")");

                    my3DCursorPosition = new Vector3f(curCursorPos[1], curCursorPos[2], curCursorPos[0]);
                    my3DCursorTransform.setTranslation(my3DCursorPosition);
                    my3DCursorTransformGroup.setTransform(my3DCursorTransform);
                }
            } else {
                try {
                    //System.out.println("hapticsThread will sleep for 1 sec");
                    hapticsThread.sleep(1000);
                } catch (InterruptedException ex) {
                    ex.printStackTrace();
                }
            }
        }
    }

    public void initHaptics() {
        try {
            int tmpHapticDevFound = NativePhantomManager.Initialize();

            if (tmpHapticDevFound == 1) {
                System.out.println("SENSABLE PHANTOM WAS FOUND!");
            } else if (tmpHapticDevFound == 2) {
                System.out.println("NOVINT FALCON WAS FOUND!");
            } else if (tmpHapticDevFound == 3) {
                System.out.println("HAPTICDEVICE_FORCE WAS FOUND!");
            } else if (tmpHapticDevFound == 4) {
                System.out.println("HAPTICDEVICE_MPB WAS FOUND!");
            } else if (tmpHapticDevFound == 5) {
                System.out.println("HAPTICDEVICE_CHAI3D WAS FOUND!");
            }

            if (tmpHapticDevFound == -1) {
                EventHandlersManager.getInstance().hapticDeviceFound = false;
                System.out.println("NO HAPTIC DEVICE FOUND!");
            } else {
                EventHandlersManager.getInstance().hapticDeviceFound = true;
                
                //load all models
                for(int i=0; i<objNames.size(); i++)
                {
                    NativePhantomManager.LoadModel(objNames.get(i), chai3dObjScaleFactors.get(i), myScene.allSceneObjects.get(i).stiffness, myScene.allSceneObjects.get(i).friction);
                    /******************/
                    /* Java - CHAI 3D */
                    /*   x      y     */
                    /*   y      z     */
                    /*   z      x     */
                    /******************/
                    System.out.println("paw na valw chai model se (" + Float.toString(my3dObjectsPosition.get(i).x) + ", " + Float.toString(my3dObjectsPosition.get(i).y) + ", " + Float.toString(my3dObjectsPosition.get(i).z) + ")");
                    NativePhantomManager.moveModel(objNames.get(i), (double) my3dObjectsPosition.get(i).z, (double) my3dObjectsPosition.get(i).x, (double) my3dObjectsPosition.get(i).y);
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public void keyTyped(KeyEvent e) {
    }

    @Override
    public void keyPressed(KeyEvent e) {

        int tmpKeyCode = e.getKeyCode();
        char tmpKeyChar = e.getKeyChar();
        //System.out.println("..." + Integer.toString(tmpKeyCode));
        //System.out.println("..." + tmpKeyChar + "\n");

        //if(tmpKeyCode == KeyEvent.VK_EQUALS) //+ (zoom in)

        //if(tmpKeyCode == 37) //left

        if (tmpKeyCode == 74) //J (move left)
        {
            //move scene
            myScenePosition.x = myScenePosition.x - (float)0.05;
            t3d.setTranslation(myScenePosition);
            objTrans.setTransform(t3d);
            for(int i=0; i<objNames.size(); i++)
            {
                //find new position of my .obj
                Transform3D my3dObjTransform = new Transform3D();
                my3dObjectsPosition.set(i, new Vector3f());
                myShapes3D.get(i).getLocalToVworld(my3dObjTransform);
                my3dObjTransform.get(my3dObjectsPosition.get(i));
                //move chai3D model
                NativePhantomManager.moveModel(objNames.get(i), (double) my3dObjectsPosition.get(i).z, (double) my3dObjectsPosition.get(i).x, (double) my3dObjectsPosition.get(i).y);
                System.out.println("paw aristera to " + objNames.get(i));
            }
        } else if (tmpKeyCode == 76) //L (move right)
        {
            //move scene
            myScenePosition.x = myScenePosition.x + (float)0.05;
            t3d.setTranslation(myScenePosition);
            objTrans.setTransform(t3d);
            for(int i=0; i<objNames.size(); i++)
            {
                //find new position of my .obj
                Transform3D my3dObjTransform = new Transform3D();
                my3dObjectsPosition.set(i, new Vector3f());
                myShapes3D.get(i).getLocalToVworld(my3dObjTransform);
                my3dObjTransform.get(my3dObjectsPosition.get(i));
                //move chai3D model
                NativePhantomManager.moveModel(objNames.get(i), (double) my3dObjectsPosition.get(i).z, (double) my3dObjectsPosition.get(i).x, (double) my3dObjectsPosition.get(i).y);
            }
        } else if (tmpKeyCode == 73) //I (move up)
        {
            //move scene
            myScenePosition.y = myScenePosition.y + (float)0.05;
            t3d.setTranslation(myScenePosition);
            objTrans.setTransform(t3d);
            for(int i=0; i<objNames.size(); i++)
            {
                //find new position of my .obj
                Transform3D my3dObjTransform = new Transform3D();
                my3dObjectsPosition.set(i, new Vector3f());
                myShapes3D.get(i).getLocalToVworld(my3dObjTransform);
                my3dObjTransform.get(my3dObjectsPosition.get(i));
                //move chai3D model
                NativePhantomManager.moveModel(objNames.get(i), (double) my3dObjectsPosition.get(i).z, (double) my3dObjectsPosition.get(i).x, (double) my3dObjectsPosition.get(i).y);
            }
        } else if (tmpKeyCode == 75) //K (move down)
        {
            //move scene
            myScenePosition.y = myScenePosition.y - (float)0.05;
            t3d.setTranslation(myScenePosition);
            objTrans.setTransform(t3d);
            for(int i=0; i<objNames.size(); i++)
            {
                //find new position of my .obj
                Transform3D my3dObjTransform = new Transform3D();
                my3dObjectsPosition.set(i, new Vector3f());
                myShapes3D.get(i).getLocalToVworld(my3dObjTransform);
                my3dObjTransform.get(my3dObjectsPosition.get(i));
                //move chai3D model
                NativePhantomManager.moveModel(objNames.get(i), (double) my3dObjectsPosition.get(i).z, (double) my3dObjectsPosition.get(i).x, (double) my3dObjectsPosition.get(i).y);
            }
        } else if (tmpKeyCode == 65) //A (move back)
        {
            //move scene
            myScenePosition.z = myScenePosition.z - (float)0.05;
            t3d.setTranslation(myScenePosition);
            objTrans.setTransform(t3d);
            for(int i=0; i<objNames.size(); i++)
            {
                //find new position of my .obj
                Transform3D my3dObjTransform = new Transform3D();
                my3dObjectsPosition.set(i, new Vector3f());
                myShapes3D.get(i).getLocalToVworld(my3dObjTransform);
                my3dObjTransform.get(my3dObjectsPosition.get(i));
                //move chai3D model
                NativePhantomManager.moveModel(objNames.get(i), (double) my3dObjectsPosition.get(i).z, (double) my3dObjectsPosition.get(i).x, (double) my3dObjectsPosition.get(i).y);
            }
        } else if (tmpKeyCode == 90) //Z (move front)
        {
            //move scene
            myScenePosition.z = myScenePosition.z + (float)0.05;
            t3d.setTranslation(myScenePosition);
            objTrans.setTransform(t3d);
            for(int i=0; i<objNames.size(); i++)
            {
                //find new position of my .obj
                Transform3D my3dObjTransform = new Transform3D();
                my3dObjectsPosition.set(i, new Vector3f());
                myShapes3D.get(i).getLocalToVworld(my3dObjTransform);
                my3dObjTransform.get(my3dObjectsPosition.get(i));
                //move chai3D model
                NativePhantomManager.moveModel(objNames.get(i), (double) my3dObjectsPosition.get(i).z, (double) my3dObjectsPosition.get(i).x, (double) my3dObjectsPosition.get(i).y);
            }
        } else if (tmpKeyCode == 82) //R (test rotation matrix)
        {
            String curRotMatr = NativePhantomManager.testRotMatrix();
            System.out.println(curRotMatr);
        }
        
        
    }

    @Override
    public void keyReleased(KeyEvent e) {
        int tmpKeyCode = e.getKeyCode();
        
        if(tmpKeyCode == KeyEvent.VK_SPACE) //SPACE
        {
            NativePhantomManager.resetPhantomPosition(0.55, 0.59);
            hapticsThreadRunning = true;
        }        
        else if(tmpKeyCode == KeyEvent.VK_D) //D
        {
            //String curScale = NativePhantomManager.decreaseModelSize(objNames);
            //System.out.println("DECREASED SCALE -> " + curScale);
        }
        else if(tmpKeyCode == KeyEvent.VK_F) //F
        {
            //String curScale = NativePhantomManager.increaseModelSize(objNames);
            //System.out.println("INCREASED SCALE -> " + curScale);
        }
        
    }

    @Override
    public void actionPerformed(ActionEvent e) {
    }

    /**
     * Initializes the applet P4AllHapticModuleApplet
     */
//    public void init() {
//        try {
//            java.awt.EventQueue.invokeAndWait(new Runnable() {
//
//                public void run() {
//                    initComponents();
//                }
//            });
//        } catch (Exception ex) {
//            ex.printStackTrace();
//        }
//    }
    /**
     * This method is called from within the init() method to initialize the
     * form. WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        setBackground(new java.awt.Color(255, 255, 204));

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 400, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 300, Short.MAX_VALUE)
        );
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables

    //
    // The following allows ObjLoad to be run as an application
    // as well as an applet
    //
    public static void main(String[] args) {
        new MainFrame(new P4AllHapticModuleApplet(args), 700, 700);
    }
    
    // Running as an applet
    public P4AllHapticModuleApplet() {
    }

    // Caled if running as a program
    public P4AllHapticModuleApplet(String[] args) {
        if (args.length != 0) {
            for (int i = 0; i < args.length; i++) {
                if (args[i].startsWith("-")) {
                    if (args[i].equals("-s")) {
                        spin = true;
                    } else if (args[i].equals("-n")) {
                        noTriangulate = true;
                    } else if (args[i].equals("-t")) {
                        noStripify = true;
                    } else if (args[i].equals("-c")) {
                        if (i < args.length - 1) {
                            creaseAngle = (new Double(args[++i])).doubleValue();
                        } else {
                            usage();
                        }
                    } else {
                        usage();
                    }
                } else {
                    for(int cnt=0; cnt<objFilenames.size(); cnt++)
                    {
                        try {
                            if ((args[i].indexOf("file:") == 0)
                                    || (args[i].indexOf("http") == 0)) {
                                objFilenames.set(cnt, new URL(args[i]));
                            } else if (args[i].charAt(0) != '/') {
                                objFilenames.set(cnt, new URL("file:./" + args[i]));
                            } else {
                                objFilenames.set(cnt, new URL("file:" + args[i]));
                            }
                        } catch (MalformedURLException e) {
                            System.err.println(e);
                            System.exit(1);
                        }
                    }
                }
            }
        }
    }

    private void usage() {
        System.out.println("Usage: java ObjLoad [-s] [-n] [-t] [-c degrees] <.obj file>");
        System.out.println("  -s Spin (no user interaction)");
        System.out.println("  -n No triangulation");
        System.out.println("  -t No stripification");
        System.out.println("  -c Set crease angle for normal generation (default is 60 without");
        System.out.println("     smoothing group info, otherwise 180 within smoothing groups)");
        System.exit(0);
    } // End of usage
}

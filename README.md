P4A - WebHapticModule
=====================

A module that enables adding haptic feedback in web applications. A video presentation can be found here: [https://www.youtube.com/watch?v=pFxUimFy8Mk](https://www.youtube.com/watch?v=pFxUimFy8Mk)

The source code contains three main directories:

1) [NativePhantomManager](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/NativePhantomManager): This includes the source code that generates the [NativePhantomManager.dll](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/P4AllHapticModuleApplet/src/com/certh/iti/haptics/NativePhantomManager.dll)

2) [P4AllHapticModuleApplet](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/P4AllHapticModuleApplet): This includes the source code of the applet that can be used to add a 3D scene with haptic feedback in a web application. The 3D scene to be added can be defined in [config.json ](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/TestApp/config.json)

3) [TestApp](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/TestApp): A complete example showing how the P4A WebHapticModule can be used to add a 3D scene with haptic feedback in a preferred web page/application.

## Usage instructions

In order to add haptic feedback to a web application using the P4A WebHapticModule, you may follow the following steps:

### Step 1: Build "NativePhantomManager.dll"
The first step is optional and includes the manual compiling/building of "NativePhantomManager.dll", which is used by the targeted Java Applet and enables haptic interaction with the 3D objects displayed in the Applet using a variety of haptic devices. The .dll can be compiled/build by opening [NativePhantomManager.sln](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/NativePhantomManager/NativePhantomManager.sln) in Microsoft Visual Studio (the aforementioned solution file (.sln) has been created using Microsoft Visual Studio 2005) This .dll is based on the [Java Native Interface (JNI)](http://docs.oracle.com/javase/7/docs/technotes/guides/jni/) framework, which enables execution of functions of [CHAI 3D](http://www.chai3d.org/) (written in C++) inside a Java application.

You also have the option to skip this step by using directly the already built "NativePhantomManager.dll", which can be found [here](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/P4AllHapticModuleApplet/src/com/certh/iti/haptics/NativePhantomManager.dll).

Manual build of "NativePhantomManager.dll" will be actually needed only in cases where you want to modify/extend the functionalities already provided.

### Step 2: Build "P4AllHapticModuleApplet"
The second step includes the compiling/building of [P4AllHapticModuleApplet](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/P4AllHapticModuleApplet), which is the configurable Java Applet that will be finally added in the preferred page/application. In order to build "P4AllHapticModuleApplet" you may use any Java IDE. However, if you decide to use [NetBeans](https://netbeans.org/), a pre-configured environment is already provided [here](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/P4AllHapticModuleApplet/nbproject). 

In case you decided to build by yourself the "NativePhantomManager.dll", as described in Step 1, you will need to replace [the existing NativePhantomManager.dll](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/P4AllHapticModuleApplet/src/com/certh/iti/haptics/NativePhantomManager.dll) with the newly generated .dll and then "clean & build" the [P4AllHapticModuleApplet](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/P4AllHapticModuleApplet).

### Step 3: Adding a 3D scene with haptic feedback in a preferred web page/application
A complete example showing how the P4A WebHapticModule can be used to add a 3D scene with haptic feedback in a preferred web page/application can be found [here](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/TestApp).

More specifically, you need to:

a) Create a [JSON file](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/TestApp/config.json) describing the 3D scene to be added.

	{
		"backgoundImage" : "./sampleBackground/background.jpg",	
		"backgroundColor" : {
			"R": 0.05,
			"G": 0.05,
			"B": 0.5
		},
		"pointerSize" : 0.03,
		"pointerColor" : {
			"R": 1.0,
			"G": 0.0,
			"B": 0.0
		},
		"allSceneObjects" : [
			{
				"filename" : "./sampleModels/Object1.obj",
				"scaleFactor" : 0.25,
				"positionX" : 1.0,
				"positionY" : -0.3,
				"positionZ" : 0.0,
				"stiffness" : 0.99,
				"friction" : 0.34,
				"cameraFocus" : false
			},
			{
				"filename" : "./sampleModels/Object2.obj",
				"scaleFactor" : 0.7,
				"positionX" : -0.5,
				"positionY" : 0.2,
				"positionZ" : -0.3,
				"stiffness" : 0.99,
				"friction" : 0.74,
				"cameraFocus" : true
			}
		]
	}

In particular, the description of the scene is achieved through the following attributes:
- *backgoundImage*: the filepath of  the image that will be used as background of the 3D scene.
- *backgroundColor*: if a specific color is preferred as background instead of an image, the corresponding RGB values of the color can be defined using this attribute.
- *pointerSize*: this attribute defines the size of the 3D cursor.
- *pointerColor*: the RGB values of the preferred color of the 3D cursor.

and for each 3D object the following attributes have to be defined:
- *filename*: the filepath of the .obj file.
- *scaleFactor*: the scale factor of the haptic model. When adding a new 3D object in the scene, the developer may need to manually adjust this value if inconsistencies between the visual and haptic rendering occur. More specifically, when the haptic model is smaller or greater than the visual 3D model (the cursor penetrates into the object or “touches the air”, correspondingly), the developer can adjust this value to solve the aforementioned problem.  
- *positionX, positionY, positionZ*: the position of the 3D object inside the scene in X, Y and Z axis, correspondingly.
- *stiffness*: the stiffness level of the object in N/m. 
- *friction*: the value of this attribute corresponds to the static and dynamic friction for the 3D object.
- *cameraFocus*: when this attribute is *true*, the camera focuses on this object. 

b) Create a [.jnlp file](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/TestApp/launch.jnlp) describing:
- the [location of the P4AllHapticModuleApplet](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/TestApp/launch.jnlp#L17) to be loaded,
- the [location of the various dependencies](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/TestApp/launch.jnlp#L19-L22), 
- as well as the [location of the JSON file describing the 3D scene](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/TestApp/launch.jnlp#L24) to be loaded.

c) Add an [APPLET tag](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/TestApp/launch.html#L9-L15) in the HTML code of your web page/application where you prefer to add the corresponding 3D scene.

### Troubleshooting
#### Possible problem:
One common problem that you may encounter concerns an exception saying *"JAR resources in JNLP file are not signed by the same certificate"*.

####Solution:
You need to create your own certificate using the [keytool](https://docs.oracle.com/javase/6/docs/technotes/tools/solaris/keytool.html) command interface 
- e.g. keytool -genkey -alias YOUR_KEY -keystore YOUR_KEYSTORE_FILENAME

and then to sign all the .jar files (the [P4AllHapticModuleApplet.jar](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/TestApp/P4AllHapticModuleApplet.jar) and all the [dependencies](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/TestApp/lib)) using the [jarsigner](http://docs.oracle.com/javase/6/docs/technotes/tools/windows/jarsigner.html) tool.
- e.g. jarsigner -keystore YOUR_KEYSTORE_FILENAME -storepass YOUR_STORE_PASS -keypass YOUR_KEY_PASS JAR_FILENAME_TO_SIGN.jar YOUR_KEY

### Funding Acknowledgement

The research leading to these results has received funding from the European Union's Seventh Framework Programme (FP7) under grant agreement No.610510
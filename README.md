P4A - WebHapticModule
=====================

A module that enables adding haptic feedback in web applications. A video presentation can be found here: [https://www.youtube.com/watch?v=pFxUimFy8Mk](https://www.youtube.com/watch?v=pFxUimFy8Mk)

The source code contains three main directories:

1) [NativePhantomManager](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/NativePhantomManager): This includes the source code that generates the [NativePhantomManager.dll](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/P4AllHapticModuleApplet/src/com/certh/iti/haptics/NativePhantomManager.dll)

2) [P4AllHapticModuleApplet](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/P4AllHapticModuleApplet): This includes the source code of the applet that can be used to add a 3D scene with haptic feedback in a web application. The 3D scene to be added can be defined in [config.json ](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/SampleConfig/config.json)

3) [SampleConfig](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/SampleConfig): The includes a sample [config.json ](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/SampleConfig/config.json) file that describes a 3D scene with haptic feedback.

## Usage instructions

In order to add haptic feedback to a web application using the P4A WebHapticModule, you may follow the following steps:

### Step 1: Build "NativePhantomManager.dll"
The first step is optional and includes the manual compiling/building of "NativePhantomManager.dll", which is used by the targeted Java Applet and enables haptic interaction with the 3D objects displayed in the Applet using a variety of haptic devices. This .dll is based on the [Java Native Interface (JNI)](http://docs.oracle.com/javase/7/docs/technotes/guides/jni/) framework, which enables execution of functions of [CHAI 3D](http://www.chai3d.org/) (written in C++) inside a Java application.

You also have the option to skip this step by using directly the already built "NativePhantomManager.dll", which can be found [here](https://github.com/P4ALLcerthiti/WebHapticModule/blob/master/P4AllHapticModuleApplet/src/com/certh/iti/haptics/NativePhantomManager.dll).

Manual build of "NativePhantomManager.dll" will be actually needed only in cases where you want to modify/extend the functionalities already provided.

### Step 2: Build "P4AllHapticModuleApplet"
The second step includes the compiling/building of "P4AllHapticModuleApplet", which is the configurable Java Applet that will be finally added in the targeted web page. In order to build "P4AllHapticModuleApplet" you may use any Java IDE. However, if you decide to use [NetBeans](https://netbeans.org/), a pre-configured environment is already provided [here](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/P4AllHapticModuleApplet/nbproject). 

### Step 3: Adding a 3D scene with haptic feedback in a preferred web page/application
A complete example showing how the P4A WebHapticModule can be used to add a 3D scene with haptic feedback in a preferred web page/application can be found [here](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/TestApp).

More specifically, you need to:
a) Create a JSON file like [this one](https://github.com/P4ALLcerthiti/WebHapticModule/tree/master/TestApp/config.json) describing the 3D scene to be added.
b) 

### Troubleshooting
jar signing...


### Funding Acknowledgement

The research leading to these results has received funding from the European Union's Seventh Framework Programme (FP7) under grant agreement No.610510
#pragma once
//#include "Ogre.h"
//#include "CEGUI.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//CHAI 3D v.2.0.0
#include "extras/CGlobals.h"
#include "GL/glut.h"
//#include "gl.h"
#include "chai3d.h"
//-CHAI 3D


class phantomModel
{
public:
	cMesh* model; //needed
	std::string name;
	//Ogre::Vector3 pos;
	std::string pos;
	bool isPartOfTheScene;

	// rotational velocity of the object
	cVector3d rotVel;
	double scaleFactor;
};

class PhantomManager
{
private:
	PhantomManager();
public:
	boolean curHapticDeviceID;

	static const int HAPTICDEVICE_NOT_DEFINED = -1;
	static const int HAPTICDEVICE_SENSABLE = 1;
	static const int HAPTICDEVICE_NOVINT = 2;
	static const int HAPTICDEVICE_FORCE = 3;
	static const int HAPTICDEVICE_MPB = 4;
	static const int HAPTICDEVICE_CHAI3D = 5;

	float SCALE_STEP;

	double HAPTICDEVICE_FORCE_DURING_INITIALIZATION;

	//Ogre::String mapLocalFilename;
	float mapScalingFactor;

//--CHAI 3D---------------
	// a world that contains all objects of the virtual environment
	cWorld* world; //needed

	cGenericHapticDevice* hapticDevice; //needed

	// simulation clock
	cPrecisionClock simClock; //needed

	// a haptic device handler
	cHapticDeviceHandler* handler; //needed

	// a virtual tool representing the haptic device in the scene
	cGeneric3dofPointer* tool; //needed

	// radius of the tool proxy
	double proxyRadius;

	double stiffnessMax;
//-------------------

	cPrecisionClock disableCollisionTempTimer; 

	bool runWithPhantomSupport;
	
	//Ogre::Vector3 INIT_CHAI_LEFT_UP_BORDER;
	//Ogre::Vector3 INIT_CHAI_RIGHT_UP_BORDER;

	//Ogre::Vector3 CHAI_LEFT_UP_BORDER;
	//Ogre::Vector3 CHAI_RIGHT_UP_BORDER;
	//Ogre::Vector3 CHAI_RIGHT_DOWN_BORDER;
	//Ogre::Vector3 CHAI_LEFT_DOWN_BORDER;

	//Ogre::Vector3 MAX_CHAI_LEFT_UP_BORDER;	//when hapgets have the maximum size
	//Ogre::Vector3 MAX_CHAI_RIGHT_UP_BORDER;

	bool PhantomIsCurrentlySupported;
	bool startHaptics;
	bool updatePhantom;
	bool phantomBtnPushed;

	bool userSelectedTheImageToBeLoaded;

	bool collisionWithAllComponentsEnabled;

	float mapCHAIScaleFactorX;
	float mapCHAIScaleFactorY;
	float mapCHAIScaleFactorZ;

	float testObjectScaleFactorX;	//used for testing
	float testObjectScaleFactorY;
	float testObjectScaleFactorZ;

	//Ogre::Timer resumeCollisionTimer;
	std::vector <phantomModel> phantomModels;
	//Ogre::Vector3 phantomCurCHAIPos;
	std::string curCollisionObjName;
	//Ogre::String curCollisionObjName;
	//Ogre::Timer movingTimer;
	//Ogre::Timer goToPreviousURLTimer;
	bool earconPlayed;

	bool startInitializationOfPhantomPosition_center;

	int testRotMatrixIndex;

	std::string objectNameInContactWithPhantom;

	// status of the main simulation haptics loop
	bool simulationRunning;
	// has exited haptics simulation thread
	bool simulationFinished;
	// display level for collision tree
	int collisionTreeDisplayLevel;

	bool phantomManagerHasBeenInitialized;

	double INITIALIZATION_FROM_Z;
	double INITIALIZATION_TO_Z;

	static PhantomManager& Instance();

	~PhantomManager();
	int Init();
	int close();
	float* updateHaptics(void);	
	int LoadModel(const char* filename, std::string name, cVector3d chaiDimPos, float angleRotationInOgreX, float angleRotationInOgreY, float angleRotationInOgreZ, float scaleFactor, double tmpStiffness, double tmpFriction, bool isPartOfScene=false);
	int moveModel(std::string objFilename, double newPosX, double newPosY, double newPosZ);
    int removeModel(std::string objFilename);
	std::string increaseModelSize(std::string objFilename);
	std::string decreaseModelSize(std::string objFilename);
	std::string getObjNameInCollisionWithPhantom();
	//Ogre::Vector3 getNormalizedModelPos(Ogre::Vector3 absolutePos);
	int disableCollisionWithAllComponents();
	int enableCollisionWithAllComponents();
	int getIndexByName(std::string name);
	int getIndexByCMesh(cMesh* tmpModel);
	//bool chaiModelExists(Ogre::String tmpName);
	int clearModels();
	//void removeModel(Ogre::String name);	
	//void test_move(Ogre::String objName, Ogre::String where);
	std::string ftoa(const double& x);
	std::string moveMapLeft();
	std::string moveMapRight();
	std::string moveMapUp();
	std::string moveMapDown();
	std::string moveMapForward();
	std::string moveMapBack();
	//void debug_logChaiAndOgreCursorPosition();
	std::string testRotMatrix();
	int resetPhantomPosition(double fromZ, double toZ);
	void initPhantomPosition(cVector3d curPhantomPos);
};
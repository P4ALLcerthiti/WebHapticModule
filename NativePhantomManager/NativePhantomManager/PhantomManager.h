#pragma once

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//CHAI 3D v.2.0.0
#include "extras/CGlobals.h"
#include "GL/glut.h"
#include "chai3d.h"
//-CHAI 3D


class phantomModel
{
public:
	cMesh* model; //needed
	std::string name;
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

	std::vector <phantomModel> phantomModels;
	std::string curCollisionObjName;
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
	int disableCollisionWithAllComponents();
	int enableCollisionWithAllComponents();
	int getIndexByName(std::string name);
	int getIndexByCMesh(cMesh* tmpModel);
	int clearModels();
	std::string ftoa(const double& x);
	std::string moveMapLeft();
	std::string moveMapRight();
	std::string moveMapUp();
	std::string moveMapDown();
	std::string moveMapForward();
	std::string moveMapBack();
	std::string testRotMatrix();
	int resetPhantomPosition(double fromZ, double toZ);
	void initPhantomPosition(cVector3d curPhantomPos);
};
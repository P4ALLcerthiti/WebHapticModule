#include "stdafx.h"

#include <sstream>
#include <cassert>

#include <iostream>
#include <fstream>
#include <string>

#include "PhantomManager.h"

PhantomManager& PhantomManager::Instance()
{
	static PhantomManager instance;
	return instance;
}

PhantomManager::PhantomManager()
{
	phantomManagerHasBeenInitialized = false;
	simulationRunning = false;
	simulationFinished = false;
	userSelectedTheImageToBeLoaded = false;

	SCALE_STEP = 0.01;

	hapticDevice = NULL;
}

PhantomManager::~PhantomManager()
{
}

int PhantomManager::Init()
{
	HAPTICDEVICE_FORCE_DURING_INITIALIZATION = 0.0;
	objectNameInContactWithPhantom = "";
	int hapticDevFound = HAPTICDEVICE_NOT_DEFINED; //-1 means no haptic device found...
	testRotMatrixIndex = 0;

	collisionTreeDisplayLevel = 1;

	mapCHAIScaleFactorX = 1.0;
	mapCHAIScaleFactorY = 1.0;
	mapCHAIScaleFactorZ = 1.0;

	testObjectScaleFactorX = 0.007;
	testObjectScaleFactorY = 0.007;
	testObjectScaleFactorZ = 0.007;

	collisionWithAllComponentsEnabled = true;
	startInitializationOfPhantomPosition_center = false;

	// create a new world.
    world = new cWorld();
    // create a haptic device handler
    handler = new cHapticDeviceHandler();
    // get access to the first available haptic device
    handler->getDevice(hapticDevice, 0);
    
	// retrieve information about the current haptic device
	cHapticDeviceInfo info;
	if(hapticDevice!=NULL && handler!=NULL)
    {
		// open connection to haptic device
		hapticDevice->open();

		// initialize haptic device
		hapticDevice->initialize();

        info = hapticDevice->getSpecifications();
		if(info.m_modelName != "not defined")
		{
			runWithPhantomSupport = true;
			PhantomIsCurrentlySupported = true;
			
			if(info.m_manufacturerName == "Sensable Technologies") //Phantom found
			{
				hapticDevFound = HAPTICDEVICE_SENSABLE;
				HAPTICDEVICE_FORCE_DURING_INITIALIZATION = 0.8;
			}
			else if(info.m_manufacturerName == "Novint Technologies") //Falcon found
			{
				hapticDevFound = HAPTICDEVICE_NOVINT;
				HAPTICDEVICE_FORCE_DURING_INITIALIZATION = 2.67;
			}
			else if(info.m_manufacturerName == "Force Dimension") //Delta found
			{
				hapticDevFound = HAPTICDEVICE_FORCE;
				HAPTICDEVICE_FORCE_DURING_INITIALIZATION = 0.0;
			}
			else if(info.m_manufacturerName == "MPB Technologies") //freedom 6S found
			{
				hapticDevFound = HAPTICDEVICE_MPB;
				HAPTICDEVICE_FORCE_DURING_INITIALIZATION = 0.0;
			}
			else if(info.m_manufacturerName == "CHAI 3D") //virtual found
			{
				hapticDevFound = HAPTICDEVICE_CHAI3D;
				HAPTICDEVICE_FORCE_DURING_INITIALIZATION = 0.0;
			}
		}
    }

    // create a 3D tool and add it to the world
    tool = new cGeneric3dofPointer(world);
    world->addChild(tool);

    // connect the haptic device to the tool
    tool->setHapticDevice(hapticDevice);

    // initialize tool by connecting to haptic device
    tool->start();

    // map the physical workspace of the haptic device to a larger virtual workspace.
    tool->setWorkspaceRadius(1.0);

    // define a radius for the tool
    tool->setRadius(0.001);
	if(hapticDevFound == HAPTICDEVICE_SENSABLE)
		tool->setRadius(0.001);
	else if(hapticDevFound == HAPTICDEVICE_NOVINT)
		tool->setRadius(0.003);
	else if(hapticDevFound == HAPTICDEVICE_FORCE)
		tool->setRadius(0.001);
	else if(hapticDevFound == HAPTICDEVICE_MPB)
		tool->setRadius(0.001);
	else if(hapticDevFound == HAPTICDEVICE_CHAI3D)
		tool->setRadius(0.001);
	
    // hide the device sphere. only show proxy.
    tool->m_deviceSphere->setShowEnabled(false);

    // set the physical radius of the proxy. for performance reasons, it is
    // sometimes useful to set this value to zero when dealing with
    // complex objects.
    proxyRadius = 0.001;
    tool->m_proxyPointForceModel->setProxyRadius(proxyRadius);

    // inform the proxy algorithm to only check front sides of triangles
    tool->m_proxyPointForceModel->m_collisionSettings.m_checkBothSidesOfTriangles = true;

    // enable if objects in the scene are going to rotate of translate
    // or possibly collide against the tool. If the environment
    // is entirely static, you can set this parameter to "false"
    tool->m_proxyPointForceModel->m_useDynamicProxy = true;

    // read the scale factor between the physical workspace of the haptic
    // device and the virtual workspace defined for the tool
    double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

    // define a maximum stiffness that can be handled by the current
    // haptic device. The value is scaled to take into account the
    // workspace scale factor
    stiffnessMax = info.m_maxForceStiffness / workspaceScaleFactor;

	// simulation in now running
    simulationRunning = true;

	phantomModels.clear();

	startHaptics = false;
	
	curCollisionObjName = "";
	phantomBtnPushed = false;
	disableCollisionTempTimer.start(true);

	phantomManagerHasBeenInitialized = true;
	simulationRunning = true;
	updatePhantom = true;

	curHapticDeviceID = hapticDevFound;

	return hapticDevFound;
}

int PhantomManager::close()
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while(simulationFinished == false)
	{ 
		cSleepMs(100); 
	}

    // close haptic device
    tool->stop();

	int tmpRes = 1;
	return tmpRes;
}

float* PhantomManager::updateHaptics(void)
{
	float* tmpCurCursorPos = new float[3];
	tmpCurCursorPos[0] = (float)0.0;
	tmpCurCursorPos[1] = (float)0.0;
	tmpCurCursorPos[2] = (float)0.0;

	// reset clock
    simClock.reset();

    // main haptic simulation loop
	if(simulationRunning)
    {
		if(updatePhantom)
		{
			// compute global reference frames for each object
			world->computeGlobalPositions(false);

			// update position and orientation of tool
			tool->updatePose();
			// compute interaction forces
			tool->computeInteractionForces();
			// send forces to device
			tool->applyForces();
			
			// retrieve and update the force that is applied on each object

			// stop the simulation clock
			simClock.stop();

			// read the time increment in seconds
			double timeInterval = simClock.getCurrentTimeSeconds();

			// restart the simulation clock
			simClock.reset();
			simClock.start();

			// temp variable to compute rotational acceleration
			cVector3d rotAcc(0,0,0);

			// check if tool is touching an object
			cGenericObject* objectContact = tool->m_proxyPointForceModel->m_contactPoint0->m_object;
			if (objectContact != NULL)
			{
				// retrieve the root of the object mesh
				cGenericObject* obj = objectContact->getSuperParent();

				cGenericObject* temp_a_colObject = objectContact;				
				if(temp_a_colObject != NULL)
				{
					int tmpColObjIndex = getIndexByCMesh(dynamic_cast<cMesh*>(temp_a_colObject->getParent()));
					if(tmpColObjIndex != -1)
					{
						objectNameInContactWithPhantom = phantomModels.at(tmpColObjIndex).name;
					}
					else
					{
						objectNameInContactWithPhantom = "";
					}
				}
				else
				{
					objectNameInContactWithPhantom = "";
				}

				// get position of cursor in global coordinates
				cVector3d toolPos = tool->m_deviceGlobalPos;

				// get position of object in global coordinates
				cVector3d objectPos = obj->getGlobalPos();

				// compute a vector from the center of mass of the object (point of rotation) to the tool
				cVector3d vObjectCMToTool = cSub(toolPos, objectPos);

				// compute acceleration based on the interaction forces
				// between the tool and the object
				if (vObjectCMToTool.length() > 0.0)
				{
					// get the last force applied to the cursor in global coordinates
					// we negate the result to obtain the opposite force that is applied on the
					// object
					cVector3d toolForce = cNegate(tool->m_lastComputedGlobalForce);

					// compute effective force to take into account the fact the object
					// can only rotate around a its center mass and not translate
					cVector3d effectiveForce = toolForce - cProject(toolForce, vObjectCMToTool);

					// compute the resulting torque
					cVector3d torque = cMul(vObjectCMToTool.length(), cCross( cNormalize(vObjectCMToTool), effectiveForce));

					// update rotational acceleration
					const double OBJECT_INERTIA = 0.4;
					rotAcc = (1.0 / OBJECT_INERTIA) * torque;
				}
			}
			else
				objectNameInContactWithPhantom = "";

			// set a threshold on the rotational velocity term
			const double ROT_VEL_MAX = 10.0;

			// add some damping too
			const double DAMPING_GAIN = 0.1;

			//update 3D cursor
			// get position of cursor in global coordinates
			cVector3d chaiCursorPos = tool->m_deviceGlobalPos;
			tmpCurCursorPos[0] = (float)chaiCursorPos.x;
			tmpCurCursorPos[1] = (float)chaiCursorPos.y;
			tmpCurCursorPos[2] = (float)chaiCursorPos.z;

			if(startInitializationOfPhantomPosition_center == true)
				initPhantomPosition(chaiCursorPos);

			cVector3d chaiCursorOrient;
			double chaiCursorOrientAngle;
			tool->m_deviceGlobalRot.toAngleAxis(chaiCursorOrientAngle, chaiCursorOrient);
		}
    }
	// exit haptics thread
    simulationFinished = true;

	return tmpCurCursorPos;
}

std::string PhantomManager::getObjNameInCollisionWithPhantom()
{
	return objectNameInContactWithPhantom;
}

int PhantomManager::LoadModel(const char* filename, std::string name, cVector3d chaiDimPos, float angleRotationInOgreX, float angleRotationInOgreY, float angleRotationInOgreZ, float scaleFactor, double tmpStiffness, double tmpFriction, bool isPartOfScene)
{
	// create a virtual mesh
    cMesh* object = new cMesh(world);

    // add object to world
    world->addChild(object);

	cVector3d targetPos;

	targetPos.x = chaiDimPos.x;
	targetPos.y = chaiDimPos.y;
	targetPos.z = chaiDimPos.z;

	// set the position of the object at the center of the world
    object->setPos(targetPos);
	
	cMatrix3d rot;
	rot.set(0.0, 0.0, 1.0,
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0);
	object->setRot(rot);
	object->computeGlobalPositions();

    // load an object file
    bool fileload = object->loadFromFile(filename);
    if (!fileload)
    {
        close();
        return (-1);
    }

	object->scale(scaleFactor);
	object->computeGlobalPositions();

    // compute a boundary box
    object->computeBoundaryBox(true);

	

    // compute collision detection algorithm
    object->createAABBCollisionDetector(1.01 * proxyRadius, true, false);

	object->setStiffness(tmpStiffness*stiffnessMax, true);
	object->setFriction(tmpFriction, tmpFriction, true);

	phantomModel tmpPhantomModel;
	tmpPhantomModel.model = object;
	tmpPhantomModel.name = name;
	tmpPhantomModel.isPartOfTheScene = isPartOfScene;
	tmpPhantomModel.scaleFactor = scaleFactor;
	phantomModels.push_back(tmpPhantomModel);

	return 0;
}

int PhantomManager::moveModel(std::string objFilename, double newPosX, double newPosY, double newPosZ)
{
	cVector3d curPos = phantomModels.at(getIndexByName(objFilename)).model->getPos();
	curPos.x = newPosX;
	curPos.y = newPosY;
	curPos.z = newPosZ;
	phantomModels.at(getIndexByName(objFilename)).model->setPos(curPos);
	phantomModels.at(getIndexByName(objFilename)).model->computeGlobalPositions(true);
	
	return 0;
}

int PhantomManager::removeModel(std::string objFilename)
{
	world->deleteChild(phantomModels.at(getIndexByName(objFilename)).model);
			
	phantomModels.erase(phantomModels.begin() + getIndexByName(objFilename));

	//compute global reference frames for each object
	world->computeGlobalPositions(false);

	return 0;
}

std::string PhantomManager::increaseModelSize(std::string objFilename)
{
	std::string objCurScale = "";
	double curScale = phantomModels.at(getIndexByName(objFilename)).scaleFactor;
	double newScale = curScale + SCALE_STEP;
	phantomModels.at(getIndexByName(objFilename)).model->scale(newScale);
	phantomModels.at(getIndexByName(objFilename)).model->computeGlobalPositions();
	phantomModels.at(getIndexByName(objFilename)).model->computeBoundaryBox(true);
	phantomModels.at(getIndexByName(objFilename)).model->createAABBCollisionDetector(1.01 * proxyRadius, true, false);
	phantomModels.at(getIndexByName(objFilename)).scaleFactor = newScale;
	objCurScale = "object(0) scale: " + ftoa(newScale);
	return objCurScale;
}

std::string PhantomManager::decreaseModelSize(std::string objFilename)
{
	std::string objCurScale = "";
	double curScale = phantomModels.at(getIndexByName(objFilename)).scaleFactor;
	double newScale = curScale - SCALE_STEP;
	if(newScale > 0.0)
	{
		phantomModels.at(getIndexByName(objFilename)).model->scale(newScale);
		phantomModels.at(getIndexByName(objFilename)).model->computeGlobalPositions();
		phantomModels.at(getIndexByName(objFilename)).model->computeBoundaryBox(true);
		phantomModels.at(getIndexByName(objFilename)).model->createAABBCollisionDetector(1.01 * proxyRadius, true, false);
		phantomModels.at(getIndexByName(objFilename)).scaleFactor = newScale;
	}
	objCurScale = "object(0) scale: " + ftoa(newScale);
	return objCurScale;
}

int PhantomManager::disableCollisionWithAllComponents()
{
	if(collisionWithAllComponentsEnabled == true)
	{
		for(unsigned int i=0; i<phantomModels.size(); i++)
		{
			if(phantomModels.at(i).model->getHapticEnabled())
				phantomModels.at(i).model->setHapticEnabled(false, true);
		}
		collisionWithAllComponentsEnabled = false;
	}
	return 1;
}

int PhantomManager::enableCollisionWithAllComponents()
{
	if(collisionWithAllComponentsEnabled == false)
	{
		for(unsigned int i=0; i<phantomModels.size(); i++)
		{
			if(phantomModels.at(i).model->getHapticEnabled()==false)
				phantomModels.at(i).model->setHapticEnabled(true, true);
		}
		collisionWithAllComponentsEnabled = true;
	}
	return 1;
}


int PhantomManager::getIndexByName(std::string name)
{
	for(unsigned int i=0; i<phantomModels.size(); i++)
	{
		if(phantomModels.at(i).name == name)
			return i;
	}
	return -1;
}

int PhantomManager::getIndexByCMesh(cMesh* tmpModel)
{
	for(unsigned int i=0; i<phantomModels.size(); i++)
	{
		if(phantomModels.at(i).model == tmpModel)
			return i;
	}
	return -1;
}

int PhantomManager::clearModels()
{
	for(unsigned int i=0; i<phantomModels.size(); i++)
	{
		world->removeChild(phantomModels.at(i).model);
			
		phantomModels.at(i).model->deleteAllChildren();
		phantomModels.at(i).model->deleteCollisionDetector(true);

		phantomModels.at(i).model->removeFromGraph();
		delete phantomModels.at(i).model;
		phantomModels.erase(phantomModels.begin() + i);
		i=-1;	//phantomModels.size() has been changed after "erase" so....restart loop!
	}
	//compute global reference frames for each object
	world->computeGlobalPositions(false);

	return 1;
}

std::string PhantomManager::ftoa(const double& x)
{
  std::ostringstream o;
  if (!(o << x)) return "ERROR";
  return o.str();
}

std::string PhantomManager::moveMapLeft()
{
	std::string mapCurPos = "";
	cVector3d curPos =  phantomModels.at(0).model->getPos();
	curPos.x = curPos.x - 0.001;
	phantomModels.at(0).model->setPos(curPos);
	phantomModels.at(0).model->computeGlobalPositions(true);
	mapCurPos = "object(0) curPos: (" + ftoa(curPos.x) + ", " + ftoa(curPos.y) + ", " + ftoa(curPos.z) + ")";
	return mapCurPos;
}

std::string PhantomManager::moveMapRight()
{
	std::string mapCurPos = "";
	cVector3d curPos =  phantomModels.at(0).model->getPos();
	curPos.x = curPos.x + 0.001;
	phantomModels.at(0).model->setPos(curPos);
	phantomModels.at(0).model->computeGlobalPositions(true);
	mapCurPos = "object(0) curPos: (" + ftoa(curPos.x) + ", " + ftoa(curPos.y) + ", " + ftoa(curPos.z) + ")";
	return mapCurPos;
}

std::string PhantomManager::moveMapUp()
{	
	std::string mapCurPos = "";
	cVector3d curPos =  phantomModels.at(0).model->getPos();
	curPos.y = curPos.y + 0.001;
	phantomModels.at(0).model->setPos(curPos);
	phantomModels.at(0).model->computeGlobalPositions(true);
	mapCurPos = "object(0) curPos: (" + ftoa(curPos.x) + ", " + ftoa(curPos.y) + ", " + ftoa(curPos.z) + ")";
	return mapCurPos;
}

std::string PhantomManager::moveMapDown()
{
	std::string mapCurPos = "";
	cVector3d curPos =  phantomModels.at(0).model->getPos();
	curPos.y = curPos.y - 0.001;
	phantomModels.at(0).model->setPos(curPos);
	phantomModels.at(0).model->computeGlobalPositions(true);
	mapCurPos = "object(0) curPos: (" + ftoa(curPos.x) + ", " + ftoa(curPos.y) + ", " + ftoa(curPos.z) + ")";
	return mapCurPos;
}

std::string PhantomManager::moveMapForward()
{
	std::string mapCurPos = "";
	cVector3d curPos =  phantomModels.at(0).model->getPos();
	curPos.z = curPos.z + 0.001;
	phantomModels.at(0).model->setPos(curPos);
	phantomModels.at(0).model->computeGlobalPositions(true);
	mapCurPos = "object(0) curPos: (" + ftoa(curPos.x) + ", " + ftoa(curPos.y) + ", " + ftoa(curPos.z) + ")";
	return mapCurPos;
}

std::string PhantomManager::moveMapBack()
{
	std::string mapCurPos = "";
	cVector3d curPos =  phantomModels.at(0).model->getPos();
	curPos.z = curPos.z - 0.001;
	phantomModels.at(0).model->setPos(curPos);
	phantomModels.at(0).model->computeGlobalPositions(true);
	mapCurPos = "object(0) curPos: (" + ftoa(curPos.x) + ", " + ftoa(curPos.y) + ", " + ftoa(curPos.z) + ")";
	return mapCurPos;
}

std::string PhantomManager::testRotMatrix()
{
	bool forward = true;
	if(forward)
	{
		testRotMatrixIndex++;
		if(testRotMatrixIndex == 25)
			testRotMatrixIndex = 1;
	}
	else
	{
		testRotMatrixIndex--;
		if(testRotMatrixIndex == 0)
			testRotMatrixIndex = 24;
	}	

	cMatrix3d rot;
    
	if(testRotMatrixIndex == 1)
	{
		rot.set(1.0, 0.0, 0.0,
				0.0, 1.0, 0.0,
				0.0, 0.0, 1.0);
	}
	else if(testRotMatrixIndex == 2)
	{
		rot.set(1.0, 0.0, 0.0,
				0.0, 0.0, -1.0,
				0.0, 1.0, 0.0);
	}
	else if(testRotMatrixIndex == 3)
	{
		rot.set(1.0, 0.0, 0.0,
				0.0, -1.0, 0.0,
				0.0, 0.0, -1.0);
	}
	else if(testRotMatrixIndex == 4)
	{
		rot.set(1.0, 0.0, 0.0,
				0.0, 0.0, 1.0,
				0.0, -1.0, 0.0);
	}
	else if(testRotMatrixIndex == 5)
	{
		rot.set(0.0, -1.0, 0.0,
				1.0, 0.0, 0.0,
				0.0, 0.0, 1.0);
	}
	else if(testRotMatrixIndex == 6)
	{
		rot.set(0.0, 0.0, 1.0,
				1.0, 0.0, 0.0,
				0.0, 1.0, 0.0);
	}
	else if(testRotMatrixIndex == 7)
	{
		rot.set(0.0, 1.0, 0.0,
				1.0, 0.0, 0.0,
				0.0, 0.0, -1.0);
	}
	else if(testRotMatrixIndex == 8)
	{
		rot.set(0.0, 0.0, -1.0,
				1.0, 0.0, 0.0,
				0.0, -1.0, 0.0);
	}
	else if(testRotMatrixIndex == 9)
	{
		rot.set(-1.0, 0.0, 0.0,
				0.0, -1.0, 0.0,
				0.0, 0.0, 1.0);
	}
	else if(testRotMatrixIndex == 10)
	{
		rot.set(-1.0, 0.0, 0.0,
				0.0, 0.0, -1.0,
				0.0, -1.0, 0.0);
	}
	else if(testRotMatrixIndex == 11)
	{
		rot.set(-1.0, 0.0, 0.0,
				0.0, 1.0, 0.0,
				0.0, 0.0, -1.0);
	}
	else if(testRotMatrixIndex == 12)
	{
		rot.set(-1.0, 0.0, 0.0,
				0.0, 0.0, 1.0,
				0.0, 1.0, 0.0);
	}
	else if(testRotMatrixIndex == 13)
	{
		rot.set(0.0, 1.0, 0.0,
				-1.0, 0.0, 0.0,
				0.0, 0.0, 1.0);
	}
	else if(testRotMatrixIndex == 14)
	{
		rot.set(0.0, 0.0, 1.0,
				-1.0, 0.0, 0.0,
				0.0, -1.0, 0.0);
	}
	else if(testRotMatrixIndex == 15)
	{
		rot.set(0.0, -1.0, 0.0,
				-1.0, 0.0, 0.0,
				0.0, 0.0, -1.0);
	}
	else if(testRotMatrixIndex == 16)
	{
		rot.set(0.0, 0.0, -1.0,
				-1.0, 0.0, 0.0,
				0.0, 1.0, 0.0);
	}
	else if(testRotMatrixIndex == 17)
	{
		rot.set(0.0, 0.0, -1.0,
				0.0, 1.0, 0.0,
				1.0, 0.0, 0.0);
	}
	else if(testRotMatrixIndex == 18)
	{
		rot.set(0.0, 1.0, 0.0,
				0.0, 0.0, 1.0,
				1.0, 0.0, 0.0);
	}
	else if(testRotMatrixIndex == 19)
	{
		rot.set(0.0, 0.0, 1.0,
				0.0, -1.0, 0.0,
				1.0, 0.0, 0.0);
	}
	else if(testRotMatrixIndex == 20)
	{
		rot.set(0.0, -1.0, 0.0,
				0.0, 0.0, -1.0,
				1.0, 0.0, 0.0);
	}
	else if(testRotMatrixIndex == 21)
	{
		rot.set(0.0, 0.0, -1.0,
				0.0, -1.0, 0.0,
				-1.0, 0.0, 0.0);
	}
	else if(testRotMatrixIndex == 22)
	{
		rot.set(0.0, -1.0, 0.0,
				0.0, 0.0, 1.0,
				-1.0, 0.0, 0.0);
	}
	else if(testRotMatrixIndex == 23)
	{
		rot.set(0.0, 0.0, 1.0,
				0.0, 1.0, 0.0,
				-1.0, 0.0, 0.0);
	}
	else if(testRotMatrixIndex == 24)
	{
		rot.set(0.0, 1.0, 0.0,
				0.0, 0.0, -1.0,
				-1.0, 0.0, 0.0);
	}

	for(unsigned int i=0; i<phantomModels.size(); i++)
	{
		phantomModels.at(i).model->setRot(rot);
		phantomModels.at(i).model->computeGlobalPositions();
	}
	
	std::string rotMatStr;
	std::stringstream out;
	out << testRotMatrixIndex;
	rotMatStr = out.str();

	return "testRotMatrix: dokimazw ton rotation matrix No." + rotMatStr;
}

void PhantomManager::initPhantomPosition(cVector3d curPhantomPos)
{
	cVector3d tmpForce;
	hapticDevice->getForce(tmpForce);

	//center
	if(startInitializationOfPhantomPosition_center)
	{
		if(curPhantomPos.x<INITIALIZATION_FROM_Z)
			tmpForce.x = tmpForce.x + HAPTICDEVICE_FORCE_DURING_INITIALIZATION;
		else if(curPhantomPos.x>INITIALIZATION_TO_Z)
			tmpForce.x = tmpForce.x - HAPTICDEVICE_FORCE_DURING_INITIALIZATION;
		if(curPhantomPos.y<-0.03)
			tmpForce.y = tmpForce.y + HAPTICDEVICE_FORCE_DURING_INITIALIZATION;
		else if(curPhantomPos.y>0.01)
			tmpForce.y = tmpForce.y - HAPTICDEVICE_FORCE_DURING_INITIALIZATION;
		if(curPhantomPos.z<0.02)
			tmpForce.z = tmpForce.z + HAPTICDEVICE_FORCE_DURING_INITIALIZATION;
		else if(curPhantomPos.z>0.06)
			tmpForce.z = tmpForce.z - HAPTICDEVICE_FORCE_DURING_INITIALIZATION;

		if(curPhantomPos.x>INITIALIZATION_FROM_Z && curPhantomPos.x<INITIALIZATION_TO_Z
			&& curPhantomPos.y>-0.03 && curPhantomPos.y<0.01
			&& curPhantomPos.z>0.02 && curPhantomPos.z<0.06)
		{
			startInitializationOfPhantomPosition_center = false;
			enableCollisionWithAllComponents();
		}
		else
			hapticDevice->setForce(tmpForce);
	}
}

int PhantomManager::resetPhantomPosition(double fromZ, double toZ)
{
	INITIALIZATION_FROM_Z = fromZ;
	INITIALIZATION_TO_Z = toZ;
	disableCollisionWithAllComponents();
	startInitializationOfPhantomPosition_center = true;
	return 1;
}


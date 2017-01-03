#include "stdafx.h"

#include <sstream>
#include <cassert>

#include <iostream>
#include <fstream>
#include <string>

#include "PhantomManager.h"
//#include "nxManager.h"
//#include "HapticComponentsManager.h"
//#include "RoadManager.h"

//#include "Magick++.h"


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

//eSpeak initialization
//	eSpeak_init();
//-//eSpeak initialization
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
	/************* see PHANTOM POSITION.txt ***********************/
	/*            TA CHAI MODELS KINOUNTAI STON EKSHS XWRO:       */
    /* (-0.213, -0.662, 0.703)		       (-0.213, 0.662, 0.703) */
    /*                                                            */
    /* (-0.213, -0.662, 0.037)		       (-0.213, 0.662, 0.037) */
	/**************************************************************/
	//disableCollisionTempTimer.reset();

	collisionTreeDisplayLevel = 1;

	//INIT_CHAI_LEFT_UP_BORDER = Ogre::Vector3(-0.213, -0.662, 0.703);
	//INIT_CHAI_RIGHT_UP_BORDER = Ogre::Vector3(-0.213, 0.662, 0.703);

	//MAX_CHAI_LEFT_UP_BORDER = Ogre::Vector3(-0.213, -0.662, 0.703);	//prepei na to vrw...TO DO!
	//MAX_CHAI_RIGHT_UP_BORDER = Ogre::Vector3(-0.213, -0.662, 0.703);

	//CHAI_LEFT_UP_BORDER = INIT_CHAI_LEFT_UP_BORDER;
	//CHAI_RIGHT_UP_BORDER = INIT_CHAI_RIGHT_UP_BORDER;
	//CHAI_RIGHT_DOWN_BORDER = Ogre::Vector3(-0.213, 0.662, 0.037);
	//CHAI_LEFT_DOWN_BORDER = Ogre::Vector3(-0.213, -0.662, 0.037);

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
//NIKOS 14-01-2011
		// open connection to haptic device
		hapticDevice->open();

		// initialize haptic device
		hapticDevice->initialize();
//-NIKOS 14-01-2011	

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
	//Ogre::LogManager::getSingleton().logMessage("\n\n\n\n---PhantomManager::Init() -> " + info.m_manufacturerName + " haptic device found!!!");

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

    // create a thread which starts the main haptics rendering loop
    //cThread* hapticsThread = new cThread();
    //hapticsThread->set(updateHaptics, CHAI_THREAD_PRIORITY_HAPTICS);

	phantomModels.clear();
	//Ogre::LogManager::getSingleton().logMessage("PHANTOM MANAGER INIT - phantomModels.clear()");

	startHaptics = false;
	
	//startPhantomReset = false;
	//phantomResetTimer.reset();
	//phantomResetDuration = 2000;
	//phantomCurCHAIPos = Ogre::Vector3(-1.0, -1.0, -1.0);
	curCollisionObjName = "";
	phantomBtnPushed = false;
	//resumeCollisionTimer.reset();
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
	//Ogre::LogManager::getSingleton().logMessage("simulationRunning:" + Ogre::StringConverter::toString(simulationRunning)
	//	+ ", updatePhantom:" + Ogre::StringConverter::toString(updatePhantom)
	//	+ ", disableCollisionTempTimer.getMilliseconds():" + Ogre::StringConverter::toString(disableCollisionTempTimer.getMilliseconds()));

	// reset clock
    simClock.reset();

    // main haptic simulation loop
	if(simulationRunning)
    {
		if(updatePhantom)// && disableCollisionTempTimer.getCurrentTimeSeconds()>1.5)
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
					//	//Ogre::LogManager::getSingleton().logMessage("CONTACT WITH " + phantomModels.at(tmpColObjIndex).name);	
					//	doOnCollision(phantomModels.at(tmpColObjIndex).name);
						objectNameInContactWithPhantom = phantomModels.at(tmpColObjIndex).name;
					}
					else
					{
						//	doOnCollision("");
						objectNameInContactWithPhantom = "";
					}
				}
				else
				{
					//doOnCollision("");
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

			// update rotational velocity
//			phantomModels.at(getIndexByName("testHyperlink")).rotVel.add(timeInterval * rotAcc);

			// set a threshold on the rotational velocity term
			const double ROT_VEL_MAX = 10.0;
//			double velMag = phantomModels.at(getIndexByName("testHyperlink")).rotVel.length();
//			if (velMag > ROT_VEL_MAX)
//			{
//				phantomModels.at(getIndexByName("testHyperlink")).rotVel.mul(ROT_VEL_MAX / velMag);
//			}

			// add some damping too
			const double DAMPING_GAIN = 0.1;
//			phantomModels.at(getIndexByName("testHyperlink")).rotVel.mul(1.0 - DAMPING_GAIN * timeInterval);

			// if user switch is pressed, set velocity to zero
//			if (tool->getUserSwitch(0) == 1)
//			{
//				phantomModels.at(getIndexByName("testHyperlink")).rotVel.zero();
//			}

			// compute the next rotation configuration of the object
//			if (phantomModels.at(getIndexByName("testHyperlink")).rotVel.length() > CHAI_SMALL)
//			{
//				phantomModels.at(getIndexByName("testHyperlink")).model->rotate(cNormalize(phantomModels.at(getIndexByName("testHyperlink")).rotVel), timeInterval * phantomModels.at(getIndexByName("testHyperlink")).rotVel.length());
//			}

//NIKOS - update 3D cursor
			// get position of cursor in global coordinates
			cVector3d chaiCursorPos = tool->m_deviceGlobalPos;
			tmpCurCursorPos[0] = (float)chaiCursorPos.x;
			tmpCurCursorPos[1] = (float)chaiCursorPos.y;
			tmpCurCursorPos[2] = (float)chaiCursorPos.z;

			if(startInitializationOfPhantomPosition_center == true)
				initPhantomPosition(chaiCursorPos);

			//Ogre::LogManager::getSingleton().logMessage("Cursor CHAI pos: (" + 
			//	Ogre::StringConverter::toString((float)chaiCursorPos.x) + ", " +
			//	Ogre::StringConverter::toString((float)chaiCursorPos.y) + ", " + 
			//	Ogre::StringConverter::toString((float)chaiCursorPos.z) + ")");
			
			//ogreCursorPos = cVector3d(0,0,0);
			//ogreCursorPos.x = (chaiCursorPos.y+0.2) * 700;
			//ogreCursorPos.y = (chaiCursorPos.z+1.2) * 900;
			//ogreCursorPos.z = (chaiCursorPos.x-0.8) * 800;
			
			cVector3d chaiCursorOrient;
			double chaiCursorOrientAngle;
			tool->m_deviceGlobalRot.toAngleAxis(chaiCursorOrientAngle, chaiCursorOrient);
			/*float ogreCursorOrientAngle = chaiCursorOrientAngle;
			Ogre::Vector3 ogreCursorOrient = Ogre::Vector3(0,0,0);
			ogreCursorOrient.x = chaiCursorOrient.y;
			ogreCursorOrient.y = chaiCursorOrient.z;
			ogreCursorOrient.z = chaiCursorOrient.x;*/

			//if phantom buttom is not clicked -> show BLUE curor
			//else -> show ORANGE cursor
			
			//if(nxManager::Instance().phantomCursorNxBody)
			//{
			//	//update phantomCursor position
			//	nxManager::Instance().phantomCursorNxBody->setGlobalPosition(ogreCursorPos);
			//	//Ogre::LogManager::getSingleton().logMessage("cursor pos: " + Ogre::StringConverter::toString(ogreCursorPos));
			//	//update phantomCursor orientation
			//	nxManager::Instance().phantomCursorNxBody->setGlobalOrientation(Ogre::Quaternion(Ogre::Radian(ogreCursorOrientAngle), ogreCursorOrient));
			//}

			//if(nxManager::Instance().phantomCursorClickedNxBody)
			//{
			//	//update phantomCursorClickedNxBody position
			//	nxManager::Instance().phantomCursorClickedNxBody->setGlobalPosition(ogreCursorPos);
			//	//update phantomCursorClickedNxBody orientation
			//	nxManager::Instance().phantomCursorClickedNxBody->setGlobalOrientation(Ogre::Quaternion(Ogre::Radian(ogreCursorOrientAngle), ogreCursorOrient));
			//}

			//if phantom buttom is not clicked -> show BLUE curor
			//else -> show ORANGE cursor
			/*if(tool->getUserSwitch(0) != 1)
			{
				if(nxManager::Instance().phantomCursorNxBody)
					nxManager::Instance().phantomCursorNxBody->getNode()->setVisible(true);
				if(nxManager::Instance().phantomCursorClickedNxBody)
					nxManager::Instance().phantomCursorClickedNxBody->getNode()->setVisible(false);
				phantomBtnPushed = false;
			}							
			else if(tool->getUserSwitch(0) == 1)
			{
				if(nxManager::Instance().phantomCursorNxBody)
					nxManager::Instance().phantomCursorNxBody->getNode()->setVisible(false);
				if(nxManager::Instance().phantomCursorClickedNxBody)
					nxManager::Instance().phantomCursorClickedNxBody->getNode()->setVisible(true);
				phantomBtnPushed = true;
			}*/
//-NIKOS
		}

		/*if(userSelectedTheImageToBeLoaded)
		{
			if(nxManager::Instance().mapHasBeenLoaded==false)
			{
				nxManager::Instance().showNextLoadingScreen();
				if(nxManager::Instance().isMapGenerationCompleted()==true && nxManager::Instance().waitingTimer.getMilliseconds()>3000)
					nxManager::Instance().loadMap();
			}
			else
			{
				if(nxManager::Instance().mapNetworkStructureGraphHasBeenCreated==false)
				{
					nxManager::Instance().showNextLoadingScreen();
					if(HapticComponentsManager::Instance().imageExists("Road Network Structure_currentMap_graph.bmp"))
					{
						Magick::Image tmpImage("Road Network Structure_currentMap_graph.bmp");
						RoadManager::Instance().mapNetworkStructureGraph = tmpImage;

						RoadManager::Instance().mapNetworkStructureGraph.colorSpace(MagickLib::RGBColorspace);

						nxManager::Instance().mapNetworkStructureGraphHasBeenCreated = true;
					}
				}
				else
					RoadManager::Instance().roadNamesTTSUpdate(ogreCursorPos);
			}
		}*/
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
	//object->scaleObject(cVector3d(scaleFactor, scaleFactor, scaleFactor));
	object->computeGlobalPositions();

    // compute a boundary box
    object->computeBoundaryBox(true);

	

    // compute collision detection algorithm
    object->createAABBCollisionDetector(1.01 * proxyRadius, true, false);
	//object->createSphereTreeCollisionDetector(1.01 * proxyRadius, true, false);
	//object->createBruteForceCollisionDetector(true, true);

    // define a default stiffness and friction for the object
/*    if(curHapticDeviceID == HAPTICDEVICE_NOT_DEFINED)
	{
		object->setStiffness(0.8*stiffnessMax, true);
		object->setFriction(0.3, 0.3, true);
	}
	else if(curHapticDeviceID == HAPTICDEVICE_SENSABLE)
	{
		object->setStiffness(0.99*stiffnessMax, true); //arxika to eixa 0.99
		object->setFriction(0.34, 0.34, true);
	}
	else if(curHapticDeviceID == HAPTICDEVICE_NOVINT)
	{
		object->setStiffness(0.99*stiffnessMax, true);
		object->setFriction(0.34, 0.34, true);
	}
	else if(curHapticDeviceID == HAPTICDEVICE_FORCE)
	{
		object->setStiffness(0.8*stiffnessMax, true);
		object->setFriction(0.3, 0.3, true);
	}
	else if(curHapticDeviceID == HAPTICDEVICE_MPB)
	{
		object->setStiffness(0.8*stiffnessMax, true);
		object->setFriction(0.3, 0.3, true);
	}
	else if(curHapticDeviceID == HAPTICDEVICE_CHAI3D)
	{
		object->setStiffness(0.8*stiffnessMax, true);
		object->setFriction(0.3, 0.3, true);
	}
*/

	object->setStiffness(tmpStiffness*stiffnessMax, true);
	object->setFriction(tmpFriction, tmpFriction, true);

	phantomModel tmpPhantomModel;
	tmpPhantomModel.model = object;
	tmpPhantomModel.name = name;
	//tmpPhantomModel.pos.x = targetPos.x;
	//tmpPhantomModel.pos.y = targetPos.y;
	//tmpPhantomModel.pos.z = targetPos.z;
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
			
	//phantomModels.at(getIndexByName(objFilename)).model->deleteAllChildren();
	//phantomModels.at(getIndexByName(objFilename)).model->deleteCollisionDetector(true);

	//phantomModels.at(getIndexByName(objFilename)).model->removeFromGraph();
	//delete phantomModels.at(getIndexByName(objFilename)).model;
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
	//if(newScale < 1.0)
	//{
		phantomModels.at(getIndexByName(objFilename)).model->scale(newScale);
		phantomModels.at(getIndexByName(objFilename)).model->computeGlobalPositions();
		phantomModels.at(getIndexByName(objFilename)).model->computeBoundaryBox(true);
		phantomModels.at(getIndexByName(objFilename)).model->createAABBCollisionDetector(1.01 * proxyRadius, true, false);
		phantomModels.at(getIndexByName(objFilename)).scaleFactor = newScale;
	//}
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

//Ogre::Vector3 PhantomManager::getNormalizedModelPos(Ogre::Vector3 absolutePos)
//{
//	if(PhantomIsCurrentlySupported)
//	{
//		Ogre::Vector3 percentagePos;
//		percentagePos.x = (absolutePos.x-nxManager::Instance().SCENE_BACK_UP_LEFT_BORDER.x)/(nxManager::Instance().SCENE_BACK_UP_RIGHT_BORDER.x - nxManager::Instance().SCENE_BACK_UP_LEFT_BORDER.x);
//		percentagePos.y = (absolutePos.y-nxManager::Instance().SCENE_BACK_UP_LEFT_BORDER.y)/(nxManager::Instance().SCENE_BACK_DOWN_LEFT_BORDER.y - nxManager::Instance().SCENE_BACK_UP_LEFT_BORDER.y);
//		//camera pos.z = 1795
//		percentagePos.z = (absolutePos.z-nxManager::Instance().SCENE_BACK_UP_LEFT_BORDER.z)/(1795 - nxManager::Instance().SCENE_BACK_UP_LEFT_BORDER.z);
//
//		//Ogre::LogManager::getSingleton().logMessage("absolutePos:" + Ogre::StringConverter::toString(absolutePos) + " --- percentageX:" + Ogre::StringConverter::toString(percentagePos.x) + ", percentageY:" + Ogre::StringConverter::toString(percentagePos.y) + ", percentageZ:" + Ogre::StringConverter::toString(percentagePos.z));
//		/************* see PHANTOM POSITION.txt ***********************/
//		/*            TA CHAI MODELS KINOUNTAI STON EKSHS XWRO:       */
//		/* (-0.213, -0.662, 0.703)		       (-0.213, 0.662, 0.703) */
//		/*                                                            */
//		/* (-0.213, -0.662, 0.037)		       (-0.213, 0.662, 0.037) */
//		/**************************************************************/
//
//		/******************/
//		/* Ogre - CHAI 3D */
//		/*   x      y     */
//		/*   y      z     */
//		/*   z      x     */
//		/******************/
//
//		Ogre::Vector3 result;
//		result.x = CHAI_LEFT_UP_BORDER.x;
//		result.y = percentagePos.x*(CHAI_RIGHT_UP_BORDER.y-CHAI_LEFT_UP_BORDER.y) + CHAI_LEFT_UP_BORDER.y;
//		result.z = percentagePos.y*(CHAI_LEFT_DOWN_BORDER.z-CHAI_LEFT_UP_BORDER.z) + CHAI_LEFT_UP_BORDER.z;
//
//		//Ogre::LogManager::getSingleton().logMessage("getNormalizedModelPos returns : " + Ogre::StringConverter::toString(result));
//
//		return result;
//	}
//	return Ogre::Vector3(0,0,0);
//}

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

//bool PhantomManager::chaiModelExists(Ogre::String tmpName)
//{
//	for(unsigned int i=0; i<phantomModels.size(); i++)
//	{
//		if(phantomModels.at(i).name == tmpName)
//			return true;
//	}
//	return false;
//}

int PhantomManager::clearModels()
{
	//Ogre::LogManager::getSingleton().logMessage("PhantomManager::clearModels()");

	for(unsigned int i=0; i<phantomModels.size(); i++)
	{
		//if(phantomModels.at(i).isPartOfTheScene == false)
		//{
			//Ogre::LogManager::getSingleton().logMessage("...clearModels -> model:" + phantomModels.at(i).name + " deleted!");
			world->removeChild(phantomModels.at(i).model);
			
			phantomModels.at(i).model->deleteAllChildren();
			phantomModels.at(i).model->deleteCollisionDetector(true);

			phantomModels.at(i).model->removeFromGraph();
			delete phantomModels.at(i).model;
			phantomModels.erase(phantomModels.begin() + i);
			i=-1;	//phantomModels.size() has been changed after "erase" so....restart loop!
		//}
	}
	//compute global reference frames for each object
	world->computeGlobalPositions(false);

	return 1;
}

//void PhantomManager::removeModel(Ogre::String name)
//{
//	for(unsigned int i=0; i<phantomModels.size(); i++)
//	{
//		if(phantomModels.at(i).name == name)
//		{
//			phantomModels.at(i).model->removeFromGraph();
//			delete phantomModels.at(i).model;
//			phantomModels.erase(phantomModels.begin() + i);
//			break;
//		}
//	}
//}

//void PhantomManager::test_move(Ogre::String objName, Ogre::String where)
//{
//	cVector3d curPos =  phantomModels.at(getIndexByName(objName)).model->getPos();
//	if(where == "up")
//		curPos.z = curPos.z + 0.001;
//	else if(where == "down")
//		curPos.z = curPos.z - 0.001;
//	else if(where == "left")
//		curPos.y = curPos.y - 0.001;
//	else if(where == "right")
//		curPos.y = curPos.y + 0.001;
//	else if(where == "back")
//		curPos.x = curPos.x - 0.001;
//	else if(where == "front")
//		curPos.x = curPos.x + 0.001;
//	phantomModels.at(getIndexByName(objName)).model->setPos(curPos);
//	phantomModels.at(getIndexByName(objName)).model->computeGlobalPositions(true);
//
//	Ogre::Vector3 ogreCurPos;
//	ogreCurPos.x = curPos.x;
//	ogreCurPos.y = curPos.y;
//	ogreCurPos.z = curPos.z;
//	Ogre::LogManager::getSingleton().logMessage("----test_move(" + objName + ")-> curPos: (" + Ogre::StringConverter::toString(ogreCurPos.x) + ", " + Ogre::StringConverter::toString(ogreCurPos.y) + ", " + Ogre::StringConverter::toString(ogreCurPos.z) + ")");
//}

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

//void PhantomManager::debug_logChaiAndOgreCursorPosition()
//{
//	Ogre::Vector3 ogreCursorPos = nxManager::Instance().phantomCursorNxBody->getGlobalPosition();
//	
//	cVector3d chaiCursorPos = tool->m_deviceGlobalPos;
//	Ogre::Vector3 tmpChaiCursorPos = Ogre::Vector3(0,0,0);
//	tmpChaiCursorPos.x = chaiCursorPos.x;
//	tmpChaiCursorPos.y = chaiCursorPos.y;
//	tmpChaiCursorPos.z = chaiCursorPos.z;
//
//	Ogre::LogManager::getSingleton().logMessage("...Cursor Position -> CHAI: " + Ogre::StringConverter::toString(tmpChaiCursorPos) + ", OGRE: " + Ogre::StringConverter::toString(ogreCursorPos));
//}

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
	/************************ 3D Cursor CHAI position ***************************************/
	/*ME PALIO CHAI  (0.408296, -0.996335, -0.0331316)	  (0.398164, 0.596736, -0.0341391)  */
	/*			             																*/
	/*  (0.456283, -0.999814, -1.1638)		  (0.459533, 0.590303, -1.15817)                */
	/****************************************************************************************/
	
	//(-0.225896, -0.667207, 0.713535)		 (-0.233297, 0.688644, 0.700478)
	//       			(-0.225609, 0.00728812, 0.344537)
	//(-0.257904, -0.679591, 0.0268007)		 (-0.250408, 0.695956, 0.0308991)

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


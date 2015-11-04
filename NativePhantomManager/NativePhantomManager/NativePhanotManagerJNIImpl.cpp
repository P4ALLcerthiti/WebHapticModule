#include "stdafx.h"
#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>

#include "Graph2Obj.h"


#include "com_certh_iti_haptics_NativePhantomManager.h"

#include "PhantomManager.h"



JNIEXPORT jint JNICALL Java_com_certh_iti_haptics_NativePhantomManager_Initialize(JNIEnv *, jclass)
{	
	return PhantomManager::Instance().Init();
}

JNIEXPORT jint JNICALL Java_com_certh_iti_haptics_NativePhantomManager_close(JNIEnv *, jclass)
{	
	return PhantomManager::Instance().close();
}

JNIEXPORT jfloatArray JNICALL Java_com_certh_iti_haptics_NativePhantomManager_Update(JNIEnv *env, jclass)
{	
	float* curCursorPos = PhantomManager::Instance().updateHaptics();

	jfloatArray ret = (jfloatArray)env->NewFloatArray(3);
	env->SetFloatArrayRegion(ret, 0, 3, curCursorPos);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_certh_iti_haptics_NativePhantomManager_LoadModel(JNIEnv *env, jclass, jstring objFilename, jdouble scaleFactor, jdouble stiffness, jdouble friction)
{	
	const char *objFilenameString = env->GetStringUTFChars(objFilename, 0);
	
	PhantomManager::Instance().LoadModel(objFilenameString, 
				objFilenameString, 
				cVector3d(0, 0, 0),
				0.0, 
				90.0, 
				90.0, 
				scaleFactor,
				stiffness,
				friction);

	env->ReleaseStringUTFChars(objFilename, objFilenameString);

	return 7;
}

JNIEXPORT jint JNICALL Java_com_certh_iti_haptics_NativePhantomManager_moveModel(JNIEnv *env, jclass, jstring objFilename, jdouble newPosX, jdouble newPosY, jdouble newPosZ)
{	
	const char *objFilenameString = env->GetStringUTFChars(objFilename, 0);
	return PhantomManager::Instance().moveModel(objFilenameString, newPosX, newPosY, newPosZ);
}

JNIEXPORT jint JNICALL Java_com_certh_iti_haptics_NativePhantomManager_removeModel(JNIEnv *env, jclass, jstring objFilename)
{	
	const char *objFilenameString = env->GetStringUTFChars(objFilename, 0);
	return PhantomManager::Instance().removeModel(objFilenameString);
}

JNIEXPORT jstring JNICALL Java_com_certh_iti_haptics_NativePhantomManager_increaseModelSize(JNIEnv *env, jclass, jstring objFilename)
{	
	const char *objFilenameString = env->GetStringUTFChars(objFilename, 0);
	std::string tmpCurScale = PhantomManager::Instance().increaseModelSize(objFilenameString);
	jstring jst = env->NewStringUTF(tmpCurScale.c_str());
	return jst;
}

JNIEXPORT jstring JNICALL Java_com_certh_iti_haptics_NativePhantomManager_decreaseModelSize(JNIEnv *env, jclass, jstring objFilename)
{	
	const char *objFilenameString = env->GetStringUTFChars(objFilename, 0);
	std::string tmpCurScale = PhantomManager::Instance().decreaseModelSize(objFilenameString);
	jstring jst = env->NewStringUTF(tmpCurScale.c_str());
	return jst;
}

JNIEXPORT jint JNICALL Java_com_certh_iti_haptics_NativePhantomManager_clearModels(JNIEnv *, jclass)
{	
	return PhantomManager::Instance().clearModels();
}

JNIEXPORT jstring JNICALL Java_com_certh_iti_haptics_NativePhantomManager_moveMapLeft(JNIEnv *env, jclass)
{	
	std::string tmpMapCurPos = PhantomManager::Instance().moveMapLeft();
	jstring jst = env->NewStringUTF(tmpMapCurPos.c_str());
	return jst;
}

JNIEXPORT jstring JNICALL Java_com_certh_iti_haptics_NativePhantomManager_moveMapRight(JNIEnv *env, jclass)
{	
	std::string tmpMapCurPos = PhantomManager::Instance().moveMapRight();
	jstring jst = env->NewStringUTF(tmpMapCurPos.c_str());
	return jst;
}

JNIEXPORT jstring JNICALL Java_com_certh_iti_haptics_NativePhantomManager_moveMapUp(JNIEnv *env, jclass)
{	
	std::string tmpMapCurPos = PhantomManager::Instance().moveMapUp();
	jstring jst = env->NewStringUTF(tmpMapCurPos.c_str());
	return jst;
}

JNIEXPORT jstring JNICALL Java_com_certh_iti_haptics_NativePhantomManager_moveMapDown(JNIEnv *env, jclass)
{	
	std::string tmpMapCurPos = PhantomManager::Instance().moveMapDown();
	jstring jst = env->NewStringUTF(tmpMapCurPos.c_str());
	return jst;
}

JNIEXPORT jstring JNICALL Java_com_certh_iti_haptics_NativePhantomManager_moveMapForward(JNIEnv *env, jclass)
{	
	std::string tmpMapCurPos = PhantomManager::Instance().moveMapForward();
	jstring jst = env->NewStringUTF(tmpMapCurPos.c_str());
	return jst;
}

JNIEXPORT jstring JNICALL Java_com_certh_iti_haptics_NativePhantomManager_moveMapBack(JNIEnv *env, jclass)
{	
	std::string tmpMapCurPos = PhantomManager::Instance().moveMapBack();
	jstring jst = env->NewStringUTF(tmpMapCurPos.c_str());
	return jst;
}

JNIEXPORT jstring JNICALL Java_com_certh_iti_haptics_NativePhantomManager_testRotMatrix(JNIEnv *env, jclass)
{	
	std::string tmpRotMatr = PhantomManager::Instance().testRotMatrix();
	jstring jst = env->NewStringUTF(tmpRotMatr.c_str());
	return jst;
}

JNIEXPORT jstring JNICALL Java_com_certh_iti_haptics_NativePhantomManager_getObjNameInCollisionWithPhantom(JNIEnv *env, jclass)
{	
	std::string tmpRotMatr = PhantomManager::Instance().getObjNameInCollisionWithPhantom();
	jstring jst = env->NewStringUTF(tmpRotMatr.c_str());
	return jst;
}

JNIEXPORT jint JNICALL Java_com_certh_iti_haptics_NativePhantomManager_resetPhantomPosition(JNIEnv *, jclass, jdouble fromZ, jdouble toZ)
{	
	return PhantomManager::Instance().resetPhantomPosition(fromZ, toZ);
}

JNIEXPORT jint JNICALL Java_com_certh_iti_haptics_NativePhantomManager_createObj
(JNIEnv* env, jclass, jobjectArray vVertex, jint numVertex, jobjectArray vLink, jint numLink, jstring strFilename, jdouble dDitchWidth, jint numBuildings, jintArray vNumBuildingVertices, jobjectArray vIdxBuilding)
{
	SimpleGraph* graph = new SimpleGraph();
	double p[3] = {0,0,0};

	jdoubleArray dArray;
	jintArray iArray;
	jdouble* dDouble;
	jint* iInt;
	jint* iInt2;
	for (int i=0; i<numVertex; i++)
	{
		dArray = (jdoubleArray)env->GetObjectArrayElement(vVertex, i);
		dDouble = env->GetDoubleArrayElements(dArray,0);
		p[0] = dDouble[0];
		p[1] = dDouble[1];
		p[2] = dDouble[2];
		graph->AddNode(new SimpleNode(p, p[2]));

		env->ReleaseDoubleArrayElements(dArray, dDouble, 0);
	}

 	for (int i=0; i<numLink; i++)
 	{
		iArray = (jintArray)env->GetObjectArrayElement(vLink, i);
		iInt = env->GetIntArrayElements(iArray,0);
		graph->AddLink(iInt[0], iInt[1]);
		env->ReleaseIntArrayElements(iArray, iInt, 0);
 	}

	std::vector < std::vector<unsigned int> > vB;

	iInt2 = env->GetIntArrayElements(vNumBuildingVertices,0);

	for (int i=0; i<numBuildings; i++)
	{
		std::vector <unsigned int> vBuild;
		iArray = (jintArray)env->GetObjectArrayElement(vIdxBuilding, i);
		iInt = env->GetIntArrayElements(iArray,0);

		for (int j=0; j<iInt2[i]; j++)
		{
			vBuild.push_back(iInt[j]);
		}
		env->ReleaseIntArrayElements(iArray, iInt, 0);

		vB.push_back(vBuild);
	}

	graph->WriteToObj("testMap.obj", dDitchWidth, vB);

	//clearing
	delete graph;

	return 0;
}
#pragma once
#include <PxPhysicsAPI.h>
#include <PxToolkit.h>

#include <malloc.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <wchar.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <vehicle/PxVehicleSDK.h>

#include <irrlicht.h>												
#include <driverChoice.h>																			
#include <Windows.h>												
#include <iostream>													
#include <driverChoice.h>											
#include <time.h>

#include "documentation.h"											
#include "audio.h"		
#include "libraries.h"
#include "ICustomEventReceiver.h"

using namespace physx;
using namespace std;

enum E_ACTOR_CREATION
{
	eAC_Sphere = 0,
	eAC_Capsule,
	eAC_Box,
	eAC_Convex,
	eAC_Face,
	eAC_Block,
	eAC_COUNT //do not use!
};

static const PxReal gCharacterScale = 0.1;

static const PxReal gSphereRadius[] = { 
	2.0f,  // root

	// left lower limb
	1.5f,  // lhipjoint
	1.5f,  // lfemur (upper leg)
	1.0f,  // ltibia (lower leg)
	1.0f,  // lfoot
	1.0f,  // ltoes

	// right lower limb
	1.5f,  // rhipjoint
	1.5f,  // rfemur
	1.0f,  // rtibia
	1.0f,  // rfoot
	1.0f,  // rtoes

	// spine
	2.5f,  // lowerback
	2.5f,  // upperback
	2.0f,  // thorax
	1.0f,  // lowerneck
	1.0f,  // upperneck

	2.0f,  // head

	// left arm/hand
	1.5f,  // lclavicle (shoulder)
	1.5f,  // lhumerus (upper arm)
	1.3f,  // lradius (lower arm)
	1.5f,  // lwrist
	1.8f,  // lhand
	0.5f,  // lfingers
	0.5f,  // lthumb

	// right arm/hand
	1.5f,  // rclavicle
	1.5f,  // rhumerus
	1.3f,  // rradius
	1.5f,  // rwrist
	1.8f,  // rhand
	0.5f,  // rfingers
	0.5f,  // rthumb
};

class CMutex
{
protected:
	//PxFoundation is declared outside because whilst inside creation assertion error 
	//occurs
	physx::PxFoundation*			mFoundation;
	//------------------------------top-level PhysX
	bool recordMemoryAllocations;
	physx::PxProfileZoneManager*	mProfileZoneManager;
	physx::PxPhysics*				mPhysX;
	//------------------------------PxCooking creating, converting and serializing data
	physx::PxCooking*				mCooking;
	physx::PxCookingParams			mCp;
	PxToolkit::MemoryOutputStream	wb;
	//------------------------------PxSceneDesc
	physx::PxScene*					mScene;
	physx::PxDefaultCpuDispatcher*	mCpuDispatcher;
	physx::PxU32					mNbThreads;
	physx::pxtask::CudaContextManager* mCudaContextManager;
	physx::PxVec3					mGravity;
	//----------------simulation--------------------
	physx::PxReal					mAccumulator,
									mStepSize;
	bool							m_bInitialDebugRender;
	physx::PxReal					mDebugRenderScale;
	//------------------------------Materials
	physx::PxMaterial*				mMaterial;
	//------------------------------PxActors
		physx::PxRigidDynamic*			mActor;
		physx::PxShape*					mShape;
		E_ACTOR_CREATION				mCreation;
		////////////////////////////////////
	std::vector<physx::PxRigidActor*> mvActors;
	std::vector<physx::PxRigidActor*>::iterator iterator;
	physx::PxTransform				pose;
	physx::PxReal					radius, density;
	physx::PxGeometry*				mGeometry;
	
	//------------------------------Static ground plane
	physx::PxRigidStatic*			plane;
	//------------------------------Character Cotroller
	physx::PxControllerManager*		ConManager;
	physx::PxRigidDynamic*			playerShape;
	//------------------------------Joint
	physx::PxFixedJoint*			mJoint;
	//------------------------------Cloth
	physx::PxClothMeshDesc			mMeshDesc;
	physx::PxCloth*					mCloth;
	physx::PxClothCollisionData		cd;
	physx::PxClothFabric*			mFabric;
	physx::PxClothParticle*			mPoints;
	physx::PxClothCollisionPlane	mPlane;
	physx::PxClothPhaseSolverConfig	mCps;
	std::vector<PxVec3>				mPos;
	std::vector<PxVec3>				mNormal;
	std::vector<PxU32>				mIndices;
	//------------------------------PVD
	const char*						pvd_host_ip;
	const char*						fileName;
	int								port;
	unsigned int					timeOut;
	PVD::PxVisualDebuggerConnectionFlags conFlags;
	PVD::PvdConnection*				pCon;
	//------------------------------Pyramid
	physx::PxConvexMesh*			pyr;
	physx::PxConvexMeshDesc			pyrDesc;
	physx::PxShape*					pyrShape;
	physx::PxRigidDynamic*			pyrAct;
	//------------------------------Heightfield
	physx::PxHeightFieldDesc*		hfDesc;
	physx::PxHeightField*			hf;
	physx::PxHeightFieldGeometry*	hfGeom;
	physx::PxShape*					hfShape;
public:
	//Description
	//Constructor
	//No.1 for the initial setup very important do not ommit !
	CMutex(physx::PxFoundation* Foundation, 
		physx::PxSimulationFilterShader filter = NULL,
		physx::pxtask::CudaContextManager* mCudaContextManager = NULL):mFoundation(Foundation)
	{
		//creating base PxFoundation essential for top-level PhysX
		if( !mFoundation )
			printf("Foundation creation failed!\n");
		else
			printf("Foundation succesfull\n");
		//top level PhysX
		recordMemoryAllocations = true;
		mProfileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager( mFoundation );
		if( !mProfileZoneManager )
			printf("Creation of Zone Manager has failed\n");
		else
			printf("Creation of Zone Manager was a success.\n");
		mPhysX = PxCreatePhysics( PX_PHYSICS_VERSION, 
									*mFoundation, 
									physx::PxTolerancesScale(), //update 19-10-2012
									recordMemoryAllocations,
									mProfileZoneManager );
		if( !mPhysX )
			printf("Creation of PhysX has failed\n");
		else
			printf("Creation of PhysX was a success.\n");
		//PxCooking
		mCp = physx::PxCookingParams();
		mCooking = PxCreateCooking( PX_PHYSICS_VERSION, 
									*mFoundation,
									mCp);
		if( !mCooking )
			printf("Cooking creation failed!!!\n");
		else
			printf("Cooking creation successfull.\n");
		//PxSceneDesc - setting up the scene
		mNbThreads = 1;
		mMaterial = mPhysX->createMaterial( 0.5f, 0.5f, 0.1f );
		physx::PxSceneDesc sceneDesc = mPhysX->getTolerancesScale();
		mGravity = physx::PxVec3( 0.f, -9.81f, 0.f );
		sceneDesc.gravity = mGravity;
		if( !sceneDesc.cpuDispatcher )
		{
			mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate( mNbThreads );
			sceneDesc.cpuDispatcher = mCpuDispatcher;
		}
		if( !sceneDesc.filterShader )
			sceneDesc.filterShader = PxDefaultSimulationFilterShader;
#ifdef PX_WINDOWS
		if( !sceneDesc.gpuDispatcher && mCudaContextManager )
		{
			printf("If something wrong I am here.\n");
			sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
		}
#endif
		//wtf ?
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
		sceneDesc.flags |= PxSceneFlag::eENABLE_SWEPT_INTEGRATION;//this is to enable the CCD(Continous Collision Detection )
		mScene = mPhysX->createScene( sceneDesc );

		CMutex::mDebugRenderScale = 0.5f;
		physx::PxSceneFlags flags = mScene->getFlags();
		mScene->setVisualizationParameter( physx::PxVisualizationParameter::eSCALE, m_bInitialDebugRender ? mDebugRenderScale : 0.f );
		mScene->setVisualizationParameter( physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.f );
		mScene->setVisualizationParameter( PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.f);
		mScene->setVisualizationParameter( PxVisualizationParameter::eJOINT_LIMITS, 1.f);
		//adding static ground plane and adding it to the simulation
		//jaworekplay
		//EDIT:When we change the "1" in PxQuat(1,x) the force of polarity of gravity is being changed.
		//EDIT:changed "1" in PxTransform(1,2,3) to 10 from 0
		pose = PxTransform(PxVec3(0.f,0.f,0.f), PxQuat(PxHalfPi, PxVec3(0.f,0.f,1.f) ) );
		plane = mPhysX->createRigidStatic(pose);
		plane->createShape( PxPlaneGeometry(), *mMaterial );
		mScene->addActor( *plane );
		//for simulation loop
		mAccumulator = 0.f;
		mStepSize = 1.f / 60.f;
		//character controller
		ConManager = PxCreateControllerManager( *mFoundation );
	}
	//Description
	//Constructor
	//No.2 for accessing the protected data
	CMutex(){};
	//Description
	//To allow simulation to finish
	//just call mScene->fetchResults(true);
	//GOOD IDEA put it in separate function and make it available through pointer
	//Read more in "Data Access and Buffering"
	virtual bool advance( physx::PxReal dt )
	{
		mAccumulator += dt;
		if( mAccumulator < mStepSize )
			return false;
		mAccumulator -= mStepSize;
		mScene->simulate( mStepSize );
		mScene->fetchResults(true);
		return true;
	}
	virtual physx::PxRigidDynamic* shootABullet()
	{
		return CMutex::mActor;
	}
	//Description
	//Please do not use for now as this function is not yet developed.
	virtual physx::PxHeightField* createHeightField()
	{
		//(PxHeightFieldSample*)alloc(sizeof(PxHeightFieldSample)*(numRows*numCols));

		return hf;
	}

	//For now only a ccapsule equivalent of the real shape
	//will see if the values of the height and the radius are correct
	virtual physx::PxRigidDynamic* createPlayerShape()
	{
		density = 500.f;
		PxRigidDynamic* aCapsuleActor = mPhysX->createRigidDynamic(PxTransform::createIdentity() );
		aCapsuleActor->setRigidDynamicFlag( PxRigidDynamicFlag::eKINEMATIC, true );
		PxTransform relativePose(PxQuat(PxHalfPi, physx::PxVec3(0,0,1)));
		PxShape* aCapsuleShape = aCapsuleActor->createShape(PxCapsuleGeometry(physx::PxReal(20.f), physx::PxReal(30.f)), *mMaterial, relativePose);
		PxRigidBodyExt::updateMassAndInertia(*aCapsuleActor, density);
		mScene->addActor( *aCapsuleActor );
		return aCapsuleActor;
	}

	bool setPlayerPosition( PxVec3 pos , physx::PxRigidDynamic& act)
	{
		pose = act.getGlobalPose();
		pose.p.x = pos.x;
		pose.p.y = pos.y;
		pose.p.z = pos.z;
		act.setGlobalPose( pose );
		return true;
	}
	//Description
	//This function will create an actor
	//either static or rigid: kinematic or ragdoll
	//Parameter 1 is enumeration of shapes available, DO NOT USE eAC_COUNT, Parameter 2 is position where new actor should be spawned.
	//TO DO
	virtual physx::PxRigidDynamic* CreateActor( E_ACTOR_CREATION Creation, PxVec3& position, PxVec3& target = PxVec3(0))
	{
		mCreation = Creation;
		mPhysX->getMaterials( &mMaterial, 1 );
		density = PxReal(500.f);
		mActor = mPhysX->createRigidDynamic( physx::PxTransform( position ) );
		PxVec3 velocity = target;
		PxReal sphereRadius(5.f);
		switch( mCreation )
		{
		case eAC_Sphere:
			mShape = mActor->createShape( physx::PxSphereGeometry( sphereRadius ), *mMaterial );
			std::cout << "Created A Sphere\n";
			mActor->userData = (void*)mCreation;
			density = 100.f;
			break;
		case eAC_Box:
			mShape = mActor->createShape(physx::PxBoxGeometry(2.5f,2.5f,2.5f),*mMaterial );
			mActor->userData = (void*)mCreation;
			density = 150.f;
			break;
		case eAC_Capsule:
			mShape = mActor->createShape(physx::PxCapsuleGeometry(5.f,2.5f), *mMaterial );
			mActor->userData = (void*)mCreation;
			break;
		case eAC_Convex :
			return CreatePyramid(PxVec3(0));
			mActor->userData = (void*)mCreation;
			break;
		// Leo
		case eAC_Face:
			mShape = mActor->createShape(physx::PxBoxGeometry(5.f,5.f,5.f), *mMaterial);
			mActor->userData = (void*)mCreation;
			break;
		default:
			std::printf("Unknown creation parameter, please specify a valid name\n");
			break;
		}
		mActor->setMass( density );
		physx::PxRigidBodyExt::updateMassAndInertia( *mActor, density );
		//specifying initial linear velocity
		mActor->setLinearVelocity( velocity );
		//add the actor to the scene
		mScene->addActor( *mActor );
		std::printf("Creation of the actor was a success!\n");
		return mActor;
		
	}
	//Description
	//This is a trial to pass the value of the scene
	//to create an actor outside the base class
	virtual physx::PxScene* Scene() const { return mScene; }

	//Description
	//This function should translate rotations from the PhysX quartternion and matrix to irrlicht vector one
	virtual void Rotate() //
	{
		PxMat33 mat = PxMat33::PxMat33( mActor->getGlobalPose().q );
		irr::core::matrix4 irrM;
		irr::f32 fM[16];
		fM[0] = mat.column0.x;
		fM[1] = mat.column0.y;
		fM[2] = mat.column0.z;
		fM[4] = mat.column1.x;
		fM[5] = mat.column1.y;
		fM[6] = mat.column1.z;
		fM[8] = mat.column2.x;
		fM[9] = mat.column2.y;
		fM[10] = mat.column2.z;
		irrM.setM( fM );
	}

	void getResults(){mScene->fetchResults(true);}

	const int CUDA(){if(mCudaContextManager) return 1; return 0;}

	virtual void moveLeft(){mActor->addForce(	PxVec3(-100.f,0.f,0.f), physx::PxForceMode::eIMPULSE );}
	virtual void moveRight(){mActor->addForce(	PxVec3(100.f,0.f,0.f), physx::PxForceMode::eIMPULSE );}
	virtual void Jump(){mActor->addForce(		PxVec3(0.f,10.f,0.f), physx::PxForceMode::eIMPULSE);}
	virtual void moveIn(){mActor->addForce(		PxVec3(0.f,0.f,-100.f), physx::PxForceMode::eIMPULSE );}
	virtual void moveForward(){mActor->addForce( PxVec3(0.f,0.f,100.f), physx::PxForceMode::eIMPULSE );}

	//Description
	//For correct usage please pass actors like this:
	// ->actor2
	// ->actor1
	virtual void joinActors(physx::PxRigidActor* actor1,physx::PxRigidDynamic* actor2) // Leo 
	{
		PxJointLimitPair* limitPair = new PxJointLimitPair(-PxPi/4, PxPi/4, 0.1f);
		limitPair->spring = 10.f;
		limitPair->damping = 2.f;
		mJoint = physx::PxFixedJointCreate(*mPhysX, 
													actor1, actor2->getGlobalPose(), 
													actor2, actor1->getGlobalPose());
		mJoint->setConstraintFlag( PxConstraintFlag::eVISUALIZATION,true );
		mJoint->setProjectionLinearTolerance(0.1f);
		//mJoint->setLimit(*limitPair); // upper, lower, tolerance
		//mJoint->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		delete limitPair;
	} 

	//Description
	//This function should be able to create cloth in PhysX engine not in the Irrlicht one!!!
	virtual physx::PxCloth* CreateCloth()
	{
	//	// compute root transform and positions of all the bones
	//	/*PxTransform rootPose;*/
	//	vector<PxVec3> positions;	
	//	vector<PxU32> indexPairs;
	//	/*mCharacter.getFramePose(rootPose, positions, indexPairs);*/

	//	// convert bones to collision capsules
	//	vector<PxClothCollisionSphere> spheres;
	//	vector<PxClothCollisionSphere>::iterator it;
	//	spheres.resize(positions.size());
	//	for (PxU32 i = 0; i < positions.size(); i++)
	//	{
	//		spheres[i].pos = positions[i];
	//		spheres[i].radius = gCharacterScale * gSphereRadius[i];
	//	}

	//	PxClothCollisionData collisionData;
	//	collisionData.numSpheres = static_cast<PxU32>(positions.size());
	//	collisionData.spheres = it;
	//	collisionData.numPairs = static_cast<PxU32>(indexPairs.size()) / 2; // number of capsules
	//	collisionData.pairIndexBuffer = indexPairs.begin();

	//	// create the cloth cape mesh from file
	//	PxClothMeshDesc meshDesc;
	//	SampleArray<PxVec3> vertices;
	//	SampleArray<PxU32> primitives;
	//	SampleArray<PxReal> uvs;
	//	const char* capeFileName = getSampleMediaFilename("ctdm_cape_m.obj");
	//	PxReal clothScale = gCharacterScale * 0.3f;
	//	PxVec3 offset = PxVec3(0,-1.5,0); 
	//	PxQuat rot = PxQuat(0, PxVec3(0,1,0));
	//	Test::ClothHelpers::createMeshFromObj(capeFileName, clothScale, &rot, &offset, 
	//		vertices, primitives, &uvs, meshDesc);

	//	if (!meshDesc.isValid()) fatalError("Could not load ctdm_cape_m.obj\n");
	//	// create the cloth
	//	PxCloth& cloth = *createClothFromMeshDesc(
	//		meshDesc, rootPose, &collisionData, PxVec3(0,-1,0),
	//		uvs.begin(), "dummy_cape_d.bmp", PxVec3(0.5f, 0.5f, 0.5f), 0.8f);

	//	mCape = &cloth;

	//	// attach top verts
	//	PxClothReadData* readData = cloth.lockClothReadData();
	//	PX_ASSERT(readData);
	//	PxU32 numParticles = cloth.getNbParticles();
	//	SampleArray<PxClothParticle> particles(numParticles);
	//	SampleArray<PxVec3> particlePositions(numParticles);
	//	for(PxU32 i = 0; i < numParticles; i++)
	//	{
	//		particles[i].pos = readData->particles[i].pos;
	//		particles[i].invWeight = (uvs[i*2+1] > 0.85f) ? 0.0f : readData->particles[i].invWeight;
	//		particlePositions[i] = readData->particles[i].pos;
	//	}
	//	readData->unlock();
	//	cloth.setParticles(particles.begin(), particles.begin());

	//	// compute initial skin binding to the character
	//	mSkin.bindToCharacter(mCharacter, particlePositions);

	//	// set solver settings
	//	cloth.setSolverFrequency(240);

	//	// damp global particle velocity to 90% every 0.1 seconds
	//	cloth.setDampingCoefficient(0.1f); // damp local particle velocity
	//	cloth.setDragCoefficient(0.1f); // transfer frame velocity

	//	// reduce effect of local frame acceleration
	//	cloth.setInertiaScale(0.3f);
	//
	//	const bool useVirtualParticles = true;
	//	const bool useSweptContact = true;
	//	const bool useCustomConfig = true;

	//	// virtual particles
	//	if (useVirtualParticles)
	//		Test::ClothHelpers::createVirtualParticles(cloth, meshDesc, 4);

	//	// ccd
	//	cloth.setClothFlag(PxClothFlag::eSWEPT_CONTACT, useSweptContact);

	//	// use GPU or not
	//#if PX_SUPPORT_GPU_PHYSX
	//	cloth.setClothFlag(PxClothFlag::eGPU, mUseGPU);
	//#endif

	//	// custom fiber configuration
	//	if (useCustomConfig)
	//	{
	//		PxClothPhaseSolverConfig config;

	//		config = cloth.getPhaseSolverConfig(PxClothFabricPhaseType::eSTRETCHING);
	//		config.solverType = PxClothPhaseSolverConfig::eSTIFF;
	//		config.stiffness = 1.0f;
	//		cloth.setPhaseSolverConfig(PxClothFabricPhaseType::eSTRETCHING, config);

	//		config = cloth.getPhaseSolverConfig(PxClothFabricPhaseType::eSTRETCHING_HORIZONTAL);
	//		config.solverType = PxClothPhaseSolverConfig::eFAST;
	//		config.stiffness = 1.0f;
	//		cloth.setPhaseSolverConfig(PxClothFabricPhaseType::eSTRETCHING_HORIZONTAL, config);

	//		config = cloth.getPhaseSolverConfig(PxClothFabricPhaseType::eSHEARING);
	//		config.solverType = PxClothPhaseSolverConfig::eFAST;
	//		config.stiffness = 0.75f;
	//		cloth.setPhaseSolverConfig(PxClothFabricPhaseType::eSHEARING, config);

	//		config = cloth.getPhaseSolverConfig(PxClothFabricPhaseType::eBENDING_ANGLE);
	//		config.solverType = PxClothPhaseSolverConfig::eBENDING;
	//		config.stiffness = 0.5f;
	//		cloth.setPhaseSolverConfig(PxClothFabricPhaseType::eBENDING_ANGLE, config);
	//	}
	return mCloth;
	}

	//Description
	//This method will create connection between PhysX application
	//and PC with PVD on over the network.
	//returns true if connection was a success.
	//TODO:
	//pass ip address to the PC.

	virtual const bool StartPvdNetwork()
	{
		printf("Starting connection with PVD ...\n");
		//check for compatybility with the current system
		if( mPhysX->getPvdConnectionManager() == NULL )
		{
			printf("Connection failed, could not establish manager!\n");
			return false;
		}
		//connection parameters
		pvd_host_ip = "127.0.0.1";	//address IP with PVD running
		port		= 5425;			//TCP port to connect to,where PVD is listening
		timeOut		= 100;			//timeout in milliseconds to wait for PVD respond
									//consoles and remote PCs need a higher timeout.
		printf("Cration of parameters finished.\n");
		conFlags = PVD::PxVisualDebuggerExt::getAllConnectionFlags();
		printf("Flags created.\n");
		pCon = PVD::PxVisualDebuggerExt::createConnection( mPhysX->getPvdConnectionManager(),
			pvd_host_ip,port,timeOut,conFlags);
		printf("Creation of network connection to PVD was a success!\n");
		return true;
	}
	//Description
	//This method will create file that can be opened by PVD.
	//Returns true if creation was success
	virtual const bool StartPvdFile()
	{
		printf("Checking compatybility.\n");
		//check for compatybility with the current system
		if( mPhysX->getPvdConnectionManager() == NULL )
		{
			printf("Could not establish manager");
			return false;
		}
		//file parameters
		fileName = "D:/PvdCapture.pxd2";	//name of the file containing capture
		conFlags = PVD::PxVisualDebuggerExt::getAllConnectionFlags();
		//attempt to connect
		pCon = PVD::PxVisualDebuggerExt::createConnection( mPhysX->getPvdConnectionManager(),fileName, conFlags );
		printf("saving to file should work now.\n");
		return true;
	}
	virtual physx::PxQuat QuatFromAxisAngle(const irr::core::vector3df axis, float angleInRadians)
	{
		 PxQuat result;
		 float angle = angleInRadians / 2.0f;
		 float sinAngle = sin(angle);
		 irr::core::vector3df n = axis;
		 n.normalize();

		 result.w = cos(angle);
		 result.x = n.X * sinAngle;
		 result.y = n.Y * sinAngle;
		 result.z = n.Z * sinAngle;

		 return result;
	}
	//Description
	//Creates not pyramid but the tetragen
	// Param 1 - position PxVec3
	// Param 2 - rotation vector3df
	virtual physx::PxRigidDynamic* CreatePyramid(PxVec3& position = PxVec3(0), irr::core::vector3df rotation = irr::core::vector3df(0))
	{
		//position = PxTransform::createIdentity().p;
		PxQuat orientation = PxTransform::createIdentity().q;
		if( rotation.X != 0 )
			orientation *= QuatFromAxisAngle( rotation, rotation.X);
		if( rotation.Y != 0 )
			orientation *= QuatFromAxisAngle( rotation, rotation.Y);
		if( rotation.Z != 0 )
			orientation *= QuatFromAxisAngle( rotation, rotation.Z);
		//                      iV - tip           I          II              III     
		PxVec3 verts[] = {PxVec3(0,7,0), PxVec3(10,0,0), PxVec3(-10,0,0), PxVec3(0,0,10)};
		pyrDesc.points.count = 4;
		pyrDesc.points.stride = sizeof(PxVec3);
		pyrDesc.points.data = verts;
		pyrDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
		PxToolkit::MemoryOutputStream buf;
		if( !mCooking->cookConvexMesh( pyrDesc, buf ) )
		{
			printf("Creation of convex mesh has failed!\n");
			return false;
		}
		else
		{
			printf("Convex shape creation - SUCCESS\n");
			PxToolkit::MemoryInputData input(buf.getData(), buf.getSize() );
			pyr = mPhysX->createConvexMesh( input );
			pyrAct = mPhysX->createRigidDynamic( physx::PxTransform( position, orientation) );
			pyrAct->setMass( 500.f );
			pyrShape = pyrAct->createShape( physx::PxConvexMeshGeometry( pyr ), *mMaterial );// remember to give dereferenced pointer for materials ^_^
			pyrAct->userData = verts;
			pyrShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
			//pyrAct->setRigidDynamicFlag( PxRigidDynamicFlag::eKINEMATIC,true);
			PxRigidBodyExt::updateMassAndInertia( *pyrAct,100.f);
			mScene->addActor( *pyrAct );
			mvActors.push_back(pyrAct);
			printf("STATUS || Convex mesh = TRUE;\n");
			return pyrAct;
		}
	return false;
	}

	virtual physx::PxFoundation* FoundationCallback(){return mFoundation;}

	//Destructor
	~CMutex()
	{
		mPhysX->release();
		//mFoundation->release();
		if( pCon )
			pCon->release();
	}
	/*class PxDefaultAllocator : public physx::PxAllocatorCallback
	{
		void* allocate( size_t size, const char*, const char*, int)
		{
			return _aligned_malloc( size, 16 );
		}
		void deallocate(void* ptr)
		{
			_aligned_free(ptr);
		}
	};*/
};


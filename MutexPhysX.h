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

enum E_ACTOR_CREATION
{
	eAC_Sphere = 0,
	eAC_Capsule,
	eAC_Box,
	eAC_Convex,
	eAC_Face,
	eAC_COUNT //do not use!
};

using namespace std;
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
		//--------------------SPHERE-----------------
		physx::PxRigidDynamic*			mActor;
		physx::PxShape*					mShape;
		/////////////////////////////////////////////
		E_ACTOR_CREATION				mCreation;
		//--------------------GENERIC ACTOR - CAN BE ANYTHING
public:
		physx::PxTriangleMesh*		mTrialMesh;
		physx::PxTriangleMeshDesc*  mTrialMeshDesc;
		physx::PxTriangleMeshGeometry* mTrialGeom;
protected:
		/////////////////////////////////////////////////////
	std::vector<physx::PxRigidActor*> mvActors;
	std::vector<physx::PxRigidActor*>::iterator iterator;
	physx::PxTransform				pose;
	physx::PxReal					radius, density;
	physx::PxGeometry*				mGeometry;
	
	//------------------------------Static ground plane
	physx::PxRigidStatic*			plane;
	//------------------------------Character Cotroller
	physx::PxControllerManager*		ConManager;
	//------------------------------Joint
	physx::PxRevoluteJoint*			mJoint;
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
	int								m_verts[15];
	//------------------------------Plane
	physx::PxPlane*					m_plane;
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
			cout << "Foundation creation failed!\n";
		else
			cout << "Foundation succesfull\n";
		//top level PhysX
		recordMemoryAllocations = true;
		mProfileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager( mFoundation );
		if( !mProfileZoneManager )
			cout << "Creation of Zone Manager has failed\n";
		else
			cout << "Creation of Zone Manager was a success.\n";
		mPhysX = PxCreatePhysics( PX_PHYSICS_VERSION, 
									*mFoundation, 
									physx::PxTolerancesScale(), //update 19-10-2012
									recordMemoryAllocations,
									mProfileZoneManager );
		if( !mPhysX )
			cout << "Creation of PhysX has failed\n";
		else
			cout << "Creation of PhysX was a success.\n";
		//PxCooking
		mCp = physx::PxCookingParams();
		mCooking = PxCreateCooking( PX_PHYSICS_VERSION, 
									*mFoundation,
									mCp);
		if( !mCooking )
			cout << "Cooking creation failed!!!\n";
		else
			cout << "Cooking creation successfull.\n";
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
			cout << "If something wrong I am here.\n";
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

	virtual physx::PxCooking& getCooking() const
	{
		return *mCooking;
	}
	virtual physx::PxPhysics& getPhysX() const
	{
		return *mPhysX;
	}
	virtual physx::PxMaterial& getMaterial() const
	{
		return *mMaterial;
	}
	//Description
	//This function will create an actor
	//either static or rigid: kinematic or ragdoll
	//Parameter 1 is enumeration of shapes available, DO NOT USE eAC_COUNT, Parameter 2 is position where new actor should be spawned.
	//TO DO
	virtual physx::PxRigidDynamic* CreateActor( E_ACTOR_CREATION Creation, PxVec3& position)
	{
		mCreation = Creation;
		mPhysX->getMaterials( &mMaterial, 1 );
		density = PxReal(500.f);
		mActor = mPhysX->createRigidDynamic( physx::PxTransform( position ) );
		PxVec3 velocity(0,0,0);
		PxReal sphereRadius(5.f);
		switch( mCreation )
		{
		case eAC_Sphere:
			mShape = mActor->createShape( physx::PxSphereGeometry( sphereRadius ), *mMaterial );
			std::cout << "Created A Sphere\n";
			mActor->userData = (void*)mCreation;
			break;
		case eAC_Box:
			mShape = mActor->createShape(physx::PxBoxGeometry(5.f,5.f,5.f),*mMaterial );
			mActor->userData = (void*)mCreation;
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
			cout << "Unknown creation parameter, please specify a valid name\n";
			break;
		}
		mActor->setMass( density );
		physx::PxRigidBodyExt::updateMassAndInertia( *mActor, density );
		//specifying initial linear velocity
		mActor->setLinearVelocity( velocity );
		//add the actor to the scene
		mScene->addActor( *mActor );
		cout << "Creation of the actor was a success!\n";
		return mActor;
		
	}
	//virtual physx::PxRigidDynamic* CreateActor( physx::PxVec3& position = PxVec3(0.f,100.f,0.f),
	//											physx::PxReal sphereRadius = PxReal(10.f), 
	//											physx::PxVec3& velocity = PxVec3(0.f, 0.f,0.f) )
	//{
	//	mPhysX->getMaterials( &mMaterial, 1 );
	//	density = PxReal(150.f);
	//	mSphereActor = mPhysX->createRigidDynamic( physx::PxTransform( position ) );
	//	mSphereShape = mSphereActor->createShape( physx::PxSphereGeometry( sphereRadius ), *mMaterial );
	//	mSphereActor->setMass( density );
	//	physx::PxRigidBodyExt::updateMassAndInertia( *mSphereActor, density );
	//	//specifying initial linear velocity
	//	mSphereActor->setLinearVelocity( velocity );
	//	//add the actor to the scene
	//	mScene->addActor( *mSphereActor );
	//	cout << "Creation of the actor was a success!\n");
	//	return mSphereActor;
	//}
	//Description
	//This is a trial to pass the value of the scene
	//to create an actor outside the base class
	virtual physx::PxScene& Scene() const { return *mScene; }

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

	void getResults(){mScene->fetchResults(true);};

	const int CUDA(){if(mCudaContextManager) return 1; return 0;}

	virtual void moveLeft(){mActor->addForce(	PxVec3(-100.f,0.f,0.f), physx::PxForceMode::eIMPULSE );}
	virtual void moveRight(){mActor->addForce(	PxVec3(100.f,0.f,0.f), physx::PxForceMode::eIMPULSE );}
	virtual void Jump(){mActor->addForce(		PxVec3(0.f,10.f,0.f), physx::PxForceMode::eACCELERATION);}
	virtual void moveIn(){mActor->addForce(		PxVec3(0.f,0.f,-100.f), physx::PxForceMode::eIMPULSE );}
	virtual void moveForward(){mActor->addForce( PxVec3(0.f,0.f,100.f), physx::PxForceMode::eIMPULSE );}

	virtual void joinActors(physx::PxRigidActor *actor1,physx::PxRigidDynamic* actor2) // Leo 
	{
		PxJointLimitPair* limitPair = new PxJointLimitPair(-PxPi/4, PxPi/4, 0.1f);
		limitPair->spring = 10.f;
		limitPair->damping = 2.f;
		mJoint = physx::PxRevoluteJointCreate(*mPhysX, 
													actor1, actor1->getGlobalPose(), 
													actor2, actor2->getGlobalPose());
		mJoint->setConstraintFlag( PxConstraintFlag::eVISUALIZATION,true );
		mJoint->setProjectionLinearTolerance(0.1f);
		mJoint->setLimit(*limitPair); // upper, lower, tolerance
		mJoint->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		delete limitPair;
	} 

	//Description
	//This function should be able to create cloth in PhysX engine not in the Irrlicht one!!!
	virtual physx::PxCloth* CreateCloth()
	{
		mMeshDesc.setToDefault();
		//geometry values
		int w = 8, h = 7;
		float hw = w / 2.f;
		float hh = h / 2.f;
		float d = 0.2f;
		//					this is short hand version of casting result of division into the int type
		//				  / full version would be static_cast<int>((w / d) + 1);
		//		   |    \/_   |
		int numX = (int)(w / d) +1;
		int numY = (int)(h / d) +1;
		mMeshDesc.points.count = (numX + 1) * (numY + 1);
		mMeshDesc.triangles.count = numX * numY * 2;
		mMeshDesc.points.stride = sizeof( PxVec3 );
		mMeshDesc.triangles.stride = 3 * sizeof(PxU32);
		//if anybody struggles to understand this bit
		//answer in the box on this question
		//what is "void* ptr;" ?
		//if you can answer this then I'll explain this to you
		mMeshDesc.points.data = (PxVec3*)malloc(sizeof(PxVec3)* mMeshDesc.points.count );
		mMeshDesc.triangles.data = (physx::PxU32*)malloc( sizeof( physx::PxU32 ) * mMeshDesc.triangles.count * 3);
		mMeshDesc.edgeFlags = 0;

		//Geometry
		int i,j;
		physx::PxVec3* p = (physx::PxVec3*)mMeshDesc.points.data;
		mPos.resize( mMeshDesc.points.count );
		mNormal.resize( mMeshDesc.points.count );
		mIndices.resize( mMeshDesc.triangles.count * 3 );

		for( i = 0; i <= numY; ++i )
		{
			for( j = 0; j <= numX; ++j )
			{
				p->x = d*j-hw;
				p->y = float(h);
				p->z = d*i;
				++p;
			}
		}
		memcpy( &mPos[0].x, (mMeshDesc.points.data), sizeof(physx::PxVec3)* mMeshDesc.points.count );
		
		//Topology
		physx::PxU32* id = (physx::PxU32*)mMeshDesc.triangles.data;
		for( i = 0; i < numY; ++i )
		{
			for( j = 0; j < numX; ++j )
			{
				PxU32 i0 = i * (numX+1) + j;            
				PxU32 i1 = i0 + 1;            
				PxU32 i2 = i0 + (numX+1);            
				PxU32 i3 = i2 + 1;
				if ((j+i)%2) 
				{                
					*id++ = i0;
					*id++ = i2;
					*id++ = i1;                
					*id++ = i1;
					*id++ = i2;
					*id++ = i3;            
				} 
				else 
				{                
					*id++ = i0;
					*id++ = i2;
					*id++ = i3;                
					*id++ = i0;
					*id++ = i3;
					*id++ = i1;
				}
			}
		}
		memcpy( &mIndices[0], mMeshDesc.triangles.data, sizeof(physx::PxU32)*mMeshDesc.triangles.count * 3);

		//check if the criteria are met
		PX_ASSERT(mMeshDesc.isValid() );
		if( !mCooking->cookClothFabric( mMeshDesc, mGravity, wb) )
			cout << "Error cooking the cloth fabric!";
		PxToolkit::MemoryInputData rb( wb.getData(), wb.getSize() );
		mFabric = mPhysX->createClothFabric( rb );
		physx::PxTransform tr;
		tr.p = physx::PxVec3(0,10,0);
		tr.q = physx::PxQuat::createIdentity();
		mPoints = (physx::PxClothParticle*)malloc( sizeof(physx::PxClothParticle)* mMeshDesc.points.count );
		p = (physx::PxVec3*)mMeshDesc.points.data;
		for( size_t i = 0; i < mMeshDesc.points.count; ++i )
		{
			mPoints[i].pos = *p;
			//Top corner points
			if( i == 0 || i == numX )
				mPoints[i].invWeight = 0;
			else
				mPoints[i].invWeight = 1.f;
			++p;
		}
		cd.setToDefault();
		physx::PxClothCollisionSphere	mCollider[2]= {
			{PxVec3(-1.0f, 0.0f, 0.0f), 0.5f},
			{PxVec3( 1.0f, 0.0f, 0.0f), 0.25f}};
		physx::PxU32 capsulePairs[] = {0,1};
		cd.spheres = mCollider;
		cd.numSpheres = 2;
		cd.pairIndexBuffer = capsulePairs;
		cd.numPairs = 1;
		mPlane.normal = physx::PxVec3(0.f,1.f,0.f);
		mPlane.distance = 0.f;
		physx::PxU32 convMask = 1;//Convex references to the first plane only
		mCloth->addCollisionPlane( mPlane );
		mCloth->addCollisionConvex( convMask );
		mCloth->userData = this;
		mCloth->setClothFlag( physx::PxClothFlag::eSWEPT_CONTACT, true);
		mCloth = mPhysX->createCloth( pose, *mFabric, mPoints, cd, physx::PxClothFlag::eSWEPT_CONTACT);
		mCps.solverType = physx::PxClothPhaseSolverConfig::eFAST;
		mCps.stiffness = 1;
		mCps.stretchStiffness = 0.5f;

		mCloth->setPhaseSolverConfig( physx::PxClothFabricPhaseType::eBENDING,mCps );
		mCloth->setPhaseSolverConfig( physx::PxClothFabricPhaseType::eSTRETCHING, mCps );
		mCloth->setPhaseSolverConfig( physx::PxClothFabricPhaseType::eSHEARING, mCps );
		mCloth->setPhaseSolverConfig( physx::PxClothFabricPhaseType::eSTRETCHING_HORIZONTAL, mCps );
		mCloth->setDampingCoefficient( 0.125f );
		mScene->addActor( *mCloth );
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
		cout << "Starting connection with PVD ...\n";
		//check for compatybility with the current system
		if( mPhysX->getPvdConnectionManager() == NULL )
		{
			cout << "Connection failed, could not establish manager!\n";
			return false;
		}
		//connection parameters
		pvd_host_ip = "127.0.0.1";	//address IP with PVD running
		port		= 5425;			//TCP port to connect to,where PVD is listening
		timeOut		= 100;			//timeout in milliseconds to wait for PVD respond
									//consoles and remote PCs need a higher timeout.
		cout << "Cration of parameters finished.\n";
		conFlags = PVD::PxVisualDebuggerExt::getAllConnectionFlags();
		cout << "Flags created.\n";
		pCon = PVD::PxVisualDebuggerExt::createConnection( mPhysX->getPvdConnectionManager(),
			pvd_host_ip,port,timeOut,conFlags);
		cout << "Creation of network connection to PVD was a success!\n";
		return true;
	}
	//Description
	//This method will create file that can be opened by PVD.
	//Returns true if creation was success
	virtual const bool StartPvdFile()
	{
		cout << "Checking compatybility.\n";
		//check for compatybility with the current system
		if( mPhysX->getPvdConnectionManager() == NULL )
		{
			cout << "Could not establish manager";
			return false;
		}
		//file parameters
		fileName = "D:/PvdCapture.pxd2";	//name of the file containing capture
		conFlags = PVD::PxVisualDebuggerExt::getAllConnectionFlags();
		//attempt to connect
		pCon = PVD::PxVisualDebuggerExt::createConnection( mPhysX->getPvdConnectionManager(),fileName, conFlags );
		cout << "saving to file should work now.\n";
		return true;
	}
	//Description
	//For now this function creates a tetragen, which combined 2 create pyramid.
	virtual physx::PxRigidDynamic* CreatePyramid(PxVec3 position)
	{
		//                      V - tip           I          II              III     
		PxVec3 verts[] = {PxVec3(0,10,0), PxVec3(10,0,0), PxVec3(-10,0,0), PxVec3(0,0,10)};
		int j = 0;
		for(int i = 0; i < 12; i += 3)
		{
			m_verts[i] = verts[j].x;
			m_verts[i + 1] = verts[j].y;
			m_verts[i + 2] = verts[j].z;
			++j;
		}
		pyrDesc.points.count = 4;
		pyrDesc.points.stride = sizeof(PxVec3);
		pyrDesc.points.data = verts;
		pyrDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
		PxToolkit::MemoryOutputStream buf;
		if( !mCooking->cookConvexMesh( pyrDesc, buf ) )
		{
			cout << "Creation of convex mesh has failed!\n";
			return false;
		}
		else
		{
			cout << "Convex shape creation - SUCCESS\n";
			PxToolkit::MemoryInputData input(buf.getData(), buf.getSize() );
			pyr = mPhysX->createConvexMesh( input );
			pyrAct = mPhysX->createRigidDynamic( physx::PxTransform( position ) );
			pyrAct->setMass( 10.f );
			pyrShape = pyrAct->createShape( physx::PxConvexMeshGeometry( pyr ), *mMaterial );// remember to give dereferenced pointer for materials ^_^
			pyrAct->userData = verts;
			mScene->addActor( *pyrAct );
			cout << "STATUS || Convex mesh = TRUE;\n";
			return pyrAct;
		}
	return false;
	}
	virtual int*  getVertices()
	{
		return m_verts;
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


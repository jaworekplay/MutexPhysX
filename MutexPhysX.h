#pragma once
//to use destruction with APEX you need to define DESTRUCTION variable

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
#include "ICustomEventReceiver.h"
#include "CPhysXNode.h"												
																	
#include <driverChoice.h>											
#include "libraries.h"												
#include <Windows.h>												
#include <iostream>													
#include <driverChoice.h>											
#include <time.h>													
#include "documentation.h"											
#include "audio.h"					



#ifdef DESTRUCTION
#include <NxApex.h>
#include "MeshImportTypes.h"

using namespace mimp;
#endif

using namespace physx;


class CIrrlichtBase
{
private:
	ICustomEventReceiver* rec;
	irr::core::dimension2d<irr::u32> size;
	irr::video::E_DRIVER_TYPE driverType;
	irr::IrrlichtDevice* device;
	irr::scene::IMesh* mesh;
	irr::scene::ISceneNode* m_rootNode;
	irr::core::vector3df m_pos;
	irr::core::vector3df m_rot;
	irr::core::vector3df m_scale;
	std::vector<int> m_vNPC;
	std::vector<int> m_vPxNPC;
	std::vector<int> m_vObj;
	std::vector<int> m_vPxObj;
	CMutex* mute;
public:
	irr::scene::ISceneManager* smgr;
	irr::video::IVideoDriver* driver;
	AudioEngine* audio;
public:
	CIrrlichtBase()
	{
		rec = new ICustomEventReceiver();
		size = irr::core::dimension2d<irr::u32>(1024,768);
		driverType = irr::video::EDT_OPENGL;//using OpenGL driver
		device = createDevice(driverType,size,16,false,false,false,rec);
		smgr = device->getSceneManager();
		driver = device->getVideoDriver();
		m_rootNode = smgr->getRootSceneNode();
		m_pos = m_rot = irr::core::vector3df(0,0,0);
		m_scale = irr::core::vector3df(1,1,1);
		/*if( rec->isKeyPressed( irr::KEY_KEY_A ) )
			mute->moveLeft();
		if( rec->isKeyPressed( irr::KEY_KEY_D ) )
			mute->moveRight();
		if( rec->isKeyPressed( irr::KEY_KEY_W ) )
			mute->moveOut();
		if( rec->isKeyPressed( irr::KEY_KEY_S ) )
			mute->moveIn();
		if( rec->isKeyPressed( irr::KEY_SPACE ) )
			mute->Jump();*/
		//this needs to wrapped so we can use it inside the class without use of the id statement
	}
	~CIrrlichtBase()
	{
		device->drop();
	}
public:
	virtual bool addNPC(){return true;}
	virtual bool addPhysXNPC(){return true;}
	virtual bool addObject(irr::io::path& fileName, char* ID, irr::core::vector3df& Position)
	{
		int id = (int)ID;
		mesh = smgr->getMesh(fileName);
		if( Position.equals( m_pos ) )
			smgr->addMeshSceneNode(mesh,m_rootNode,id,m_pos,m_rot,m_scale);
		else
			smgr->addMeshSceneNode(mesh,m_rootNode,id,Position,m_rot,m_scale);
		m_vObj.push_back(id);
		return true;
	}
	virtual bool addPhysXObject(){return true;}
	virtual bool addPlayerCharacter(){return true;}
	virtual bool render()
	{
		while(device->run())
		{
			driver->beginScene( true, true, irr::video::SColor(0,100,100,100) );
			smgr->drawAll();
			device->getGUIEnvironment()->drawAll();
			driver->endScene();
		}
		return true;
	}
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
		//--------------------SPHERE-----------------
		physx::PxRigidDynamic*			mSphereActor;
		physx::PxShape*					mSphereShape;
		/////////////////////////////////////////////

		//--------------------GENERIC ACTOR - CAN BE ANYTHING

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
		return CMutex::mSphereActor;
	}
	//Description
	//This function will create an actor
	//either static or rigid: kinematic or ragdoll
	//TO DO
	virtual physx::PxRigidDynamic* CreateActor( physx::PxVec3& position = PxVec3(0.f,100.f,0.f),
												physx::PxReal sphereRadius = physx::PxReal(2.f), 
												physx::PxVec3& velocity = physx::PxVec3(0.f, 0.f,0.f) )
	{
		mPhysX->getMaterials( &mMaterial, 1 );
		density = PxReal(150.f);
		mSphereActor = mPhysX->createRigidDynamic( physx::PxTransform( position ) );
		mSphereShape = mSphereActor->createShape( physx::PxSphereGeometry( sphereRadius ), *mMaterial );
		mSphereActor->setMass( density );
		physx::PxRigidBodyExt::updateMassAndInertia( *mSphereActor, density );
		//specifying initial linear velocity
		mSphereActor->setLinearVelocity( velocity );
		//add the actor to the scene
		mScene->addActor( *mSphereActor );
		printf("Creation of the actor was a success!\n");
		return mSphereActor;
	}
	//Description
	//This is a trial to pass the value of the scene
	//to create an actor outside the base class
	virtual physx::PxScene* Scene() const { return mScene; }

	//Description
	//This function should translate rotations from the PhysX quartternion and matrix to irrlicht vector one
	virtual void Rotate() //
	{
		PxMat33 mat = PxMat33::PxMat33( mSphereActor->getGlobalPose().q );
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

	virtual void moveLeft(){mSphereActor->addForce( PxVec3(-10.f,0.f,0.f), physx::PxForceMode::eIMPULSE );}
	virtual void moveRight(){mSphereActor->addForce( PxVec3(10.f,0.f,0.f), physx::PxForceMode::eIMPULSE );}
	virtual void Jump(){mSphereActor->addForce( PxVec3(0.f,100.f,0.f), physx::PxForceMode::eIMPULSE );}
	virtual void moveIn(){mSphereActor->addForce( PxVec3(0.f,0.f,-10.f), physx::PxForceMode::eIMPULSE );}
	virtual void moveOut(){mSphereActor->addForce( PxVec3(0.f,0.f,10.f), physx::PxForceMode::eIMPULSE );}

	virtual void joinActors(physx::PxRigidActor *actor1,physx::PxRigidDynamic* actor2) // Leo 
	{
		mJoint = physx::PxRevoluteJointCreate(*mPhysX, 
													actor1, actor2->getGlobalPose(), 
													actor2, actor2->getGlobalPose());
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
			printf("Error cooking the cloth fabric!");
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
	virtual physx::PxRigidDynamic* CreatePyramid()
	{
		static const PxVec3 verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
		pyrDesc.points.count = 5;
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
			pyrAct = mPhysX->createRigidDynamic( physx::PxTransform( PxVec3(0,15,0) ) );
			pyrShape = pyrAct->createShape( physx::PxConvexMeshGeometry( pyr ), *mMaterial );// remember to give dereferenced pointer for materials ^_^
			mScene->addActor( *pyrAct );
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
#ifdef DESTRUCTION
class APEX : protected CMutex
{
private:
	//descriptor for APEX SDK
	NxApexSDKDesc apexDesc;
	physx::apex::NxApexSDK* mAPEX;
	physx::apex::NxApexRenderDebug* deb;
	physx::PxMat44 pose;
	physx::PxErrorCallback* pxError;
	CMutex* mutex;
public:
	APEX()
	{
		pxError = &mutex->FoundationCallback()->getErrorCallback();
		apexDesc.physXSDK = CMutex::mPhysX;
		apexDesc.cooking = CMutex::mCooking;
		apexDesc.outputStream = pxError;
		apexDesc.renderResourceManager = NULL;//todo ?
		//apex custom named resource handler
		apexDesc.resourceCallback = NULL;
		//materials used by APEX
		apexDesc.wireframeMaterial = 0; //leave it to default values
		apexDesc.solidShadedMaterial = 0;//leave to defaults initialised by string with location
		mAPEX = NxCreateApexSDK( apexDesc );
		PX_ASSERT(mAPEX);
		deb = mAPEX->createApexRenderDebug(false,true);
		pose = physx::PxMat44::createIdentity();
		if( deb )
		{
			deb->beginDrawGroup(pose);
			deb->drawGrid(false,40);
			deb->endDrawGroup();
		}
	}
	//APEX needs a memory allocator and error stream. By default it uses the PhysX one
};
#endif

class CVehicle : public CMutex
{
private:
	physx::PxVehicleWheelsSimData* m_wheelSimData;
	physx::PxVehicleDriveSimData4W m_driveData;
	physx::PxVehicleChassisData m_chassisData;
public:
	CVehicle()
	{
		//required to initiate the vehicle SDK - yes vehicles have their own SDK -_-
		PxInitVehicleSDK( *CMutex::mPhysX );
	}
	~CVehicle() {PxCloseVehicleSDK();}

	virtual void createVehicle4WSimulationData( 
		const PxF32 chassisMass,
		physx::PxConvexMesh* chassisConvexMesh,
		const PxF32 wheelMass,
		physx::PxConvexMesh** wheelConvexMeshes,
		const PxVec3* wheelCentreOffsets,
		physx::PxVehicleWheelsSimData& wheelsData,
		physx::PxVehicleDriveSimData4W& driveData,
		physx::PxVehicleChassisData& chassisData )
	{
		//extract the chassis AABB dimensions from the chassis convex mesh
		const PxVec3 chassisDims;
	}
};
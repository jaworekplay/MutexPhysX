#include "MutexPhysX.h"												//////////////////////////////////////////////////////
#include "CPhysXNode.h"												//				This box is for the informations between developers ;)
#include "ICustomEventReceiver.h"									//---Check the MutexPhysX.h for new method createCloth (mute->CreateCloth())
#include <driverChoice.h>											//
#include "libraries.h"												//
#include <Windows.h>												//
#include <iostream>													//
#include <driverChoice.h>											//
#include <time.h>													//
#include "documentation.h"											//
																	///////////////////////////////////////////////////////
physx::PxFoundation* initPhysX()
{
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;
	physx::PxFoundation *mFoundation = NULL;

	printf("initializing PhysX\n");

	printf("creating Foundation\n");
	// create foundation object with default error and allocator callbacks.
	mFoundation = PxCreateFoundation(
	PX_PHYSICS_VERSION,
	gDefaultAllocatorCallback,
	gDefaultErrorCallback);
	return mFoundation;
}

//jaworekplay
//I thought it would be easier to use constant value to define number of actors in the scene
const unsigned __int16 MAX = 20; 

int main()
{
	srand ( time(NULL) );
	CMutex* mute = new CMutex( initPhysX() /*CudaManager()*/ );
	ICustomEventReceiver* rec = new ICustomEventReceiver();
	rec->startEventProcess();

	irr::core::dimension2d<irr::u32> size = irr::core::dimension2d<irr::u32>(1024,768);
	irr::video::E_DRIVER_TYPE driverType = irr::driverChoiceConsole();
	if( driverType == irr::video::EDT_COUNT )
		return 1;
	irr::IrrlichtDevice* device = irr::createDevice( driverType, size, 16,false,false,false,rec);

	irr::scene::ISceneManager* smgr = device->getSceneManager();
    irr::video::IVideoDriver* driver = device->getVideoDriver();

	//plane
	irr::scene::IMesh* groundPlane = smgr->getGeometryCreator()->createPlaneMesh( 
		irr::core::dimension2d<irr::f32>(10,10), 
		irr::core::dimension2d<irr::u32>(10,10), 
		&smgr->getVideoDriver()->getMaterial2D(), 
		irr::core::dimension2d<irr::f32>(10,10) );
	irr::scene::ISceneNode* Plane = smgr->addMeshSceneNode(groundPlane );
	Plane->setPosition( irr::core::vector3df(0.f, 0.f,15.f) );
	Plane->setRotation(irr::core::vector3df(0.f) );
	Plane->setAutomaticCulling( irr::scene::EAC_OFF );
	Plane->setMaterialTexture(0,driver->getTexture("media/rockwall.jpg") );
	Plane->setMaterialFlag(irr::video::EMF_LIGHTING, true );
	Plane->setDebugDataVisible( irr::scene::EDS_FULL );
	//jaworekplay
	//it's cloth time !!!!

	irr::scene::IMesh* clothMesh = smgr->getGeometryCreator()->createPlaneMesh(
		irr::core::dimension2d<irr::f32>(10,10), 
		irr::core::dimension2d<irr::u32>(10,10), 
		&smgr->getVideoDriver()->getMaterial2D(), 
		irr::core::dimension2d<irr::f32>(10,10) );
	irr::scene::ISceneNode* cloth = smgr->addMeshSceneNode( clothMesh,0,-1, irr::core::vector3df(0.f,20.f,0.f) );
	mute->CreateCloth();
	//------------------ALPHA !!!
	
	PxVec3 pos(PxVec3(0.f,0.f,0.f));
	CPhysXNode* physicsActor[MAX];
	for( int i = 0; i < MAX; i++ )
	{
		physicsActor[i] = new CPhysXNode( mute->CreateActor(pos) , smgr->addSphereSceneNode(2.f) ); // Physics Actor instance: PhysX actor, irrlicht Sphere Scene Node
		pos.x = rand() % 100;
		pos.z = rand() % 100;
		
	}
	irr::scene::ISceneNode* node[MAX];
	for( int i = 0; i < MAX; i++ )
	{
		node[i] = physicsActor[i]->getIrrNode();
		//node[i]->setDebugDataVisible(-1);
		node[i]->setMaterialTexture(0, driver->getTexture("/media/wall.bmp") );
		node[i]->setMaterialFlag(irr::video::EMF_LIGHTING, false );
	}
	
	/*for(int i = 1; i < 2; i++)
	{
		mute->joinActors(physicsActor[i-1]->getPhysXActor() ,physicsActor[i]->getPhysXActor());
	}*/


	//camera
	irr::scene::ICameraSceneNode* cam;
	cam = smgr->addCameraSceneNodeFPS();
	cam->setPosition( irr::core::vector3df( 0.f, 70.f, -50.f ) );
	cam->setFarValue( 10000.f );

	device->getCursorControl()->setVisible( false );
	int lastFPS = -1;
	//timer
	irr::ITimer* irrTimer;
	irrTimer = device->getTimer();
	irr::u32 timeStamp = irrTimer->getTime(), deltaTime = 0;
	irr::gui::IGUIStaticText* text = device->getGUIEnvironment()->addStaticText( L"Jaworekplay Left movement is eFORCE Right movement is eVELOCITY_CHANG nJump is eIMPULSE", irr::core::rect<irr::s32>( 10, 10, 400, 80 ),true );
	
	while( device->run() )
	{
		mute->getResults();
		//update for debugging
		mute->advance(0.05);
		//actor
		for(int i = 0; i < 500; ++i)
			physicsActor[i]->updatePos();

		driver->beginScene( true, true, irr::video::SColor(0,100,100,100) );
		if( rec->isKeyPressed( irr::KEY_PLUS ) )
		{
			//TO DO:							/
			//initiate impulse to move actor; \/
			//Before all that implement at least 100 of spheres using for loop and arrays
			// do the character controller ASAP
			// return value of the scene and create PhysX actor outside the class
			//fix rotation !
		}
		if( rec->isKeyPressed( irr::KEY_ESCAPE ) )
		{
			device->closeDevice();
			device->drop();
			return 0;
		}
		irr::s32 fps = driver->getFPS();
		if( lastFPS != fps )
		{
			wchar_t tmp[1024];
			swprintf(tmp,L"PhysX 3.2.1 and Irrlicht 1.7.3 [fps: %d] [triangles: %d], CUDA enabled",
				fps, driver->getPrimitiveCountDrawn() );
			device->setWindowCaption(tmp);
			lastFPS = fps;
		}
		if( rec->isKeyPressed( irr::KEY_KEY_A ) )
			mute->moveLeft();
		if( rec->isKeyPressed( irr::KEY_KEY_D ) )
			mute->moveRight();
		if( rec->isKeyPressed( irr::KEY_KEY_W ) )
			mute->moveOut();
		if( rec->isKeyPressed( irr::KEY_KEY_S ) )
			mute->moveIn();
		if( rec->isKeyPressed( irr::KEY_SPACE ) )
			mute->Jump();
			//to do: create separate base for irrlich and the create struct containing values from both nodes (Irricht and PhysX) create function to translate results and pass it to the AbsolutePositioning.
			//to do: the character position can be updated from the return functions of the physX i.e. mute->GetRotationFromPhysX(PxQuat)
			//to do: that reimplement the PhysXNode with up to date code version of Irrlicht and then update the absolute positioning algorithm
		smgr->drawAll();
		device->getGUIEnvironment()->drawAll();
		driver->endScene();
	}
	device->closeDevice();
	device->drop();
	return 0;
}
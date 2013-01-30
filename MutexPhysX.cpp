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
#include "audio.h"													//
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
const unsigned __int16 MAX = 10;

int main()
{
	srand ( time(NULL) );
	CMutex* mute = new CMutex( initPhysX() /*CudaManager()*/ );
	ICustomEventReceiver* rec = new ICustomEventReceiver();
	rec->startEventProcess();

	//-----------------------------PVD--------------------------
	if( mute->StartPvdNetwork() )
		std::cout << "woo hoo\n";
	////////////////////////////////////////////////////////////

	//---------------------------INITIALISATION OF IRRLICHT----------------------------
	irr::core::dimension2d<irr::u32> size = irr::core::dimension2d<irr::u32>(1024,768);
	irr::video::E_DRIVER_TYPE driverType = irr::driverChoiceConsole();
	if( driverType == irr::video::EDT_COUNT )
		return 1;
	irr::IrrlichtDevice* device = irr::createDevice( driverType, size, 16,false,false,false,rec);

	irr::scene::ISceneManager* smgr = device->getSceneManager();
    irr::video::IVideoDriver* driver = device->getVideoDriver();
	///////////////////////////////////////////////////////////////////////////////////
	//--------------------------AUDIO-------------------
	AudioEngine audio(true);//-----------------------------------this is used instead of AudioEngine audio = new AudioEngine( bool "true" or "false" );, reason why used saves time in writing too much code
	////////////////////////////////////////////////////
	//--------------------------GRENADE-----------------
	scene::IAnimatedMeshSceneNode* grenade = smgr->addAnimatedMeshSceneNode( smgr->getMesh("frac_grenade.obj"),0,-1,core::vector3df(0,20,0) );
	if( grenade )
		printf("Grenade deployed!\n");
	////////////////////////////////////////////////////
	//----------------------------Convex shape------------------
	/*CCustomNode* customPyramid = new CCustomNode(smgr->getRootSceneNode(), smgr);
	customPyramid->setPosition( core::vector3df(0,0,0));
	std::cout << typeid(customPyramid).name() << std::endl;
	physx::PxRigidDynamic* PxPyramid = mute->CreatePyramid();
	CPhysXNode* pyramid = new CPhysXNode(PxPyramid,customPyramid);*/
	////////////////////////////////////////////////////////////

	//---------------------------------------------plane
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
	//////////////////////////////////////////////////////
	//--------------------------------------------XML-----
	//jaworekplay
	//it's cloth time !!!!

	/*irr::scene::IMesh* clothMesh = smgr->getGeometryCreator()->createPlaneMesh(
		irr::core::dimension2d<irr::f32>(10,10), 
		irr::core::dimension2d<irr::u32>(10,10), 
		&smgr->getVideoDriver()->getMaterial2D(), 
		irr::core::dimension2d<irr::f32>(10,10) );
	irr::scene::ISceneNode* cloth = smgr->addMeshSceneNode( clothMesh,0,-1, irr::core::vector3df(0.f,20.f,0.f) );*/
	//mute->CreateCloth();
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
		node[i]->setMaterialTexture(0, driver->getTexture("D:/media/wall.bmp") );
		node[i]->setMaterialFlag(irr::video::EMF_LIGHTING, false );
	}
	//-------Vehicle model------
	irr::scene::IMesh* racerLSChassis = smgr->getMesh("D:/media/vehicle/Racer-LS-chassis.mesh");
	irr::scene::IMeshSceneNode* RacerChassis = smgr->addMeshSceneNode( racerLSChassis );
	////////////////////////////
	//Battlefield models for our character
	irr::scene::IMesh* bfChar = smgr->getMesh("D:/media/talib/BF3_Taliban.3ds");
	irr::scene::IMeshSceneNode* bfTalib = smgr->addMeshSceneNode( bfChar );
	if( bfTalib )
	{
		bfTalib->setMaterialFlag( irr::video::EMF_LIGHTING, false );
		/*bfTalib->setMaterialTexture( 0, driver->getTexture("D:/media/talib/ -----no textures for now :/*/ 
	}

	//----------------------------------------camera
	irr::scene::ICameraSceneNode* cam;
	cam = smgr->addCameraSceneNodeFPS();
	cam->setPosition( irr::core::vector3df( 0.f, 70.f, -50.f ) );
	cam->setFarValue( 10000.f );
	////////////////////////////////////////////////
	device->getCursorControl()->setVisible( false );
	int lastFPS = -1;
	//------------------------------------------timer
	irr::ITimer* irrTimer;
	irrTimer = device->getTimer();
	irr::u32 timeStamp = irrTimer->getTime(), deltaTime = 0;
	irr::gui::IGUIStaticText* text = device->getGUIEnvironment()->addStaticText( L"Jaworekplay Left movement is eFORCE Right movement is eVELOCITY_CHANG nJump is eIMPULSE", irr::core::rect<irr::s32>( 10, 10, 400, 80 ),true );
	/////////////////////////////////////////////////

	while( device->run() )
	{
		mute->getResults();
		//update for debugging
		mute->advance(0.05);
		//actor
		for(int i = 0; i < MAX; ++i)
			physicsActor[i]->updatePos();
		//pyramid->updatePos();

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
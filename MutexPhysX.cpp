#include "MutexPhysX.h"	
#include "ICustomEventReceiver.h"
#include "IrrlichtBase.h"
								
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

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main()
{
	srand ( time(NULL) );
	CMutex* mute = new CMutex( initPhysX() );
	mute->StartPvdNetwork();
	//-----------------------------Event Receiver---------------
		ICustomEventReceiver* rec = new ICustomEventReceiver();
	//---------------------------INITIALISATION OF IRRLICHT----------------------------
		CIrrlichtBase* irrl = new CIrrlichtBase(*mute, *rec);
	//---------------------------The new way of including actors
	//--------------------------GRENADE-----------------
	/*scene::IAnimatedMeshSceneNode* grenade = irrl->smgr->addAnimatedMeshSceneNode( irrl->smgr->getMesh("frac_grenade.obj"),0,-1,core::vector3df(0,20,0) );
	if( grenade )
		printf("Grenade deployed!\n");*/
	////////////////////////////////////////////////////
	//----------------------------Convex shape------------------
	/*CCustomNode* customPyramid = new CCustomNode(smgr->getRootSceneNode(), smgr);
	customPyramid->setPosition( core::vector3df(0,0,0));
	std::cout << typeid(customPyramid).name() << std::endl;
	physx::PxRigidDynamic* PxPyramid = mute->CreatePyramid();
	CPhysXNode* pyramid = new CPhysXNode(PxPyramid,customPyramid);*/
	////////////////////////////////////////////////////////////

	//---------------------------------------------plane
	//irr::scene::IMesh* groundPlane = irrl->smgr->getGeometryCreator()->createPlaneMesh( 
	//	irr::core::dimension2d<irr::f32>(10,10), 
	//	irr::core::dimension2d<irr::u32>(10,10), 
	//	&irrl->smgr->getVideoDriver()->getMaterial2D(), 
	//	irr::core::dimension2d<irr::f32>(10,10) );
	//irr::scene::ISceneNode* Plane = irrl->smgr->addMeshSceneNode(groundPlane);
	//Plane->setPosition( irr::core::vector3df(0.f, 0.f,15.f) );
	//Plane->setRotation(irr::core::vector3df(0.f) );
	////Plane->setAutomaticCulling( irr::scene::EAC_OFF );
	//Plane->setMaterialTexture(0,irrl->driver->getTexture("media/rockwall.jpg") );
	//Plane->setMaterialFlag(irr::video::EMF_LIGHTING, false );
	//Plane->setDebugDataVisible( irr::scene::EDS_SKELETON );
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
	//-------Vehicle model------
	
	////////////////////////////
	//Battlefield models for our character
	//irr::scene::IMesh* bfChar = irrl->smgr->getMesh("D:/media/talib/BF3_Taliban.3ds");
	//irr::scene::IMeshSceneNode* bfTalib = irrl->smgr->addMeshSceneNode( bfChar );
	//if( bfTalib )
	//{
	//	bfTalib->setMaterialFlag( irr::video::EMF_LIGHTING, false );
	//	/*bfTalib->setMaterialTexture( 0, driver->getTexture("D:/media/talib/ -----no textures for now :/*/ 
	//}

	//----------------------------------------camera
	////////////////////////////////////////////////
		
		//pyramid->updatePos();
	irrl->render();
		
			//to do: create separate base for irrlich and the create struct containing values from both nodes (Irricht and PhysX) create function to translate results and pass it to the AbsolutePositioning.
			//to do: the character position can be updated from the return functions of the physX i.e. mute->GetRotationFromPhysX(PxQuat)
			//to do: that reimplement the PhysXNode with up to date code version of Irrlicht and then update the absolute positioning algorithm
	return 0;
}
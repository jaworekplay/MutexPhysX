#include "ICustomEventReceiver.h"
#include "IrrlichtBase.h"
#include "../PhysXBase/PhysXBase.h"
								
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
	irrl->render();
		
			//to do: create separate base for irrlich and the create struct containing values from both nodes (Irricht and PhysX) create function to translate results and pass it to the AbsolutePositioning.
			//to do: the character position can be updated from the return functions of the physX i.e. mute->GetRotationFromPhysX(PxQuat)
			//to do: that reimplement the PhysXNode with up to date code version of Irrlicht and then update the absolute positioning algorithm
	return 0;
}
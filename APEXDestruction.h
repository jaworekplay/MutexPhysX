#pragma once
#include "MutexPhysX.h"
#include "MeshImportTypes.h"

#include <NxApex.h>

using namespace mimp;


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
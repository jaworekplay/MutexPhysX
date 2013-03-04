#pragma once
#include <irrlicht.h>
#include "MutexPhysX.h"
#include "CCustomNode.h"

using namespace irr;
using namespace physx;

class CPhysXNode
{
private:
	//------------------PHYSX STUFF-----------
	physx::PxRigidDynamic*	pxActor;
	physx::PxTransform		pose;
	//------------------IRRLICHT STUFF--------
	irr::scene::ISceneNode* irrActor;
	irr::core::vector3df	pos;
	//------------------CUSTOM NODE-----------
	CCustomNode* irrCustom;
public:
	/*Constructor*/
	CPhysXNode( physx::PxRigidDynamic* physicsActor , scene::ISceneManager* smgr)
	{
		int shape;
		pxActor = physicsActor;
		shape = (int)pxActor->userData;
		switch( shape )
		{
		case eAC_Sphere:
			irrActor = smgr->addSphereSceneNode();
			break;
		case eAC_Box:
			irrActor = smgr->addCubeSceneNode(5.f);
			break;
		case eAC_Capsule:
			break;
		case eAC_Convex:
			irrCustom = new CCustomNode(smgr->getRootSceneNode(),smgr,4,-1);
			break;
			//leo
		case eAC_Face:
			irrActor = smgr->addBillboardSceneNode();
			break;
		}
	}
	/*For now we won't use the CCustomNode as it is still unstable to use !*/
	CPhysXNode( physx::PxRigidDynamic* physicsActor, CCustomNode* customNode )
	{
		pxActor = physicsActor;
		irrCustom = customNode;
		irrActor = NULL;
	}
	//Destructor
	~CPhysXNode(){}
	//Description
	//After creation call this function to
	//update positions of the objects
	//Function has to be called in the main game loop!
	virtual void updatePos()
	{
		pose = pxActor->getGlobalPose();
		pos.X = pose.p.x;
		pos.Y = pose.p.y;
		pos.Z = pose.p.z;
		/*if( typeid( irrActor ).name() == "class irr::scene::ISceneNode *" )*/
		irrActor->setPosition(pos);
		/*else
			irrCustom->setPosition(pos);
*/
		PxMat33 mat = PxMat33::PxMat33( pxActor->getGlobalPose().q ); //this code is from google code thing, because we use Rigid Dynamic already we don't need to static cast ;)
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
		/*if( typeid( irrActor ).name() == "class irr::scene::ISceneNode *" )*/
			irrActor->setRotation( irrM.getRotationDegrees() );
		/*else
			irrCustom->setRotation( irrM.getRotationDegrees() );*/
	}
	irr::scene::ISceneNode* getIrrNode() const { return irrActor; }
	physx::PxRigidDynamic* getPhysXActor() const { return pxActor; }
	CCustomNode* getCustomNode() const { return irrCustom; }
};
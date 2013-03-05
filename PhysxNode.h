#pragma once
#include <irrlicht.h>
#include <PxPhysicsAPI.h>
#include "MutexPhysX.h"

class PhysxNode
{
private:
	physx::PxRigidActor* pxActor;
	irr::scene::ISceneNode* irrActor;
public:
	PhysxNode(irr::scene::ISceneManager* smgr, physx::PxRigidActor* pxGlobalActor)
	{
		irrActor = smgr->addSphereSceneNode();
		pxActor = pxGlobalActor;
		physx::PxTransform pose;
		pose = pxActor->getGlobalPose();
		irr::core::vector3df position;
		position.X = pose.p.x;
		position.Y = pose.p.x;
		position.Z = pose.p.z;
		irrActor->setPosition(position);

		irr::core::quaternion rotation;
		rotation.X = pose.q.x;
		rotation.Y = pose.q.y;
		rotation.Z = pose.q.z;
		rotation.W = pose.q.w;
		irrActor->setRotation( rotation.getMatrix().getRotationDegrees().invert() );
	}
};
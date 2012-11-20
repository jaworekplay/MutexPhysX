#pragma once
#include <ISceneNode.h>
#include <PxPhysicsAPI.h>
#include "EPhysXCharacterCollisionGroup.h"

class CPhysXCharacterControl
{
public:
	CPhysXCharacterControl( irr::scene::ISceneNode* node,
		physx::PxCapsuleController* controller,
		physx::PxVec3 gravity );
	~CPhysXCharacterControl();
	void update( irr::f32 elapsedTime );
	irr::core::vector3df getCharacterPosition();
	bool resetCharacterPosition();
	irr::scene::ISceneNode* getSceneNode() const;
	physx::PxController* getController() const;
	void jump(irr::f32 initialvelocity);
	void move(irr::core::vector3df direction);
	void setCharacterSpeed(irr::f32 speed);
	irr::f32 getCharacterSpeed();
	void setAutoStepping(irr::f32 stepOffset);
	irr::f32 getAutoStepping();
	void setEyeHeight(irr::core::vector3df eye);
	irr::core::vector3df getEyeHeight();
private:
	physx::PxCapsuleController* Controller;
	irr::scene::ISceneNode* SceneNode;
	physx::PxExtendedVec3 StartPosition;
	void startJump( irr::f32 v0 );
	void stopJump();
	physx::PxF32 getHeight( physx::PxF32 elapsedTime );
	bool m_bJump;
	physx::PxF32 V0,
		jumpTime,
		characterSpeeds;
	physx::PxVec3 gravity,
		MoveDirection;
	bool pushCharacter;
	irr::f32 stepOffset,
		slopeLimit;
	irr::core::vector3df eyeHeight;
};
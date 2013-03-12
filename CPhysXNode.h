#pragma once
#include <irrlicht.h>
#include "MutexPhysX.h"
#include "CCustomNode.h"
#define deg PxPi


using namespace irr;
using namespace physx;

class CPhysXNode:protected CMutex
{
private:
	//------------------PHYSX STUFF-----------
	physx::PxRigidDynamic*	pxActor;
	PxRigidDynamic*			cube[12];
	physx::PxTransform		pose;
	PxVec3					pos;
	PxTransform*			temp;
	//------------------IRRLICHT STUFF--------
	irr::scene::ISceneNode* irrActor;
	irr::core::vector3df	mPos;
	irr::core::vector3df	rot;
	scene::ISceneManager*   mSmgr;
	//------------------CUSTOM NODE-----------
	CCustomNode* irrCustom;
	CCustomNode* pyramid[12];
	CMutex* mute;
	bool Cube;
public:
	/*Constructor*/
	CPhysXNode( physx::PxRigidDynamic* physicsActor , scene::ISceneManager* smgr, CMutex& mutex)
	{
		int shape;
		Cube = false;
		temp = new PxTransform();
		pxActor = physicsActor;
		shape = (int)pxActor->userData;
		mute = &mutex;
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
			//irrCustom = new CCustomNode(smgr->getRootSceneNode(),smgr,4,-1);
			break;
			//leo
		case eAC_Face:
			irrActor = smgr->addBillboardSceneNode();
			break;
		}
	}
	/*For now we won't use the CCustomNode as it is still unstable to use !*/
	CPhysXNode( physx::PxRigidDynamic* physicsActor, CCustomNode* customNode, CMutex& mutex )
	{
		pxActor = physicsActor;
		irrCustom = customNode;
		Cube = false;
		mute = &mutex;
		irrActor = NULL;
	}
	CPhysXNode( CMutex& mutex, CCustomNode* pyr[12])
	{
		mute = &mutex;
		for( int i = 0; i < 12; ++i)
		{
			pyramid[i] = pyr[i]; 
		}
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
		mPos.X = pose.p.x;
		mPos.Y = pose.p.y;
		mPos.Z = pose.p.z;
		irrActor->setPosition(mPos);
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

		irrActor->setRotation( irrM.getRotationDegrees() );
	}
	virtual bool updatePosCustomNode()
	{
		PxMat33* mat ;//= new PxMat33( pxActor->getGlobalPose().q ); //this code is from google code thing, because we use Rigid Dynamic already we don't need to static cast ;)
		irr::core::matrix4 irrM;
		irr::f32 fM[16];
		if( Cube )
		{
			for( int i = 0; i < 12; ++i)
			{
				pose = cube[i]->getGlobalPose();
				mPos.X = pose.p.x;
				mPos.Y = pose.p.x;
				mPos.Z = pose.p.z;
				pyramid[i]->setPosition(mPos);
				
				mat = new PxMat33( cube[i]->getGlobalPose().q );

				fM[0] = mat->column0.x;
				fM[1] = mat->column0.y;
				fM[2] = mat->column0.z;
				fM[4] = mat->column1.x;
				fM[5] = mat->column1.y;
				fM[6] = mat->column1.z;
				fM[8] = mat->column2.x;
				fM[9] = mat->column2.y;
				fM[10] = mat->column2.z;

				irrM.setM( fM );

				pyramid[i]->setRotation( irrM.getRotationDegrees() );
			}
		}
		delete mat;
		return true;
	}
	virtual bool createBlock(irr::core::vector3df& position, irr::core::vector3df& rotation)
	{
		Cube = true;
				//--------------Base
				rot.Y = deg/4;//45 degrees
				cube[0] = mute->CreatePyramid(pos,rot);
				rot.Y = deg + deg/4;//135
				cube[1] = mute->CreatePyramid(pos,rot);

				//---------------Top
				pos.y = 14;//rise the level
				rot.Y = 0;//nil the Y axis rotation
				rot.Z = deg;//indicate which axis to rotate

				cube[2] = mute->CreatePyramid(pos,rot);
				temp = &cube[2]->getGlobalPose();//get the pose

				rot.Z = 0;//nil the axis that we have rotated already
				rot.Y = deg + deg/2 + deg/4;//indicate which one now and what values

				temp->q *= mute->QuatFromAxisAngle(rot,rot.Y);//calculate
				cube[2]->setGlobalPose(*temp);//set new pose

				rot.Y = 0;//nil the axis
				rot.X = deg;//indicate which axis now to rotate
				rot.Z = 0;//nil the axis

				cube[3] = mute->CreatePyramid(pos,rot);//initial rotation

				rot.X = 0;//nil the axis
				rot.Y = deg + deg/2 + deg/4;//indicate

				temp->q = cube[3]->getGlobalPose().q;
				temp->q *= mute->QuatFromAxisAngle(rot,rot.Y);
				cube[3]->setGlobalPose(*temp);

				//----------------------Right
				rot.Y = 0;//nil last used axis
				pos.y = 7;//height, now we need to move it to the right and up
				pos.x = 7;
				rot.Z = -deg/2;

				cube[4] = mute->CreatePyramid(pos,rot);

				rot.Z = 0;//we don't need z axis now
				rot.X = 0;//45 degrees on x axis
				rot.Y = deg/4;

				temp = &cube[4]->getGlobalPose();
				temp->q *= mute->QuatFromAxisAngle(rot,rot.Y);
				cube[4]->setGlobalPose(*temp);

				rot.Y = 0;//nil the unused axis;
				rot.Z = -deg/2;

				cube[5] = mute->CreatePyramid(pos,rot);

				rot.Z = 0;
				rot.Y = deg + deg/4;

				temp = &cube[5]->getGlobalPose();
				temp->q *= mute->QuatFromAxisAngle(rot,rot.Y);
				cube[5]->setGlobalPose(*temp);

				rot.Y = 0;

				//Left
				pos.x = -7;//Now we need to move it left
				rot.Z = deg/2 + deg;

				cube[6] = mute->CreatePyramid(pos,rot);

				rot.Z = 0;
				rot.Y = deg/4 + deg/4 + deg/4;

				temp = &cube[6]->getGlobalPose();
				temp->q *= mute->QuatFromAxisAngle(rot,rot.Y);
				cube[6]->setGlobalPose(*temp);

				rot.Y = 0;
				rot.Z = deg/2 + deg;

				cube[7] = mute->CreatePyramid(pos,rot);

				rot.Z = 0;
				rot.Y = deg + deg/4 + deg/4 + deg/4;

				temp = &cube[7]->getGlobalPose();
				temp->q *= mute->QuatFromAxisAngle(rot,rot.Y);
				cube[7]->setGlobalPose(*temp);

				rot.Y = 0;
				//Front
				pos.x = 0;
				pos.z = 7;
				rot.X = deg/2;

				cube[8] = mute->CreatePyramid(pos,rot);

				rot.X = 0;
				rot.Z = deg ;

				temp = &cube[8]->getGlobalPose();
				temp->q *= mute->QuatFromAxisAngle(rot,rot.Z);
				cube[8]->setGlobalPose(*temp);

				rot.Z = 0;
				rot.Y = deg/4;

				temp->q *= mute->QuatFromAxisAngle(rot,rot.Y);
				cube[8]->setGlobalPose(*temp);

				rot.Y = 0;
				rot.X = deg/2;

				cube[9] = mute->CreatePyramid(pos,rot);

				rot.X = 0;
				rot.Z = deg;

				temp = &cube[9]->getGlobalPose();
				temp->q *= mute->QuatFromAxisAngle(rot,rot.Z);
				cube[9]->setGlobalPose(*temp);

				rot.Z = 0;
				rot.Y = deg + deg/4;

				temp->q *= mute->QuatFromAxisAngle(rot,rot.Y);
				cube[9]->setGlobalPose(*temp);

				rot.Y = 0;
				//Back
				pos.z = -7;
				rot.Z = deg/2;

				cube[10] = mute->CreatePyramid(pos,rot);

				rot.Z = 0;
				rot.X = deg/2;

				temp = &cube[10]->getGlobalPose();
				temp->q *= mute->QuatFromAxisAngle(rot, rot.X );
				cube[10]->setGlobalPose(*temp);

				rot.X = 0;
				rot.Y = deg/4;

				temp->q *= mute->QuatFromAxisAngle(rot,rot.Y);
				cube[10]->setGlobalPose(*temp);

				rot.Y = 0;
				rot.X = deg/2;

				cube[11] = mute->CreatePyramid(pos,rot);

				rot.X = 0;
				rot.Y = deg + deg/2 + deg/4;

				temp = &cube[11]->getGlobalPose();
				temp->q *= mute->QuatFromAxisAngle(rot,rot.Y);
				cube[11]->setGlobalPose(*temp);

				rot.Y = 0;
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				//delete temp;
		//------------IRRLICHT PART----------------------------
		
		return true;
	}
	irr::scene::ISceneNode* getIrrNode() const { return irrActor; }
	physx::PxRigidDynamic* getPhysXActor() const { return pxActor; }
	CCustomNode* getCustomNode() const { return irrCustom; }
};
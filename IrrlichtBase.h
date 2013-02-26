#pragma once
#include "MutexPhysX.h"
#include "CPhysXNode.h"
#include "CCustomNode.h"

using namespace irr;
using namespace std;
using namespace physx;

class CIrrlichtBase
{
private:
	static const unsigned __int16 MAX = 11U; // 11 elements for the body of the character
	ICustomEventReceiver* rec;
	irr::core::dimension2d<irr::u32> size;
	irr::video::E_DRIVER_TYPE driverType;
	irr::IrrlichtDevice* device;
	irr::scene::IMesh* mesh;
	irr::scene::ISceneNode* m_rootNode;
	irr::scene::ISceneNode* temp;
	irr::core::vector3df m_pos;
	irr::core::vector3df m_rot;
	irr::core::vector3df m_scale;
	std::vector<irr::scene::ISceneNode*> m_vNPC;
	std::vector<physx::PxActor*> m_vPxNPC;
	std::vector<irr::scene::ISceneNode*> m_vObj;
	std::vector<physx::PxRigidStatic*> m_vPxObj;
	CMutex* mute;
	CPhysXNode* physicsActor[MAX];
	enum eBodyParts
	{
		eHEAD = 0,
		eNECK,
		eL_ARM,
		eL_FORE_ARM,
		eR_ARM,
		eR_FORE_ARM,
		eTORSO,
		eL_UPPER_LEG,
		eL_LOWER_LEG,
		eR_UPPER_LEG,
		eR_LOWER_LEG
	};
public:
	irr::scene::ISceneManager* smgr;
	irr::video::IVideoDriver* driver;
	AudioEngine* audio;
public:
	CPhysXNode* m_pxNode;
public:
	CIrrlichtBase(CMutex& mutex, ICustomEventReceiver& passRec):rec(&passRec),mute(&mutex)
	{
		size = irr::core::dimension2d<irr::u32>(1024,768);
		driverType = irr::video::EDT_OPENGL;//using OpenGL driver
		device = createDevice(driverType,size,16,false,false,false,rec);
		smgr = device->getSceneManager();
		driver = device->getVideoDriver();
		m_rootNode = smgr->getRootSceneNode();
		m_pos = m_rot = irr::core::vector3df(0,0,0);
		m_scale = irr::core::vector3df(1,1,1);
		rec->startEventProcess();
		addSpheres();
		addPhysXObject();
		addPyramid();
		addChassis();
		addCamera();
		//this needs to wrapped so we can use it inside the class without use of the id statement
	}
	~CIrrlichtBase()
	{
		rec->endEventProcess();
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
			temp = smgr->addMeshSceneNode(mesh,m_rootNode,id,m_pos,m_rot,m_scale);
		else
			smgr->addMeshSceneNode(mesh,m_rootNode,id,Position,m_rot,m_scale);
		m_vObj.push_back(temp);
		return true;
	}
	virtual bool addSpheres()
	{
		PxVec3 pos(PxVec3(0.f,0.f,0.f));
		
		for( int i = 0; i < MAX; i++ )
		{
			physicsActor[i] = new CPhysXNode( mute->CreateActor(pos,2.f) , smgr->addSphereSceneNode(2.f) ); // Physics Actor instance: PhysX actor, irrlicht Sphere Scene Node
			physicsActor[i]->getIrrNode()->setMaterialFlag( irr::video::EMF_LIGHTING, false);
			physicsActor[i]->getIrrNode()->setMaterialTexture(0, driver->getTexture("D:/media/wall.bmp") );
			pos.x = rand() % 100;
			pos.z = rand() % 100;
		}
		//this code below, when removed gained us 100% performance improvement
		/*irr::scene::ISceneNode* node[MAX];
		for( int i = 0; i < MAX; i++ )
		{
			node[i] = physicsActor[i]->getIrrNode();
			node[i]->setMaterialTexture(0, driver->getTexture("D:/media/wall.bmp") );
			node[i]->setMaterialFlag(irr::video::EMF_LIGHTING, false );
		}*/
		pos = PxVec3(0,0,0);
		return false;
	}
	virtual bool addChassis()
	{
		irr::scene::IMesh* racerLSChassis = smgr->getMesh("D:/media/vehicle/Racer-LS-chassis.mesh");
		irr::scene::IMeshSceneNode* RacerChassis = smgr->addMeshSceneNode( racerLSChassis );
		RacerChassis->setMaterialFlag( irr::video::EMF_LIGHTING, false);
		RacerChassis->setPosition( irr::core::vector3df(0.f,50.f,0.f));
		return true;
	}
	virtual bool addPyramid()
	{
		CCustomNode* pyramid = new CCustomNode(smgr->getRootSceneNode(), smgr,4,-1);
		pyramid->setDebugDataVisible( scene::E_DEBUG_SCENE_TYPE::EDS_FULL );
		return true;
	}
	virtual bool addCamera()
	{
		irr::scene::ICameraSceneNode* cam;
		cam = smgr->addCameraSceneNodeFPS();
		cam->setPosition( irr::core::vector3df( 0.f, 70.f, -50.f ) );
		cam->setFarValue( 10000.f );
		return true;
	}
	virtual bool addPhysXObject()
	{
		mute->CreatePyramid();
		return true;
	}
	virtual bool addPlayerCharacter()
	{
		//let's start with head
		physicsActor[eHEAD] = new CPhysXNode(mute->CreateActor(),smgr->addSphereSceneNode(5.f));
		return true;
	}
	virtual bool render()
	{
		
		while(device->run())
		{
			mute->advance(0.005f);
			driver->beginScene( true, true, irr::video::SColor(0,100,100,100) );
			smgr->drawAll();
			device->getGUIEnvironment()->drawAll();
			driver->endScene();

			if( rec->isKeyDown( irr::KEY_KEY_A ) )
				mute->moveLeft();
			if( rec->isKeyDown( irr::KEY_KEY_D ) )
				mute->moveRight();
			if( rec->isKeyDown( irr::KEY_KEY_W ) )
				mute->moveOut();
			if( rec->isKeyDown( irr::KEY_KEY_S ) )
				mute->moveIn();
			if( rec->isKeyDown( irr::KEY_SPACE ) )
				mute->Jump();
			if( rec->isKeyDown( irr::KEY_ESCAPE ) )
			{
				device->drop();
				return true;
			}
			for(int i = 0; i < MAX; ++i)
				physicsActor[i]->updatePos();
		}
		return true;
	}
};
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
	static const unsigned int SPH = 2;
	ICustomEventReceiver* rec;
	irr::core::dimension2d<irr::u32> size;
	irr::video::E_DRIVER_TYPE driverType;
	irr::IrrlichtDevice* device;
	irr::scene::ISceneNode* m_rootNode;
	irr::scene::ISceneNode* temp;
	irr::scene::ISceneCollisionManager* cmgr;
	irr::core::vector3df m_pos;
	irr::core::vector3df m_rot;
	irr::core::vector3df m_scale;
	std::vector<irr::scene::ISceneNode*> m_vNPC;
	std::vector<physx::PxActor*> m_vPxNPC;
	std::vector<irr::scene::ISceneNode*> m_vObj;
	std::vector<physx::PxRigidStatic*> m_vPxObj;
private:
	CMutex* mute;
	CPhysXNode* physicsActor[MAX];
	CPhysXNode* m_Spheres[SPH];
	CPhysXNode* m_Walls[4];
private:
	io::IFileSystem* files;
private:
	irr::scene::ICameraSceneNode* cam;
	PxVec3 CamPosPhysX;
	scene::IVolumeLightSceneNode* m_light;
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
		cmgr = smgr->getSceneCollisionManager();
		rec->startEventProcess();
		
		files = device->getFileSystem();
		files->addFileArchive("D:\\media\\");

		addScene();
		addObject(100, vector3df(0,10.f,10.f));
		addPhysXObject();
		addPyramid();
		addChassis();
		addSpheres();
		addCamera();
		addPyramid(vector3df(0,40,0), vector3df(0,0,180));
		//this needs to wrapped so we can use it inside the class without use of the id statement
	}
	~CIrrlichtBase()
	{
		rec->endEventProcess();
		device->drop();
	}
public:
	virtual bool addScene()
	{
		smgr->loadScene("Egypt V6.irr");
		m_light = smgr->addVolumeLightSceneNode();
		m_light->setScale(core::vector3df(46.f,45.f,46.f));
		m_light->getMaterial(0).setTexture(0, driver->getTexture("lightFalloff.png"));
		m_light->setPosition( vector3df(0,1000,0) );
		return true;
	}
	virtual bool addNPC(){return true;}
	virtual bool addPhysXNPC(){return true;}
	virtual bool addObject(int ID, irr::core::vector3df& Position)
	{
		scene::IAnimatedMesh* mesh = smgr->getMesh("Pyramid.DAE");
		scene::IAnimatedMeshSceneNode* pyr = smgr->addAnimatedMeshSceneNode( mesh );
		return true;
	}
	virtual bool addSpheres()
	{
		PxVec3 pos(PxVec3(0.f,0.f,0.f));
		
		for( int i = 0; i < SPH; ++i )
		{
			m_Spheres[i] = new CPhysXNode( mute->CreateActor(eAC_Sphere,pos),smgr );
			m_Spheres[i]->getIrrNode()->setMaterialFlag( irr::video::EMF_LIGHTING, false);
			m_Spheres[i]->getIrrNode()->setMaterialTexture(0, driver->getTexture("wall.bmp") );
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
		irr::scene::IMesh* racerLSChassis = smgr->getMesh("Racer-LS-chassis.mesh");
		irr::scene::IMeshSceneNode* RacerChassis = smgr->addMeshSceneNode( racerLSChassis );
		RacerChassis->setMaterialFlag( irr::video::EMF_LIGHTING, false);
		RacerChassis->setPosition( irr::core::vector3df(0.f,50.f,0.f));
		RacerChassis->setScale(vector3df(10.f));
		return true;
	}
	//Description
	//If you want to add pyramid just call this method
	//Parameter 1 = position
	//Parameter 2 = rotation
	//When placed in a smart way can create wall or a column
	//JaworekP1ay ^_^
	virtual bool addPyramid(vector3df& position = vector3df(0), vector3df& rotation = vector3df(0))
	{
		CCustomNode* pyramid = new CCustomNode(smgr->getRootSceneNode(), smgr,4,-1);
		pyramid->setDebugDataVisible( scene::E_DEBUG_SCENE_TYPE::EDS_FULL );
		pyramid->setMaterialFlag(video::EMF_LIGHTING, false);
		pyramid->setPosition( position );
		pyramid->setRotation(rotation );
		return true;
	}
	virtual bool addCamera()
	{
		cam = smgr->addCameraSceneNodeFPS(smgr->getRootSceneNode(), 100.f, 0.2f);
		cam->setPosition( irr::core::vector3df( 0.f, 70.f, -50.f ) );
		cam->setFarValue( 10000.f );
		//cam->setTarget(m_Spheres[SPH - 1]->getIrrNode()->getPosition());
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
		physicsActor[eHEAD] = new CPhysXNode(mute->CreateActor(eAC_Sphere, PxVec3(0)), smgr);
		return true;
	}
	virtual bool render()
	{
		
		while(device->run())
		{
			mute->advance(0.5f);
			driver->beginScene( true, true, irr::video::SColor(0,100,100,100) );
			smgr->drawAll();
			device->getGUIEnvironment()->drawAll();
			driver->endScene();
			//cam->setTarget( m_Spheres[SPH - 1]->getIrrNode()->getPosition() );
			if( rec->isKeyDown( irr::KEY_KEY_A ) )
				mute->moveLeft();
			if( rec->isKeyDown( irr::KEY_KEY_D ) )
				mute->moveRight();
			if( rec->isKeyDown( irr::KEY_KEY_W ) )
				mute->moveForward();
			if( rec->isKeyDown( irr::KEY_KEY_S ) )
				mute->moveIn();
			if( rec->isKeyDown( irr::KEY_SPACE ) )
				mute->Jump();
			if( rec->isKeyDown( irr::KEY_ESCAPE ) )
			{
				device->drop();
				return true;
			}
			vector3df newPos;
			/*newPos.X = m_Spheres[SPH - 1]->getIrrNode()->getPosition().X;
			newPos.Y = m_Spheres[SPH - 1]->getIrrNode()->getPosition().Y + 50.f;
			newPos.Z = m_Spheres[SPH - 1]->getIrrNode()->getPosition().Z - 50.f;
			cam->setPosition(newPos);*/
			for(int i = 0; i < SPH; ++i)
				m_Spheres[i]->updatePos();
		}
		return true;
	}
};
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
	static const unsigned int SPH = 20;//
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
	CCustomNode* pyramid[12];
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
		files->addFileArchive("D:\\media");
		files->addFolderFileArchive("D:\\media");
		for( int i = 0; i < SPH; ++i )
			m_Spheres[i] = NULL;
		//addScene();
		//createWall(1,8);
		/////////////////////////////////////////////////
		//addChassis();
		//addSpheres();
		addBlock();
		addPhysXObject(vector3df(0));
		addPhysXObject(vector3df(15,0,0),pyramid[1]->getRotation() );
		addCamera();
		
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
		smgr->loadScene("D:\\media\\Egypt V6.irr");
		m_light = smgr->addVolumeLightSceneNode();
		m_light->setScale(core::vector3df(46.f,45.f,46.f));
		m_light->getMaterial(0).setTexture(0, driver->getTexture("D:\\media\\lightFalloff.png"));
		m_light->setPosition( vector3df(0,1000,0) );
		return true;
	}
	virtual bool addNPC(){return true;}
	virtual bool addPhysXNPC(){return true;}
	virtual bool addObject(int ID, irr::core::vector3df& Position)
	{
		scene::IAnimatedMesh* mesh = smgr->getMesh("D:\\media\\Pyramid.DAE");
		scene::IAnimatedMeshSceneNode* pyr = smgr->addAnimatedMeshSceneNode( mesh );
		return true;
	}
	virtual bool addSpheres()
	{
		PxVec3 pos(PxVec3(0.f,0.f,0.f));
		
		for( int i = 0; i < SPH; ++i )
		{
			m_Spheres[i] = new CPhysXNode( mute->CreateActor(E_ACTOR_CREATION::eAC_Box,pos),smgr );
			m_Spheres[i]->getIrrNode()->setMaterialFlag( irr::video::EMF_LIGHTING, true);
			m_Spheres[i]->getIrrNode()->setMaterialTexture(0, driver->getTexture("media\\wall.bmp") );
			pos.x = rand() % 200;
			pos.z = rand() % 200;
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
	virtual bool addBlock(vector3df& position = vector3df(0), vector3df& rotation = vector3df(0))
	{
		//----------------------------BASE----------------------------------------
		pyramid[0] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[0]->setPosition(			position );
		pyramid[0]->setRotation(			rotation + vector3df(0,45,0));
		

		//vector3df(0,180,0)
		pyramid[1] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[1]->setPosition(			position + vector3df(0,0,0));
		pyramid[1]->setRotation(			vector3df(0,225,0));
		//////////////////////////////////////////////////////////////////////////



		//----------------------------TOP-----------------------------------------
		pyramid[2] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[2]->setPosition(			position + vector3df(0,14,0) );
		pyramid[2]->setRotation(			vector3df(180,45,0) );

		pyramid[3] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[3]->setPosition(			position + vector3df(0,14,0) );
		pyramid[3]->setRotation(			vector3df(180,225,0));
		//////////////////////////////////////////////////////////////////////////



		//----------------------------RIGHT---------------------------------------
		pyramid[4] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[4]->setPosition(			position + vector3df(7,7,0) );
		pyramid[4]->setRotation(			vector3df(0,45,90));

		pyramid[5] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[5]->setPosition(			position + vector3df(7,7,0) );
		pyramid[5]->setRotation(			vector3df(0,225,90));
		//////////////////////////////////////////////////////////////////////////



		//----------------------------LEFT----------------------------------------
		pyramid[6] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[6]->setPosition(			position + vector3df(-7,7,0) );
		pyramid[6]->setRotation(			vector3df(180,45,90)); 

		pyramid[7] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[7]->setPosition(			position + vector3df(-7,7,0) );
		pyramid[7]->setRotation(			vector3df(180,225,90));
		//////////////////////////////////////////////////////////////////////////




		//----------------------------FRONT---------------------------------------
		pyramid[8] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[8]->setPosition(			position + vector3df(0,7,-7) );
		pyramid[8]->setRotation(			vector3df(90,0,45));

		pyramid[9] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[9]->setPosition(			position + vector3df(0,7,-7) );
		pyramid[9]->setRotation(			vector3df(90,0,225));
		//////////////////////////////////////////////////////////////////////////





		//----------------------------BACK----------------------------------------
		pyramid[10] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[10]->setPosition(			position + vector3df(0,7,7) );
		pyramid[10]->setRotation(			vector3df(-90,0,45));

		pyramid[11] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[11]->setPosition(			position + vector3df(0,7,7) );
		pyramid[11]->setRotation(			vector3df(-90,0,-135));
		//////////////////////////////////////////////////////////////////////////

		for( int i = 0; i < 12; ++i)
			pyramid[i]->setMaterialTexture( 0,driver->getTexture("D:\\media\\wall.bmp"));

		return true;
	}
	virtual bool createWall(int rows, int columns)
	{
		for( int i = 0; i < rows; ++i)
		{
			for( int j = 0; j < columns; ++j)
			{
				addBlock( vector3df(i*14,j*14,0) );
			}
		}
		return true;
	}
	virtual bool addCamera()
	{
		cam = smgr->addCameraSceneNodeFPS(smgr->getRootSceneNode(), 100.f, 0.2f);
		cam->setPosition( irr::core::vector3df( 0.f, 0.f, -100.f ) );
		cam->setTarget( vector3df(0) );
		cam->setFarValue( 10000.f );
		//cam->setTarget(m_Spheres[SPH - 1]->getIrrNode()->getPosition());
		return true;
	}
	virtual bool addPhysXObject(PxVec3 position = PxVec3(0), vector3df rotation = vector3df(0))
	{
		mute->CreatePyramid(PxVec3(0), rotation);
		return true;
	}
	virtual bool addPhysXObject(vector3df position = vector3df(0), vector3df rotation = vector3df(0))
	{
		mute->CreatePyramid(PxVec3(0), rotation);
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
		u32 frames=0;
		while(device->run())
		{
			mute->advance(2.f);
			driver->beginScene( true, true, irr::video::SColor(0,100,100,100) );
			smgr->drawAll();
			device->getGUIEnvironment()->drawAll();
			driver->endScene();
			//cam->setTarget( m_Spheres[SPH - 1]->getIrrNode()->getPosition() );
				core::stringw str = L"Irrlicht Engine [";
				str += driver->getName();
				str += L"] FPS: ";
				str += (s32)driver->getFPS();
				str += L" Objects: ";
				str += driver->getPrimitiveCountDrawn();

				device->setWindowCaption(str.c_str());
				frames=0;
			//mute->joinActors( mute->CreatePyramid(), mute->CreatePyramid() );

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
			if( rec->isKeyDown( irr::KEY_KEY_R ) )
			{
				cam->setPosition( vector3df(0.f,0.f,-100.f) );
				cam->setTarget( vector3df(0) );
			}
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
			if( m_Spheres[0] != NULL)
			{
				for(int i = 0; i < SPH; ++i)
					m_Spheres[i]->updatePos();
			}
		}
		return true;
	}
};
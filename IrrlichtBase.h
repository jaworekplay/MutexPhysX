#pragma once
#include "CPhysXNode.h"

using namespace irr;
using namespace std;
using namespace physx;

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


class CIrrlichtBase
{
private:
	static const unsigned __int16	MAX = 11U; // 11 elements for the body of the character
	static const unsigned int		SPH = 1500;//
	ICustomEventReceiver*			rec;
	irr::core::dimension2d<irr::u32> size;
	irr::video::E_DRIVER_TYPE		driverType;
	irr::IrrlichtDevice*			device;
	irr::scene::ISceneNode*			m_rootNode;
	irr::scene::ISceneNode*			temp;
	irr::scene::ISceneNode*			testWall;
	irr::scene::IAnimatedMeshSceneNode* savage;
	irr::scene::ISceneCollisionManager* cmgr;
	irr::core::vector3df			m_pos;
	irr::core::vector3df			m_rot;
	irr::core::vector3df			m_scale;
	irr::scene::ITerrainSceneNode*	m_iTerNode;
	std::vector<irr::scene::ISceneNode*> m_vNPC;
	std::vector<CPhysXNode*>			m_vCPxObj;
	std::vector<CPhysXNode*>::iterator m_vIt;
	std::vector<physx::PxActor*>	m_vPxNPC;
	std::vector<irr::scene::ISceneNode*> m_vObj;
	std::vector<physx::PxRigidStatic*> m_vPxObj;

	float fHealth;
	scene::IAnimatedMeshSceneNode* syd;
private:
	CMutex*		mute;
	CPhysXNode* physicsActor[MAX];
	CPhysXNode* startPxNode;
	CPhysXNode* m_Spheres[SPH];
	CPhysXNode* m_Walls[4];
private:
	io::IFileSystem* files;
private:
	CCustomNode* pyramid[12];
	CCustomNode* startNode;
private:
	irr::scene::ICameraSceneNode* cam;
	irr::core::matrix4 m_mProjMat;
	PxVec3 CamPosPhysX;
	physx::PxRigidDynamic* startPhysXNode;
	scene::IVolumeLightSceneNode* m_light;
public:
	irr::scene::ISceneManager* smgr;
	irr::video::IVideoDriver* driver;
	AudioEngine* audio;
public:
	CPhysXNode* m_pxNode;
	bool create;
private:
	physx::PxRigidDynamic* pxPlayer;
public:
	CIrrlichtBase(CMutex& mutex, ICustomEventReceiver& passRec):rec(&passRec),mute(&mutex),pxPlayer(0)
	{
		size = irr::core::dimension2d<irr::u32>(1024,768);
		driverType = irr::video::EDT_OPENGL;//using OpenGL driver
		device = createDevice(driverType,size,16,false,false,false,rec);
		smgr = device->getSceneManager();
		smgr->getRootSceneNode()->setPosition(vector3df(0));
		driver = device->getVideoDriver();
		m_rootNode = smgr->getRootSceneNode();
		m_pos = m_rot = irr::core::vector3df(0,0,0);
		m_scale = irr::core::vector3df(1,1,1);
		cmgr = smgr->getSceneCollisionManager();
		rec->startEventProcess();
		
		for( int i = 0; i < SPH; ++i )
			m_Spheres[i] = NULL;
		create = false;
		fHealth = 100.f;

		addCamera();
		createWall();
		//this function is to be called last
		pxPlayer = mute->createPlayerShape();
		Sleep(1);
		//this needs to wrapped so we can use it inside the class without use of the id statement
	}
	CIrrlichtBase()
	{
		size = irr::core::dimension2d<irr::u32>(1024,768);
		driverType = irr::video::EDT_OPENGL;//using OpenGL driver
		device = createDevice(driverType,size,16,false,false,false,rec);
		smgr = device->getSceneManager();
		smgr->getRootSceneNode()->setPosition(vector3df(0));
		driver = device->getVideoDriver();
		m_rootNode = smgr->getRootSceneNode();
		m_pos = m_rot = irr::core::vector3df(0,0,0);
		m_scale = irr::core::vector3df(1,1,1);
		cmgr = smgr->getSceneCollisionManager();
		rec->startEventProcess();
		
		files = device->getFileSystem();
		files->addFileArchive("D:\\media");
		files->addFolderFileArchive("D:\\media");
	}
	~CIrrlichtBase()
	{
		rec->endEventProcess();
		device->drop();
	}
public:
	virtual bool addScene()
	{
		smgr->loadScene("media\\Egypt V6.irr");
		m_light = smgr->addVolumeLightSceneNode();
		m_light->setScale(core::vector3df(46.f,45.f,46.f));
		m_light->getMaterial(0).setTexture(0, driver->getTexture("D:\\media\\lightFalloff.png"));
		m_light->setPosition( vector3df(0,1000,0) );

		core::array<scene::ISceneNode *> nodes;
		smgr->getSceneNodesFromType(scene::ESNT_ANY, nodes); // Find all nodes

		for (u32 i=0; i < nodes.size(); ++i)
		{
			scene::ISceneNode * node = nodes[i];
			scene::ITriangleSelector * selector = 0;

			switch(node->getType())
			{
			case scene::ESNT_CUBE:
			case scene::ESNT_ANIMATED_MESH:
			case scene::ESNT_MESH:
			case scene::ESNT_SPHERE: // Derived from IMeshSceneNode
			case scene::ESNT_TERRAIN:
			case scene::ESNT_OCTREE:
				node = (scene::IMeshSceneNode*)node;
				node->setMaterialFlag( video::EMF_LIGHTING, true );
				break;

			default:
				// Don't create a selector for this node type
				break;
			}
		}

		return true;
	}
	virtual bool retrieveNodesFromIrrFile()
	{
		core::array<scene::ISceneNode*> nodes;
		for (u32 i=0; i < nodes.size(); ++i)
		{
			scene::ISceneNode * node = nodes[i];
			scene::ITriangleSelector * selector = 0;

			switch(node->getType())
			{
			case scene::ESNT_CUBE:
			case scene::ESNT_ANIMATED_MESH:
				// Because the selector won't animate with the mesh,
				// and is only being used for camera collision, we'll just use an approximate
				// bounding box instead of ((scene::IAnimatedMeshSceneNode*)node)->getMesh(0)
				selector = smgr->createTriangleSelectorFromBoundingBox(node);
			break;

			case scene::ESNT_MESH:
			case scene::ESNT_SPHERE: // Derived from IMeshSceneNode
				selector = smgr->createTriangleSelector(((scene::IMeshSceneNode*)node)->getMesh(), node);
				break;

			case scene::ESNT_TERRAIN:
				m_iTerNode = ((scene::ITerrainSceneNode*)node);
				break;

			case scene::ESNT_OCTREE:
				selector = smgr->createOctreeTriangleSelector(((scene::IMeshSceneNode*)node)->getMesh(), node);
				break;

			default:
				// Don't create a selector for this node type
				break;
			}

			if(selector)
			{
				// Add it to the meta selector, which will take a reference to it
				
				// And drop my reference to it, so that the meta selector owns it.
				selector->drop();
			}
		}
		return true;
	}
	virtual bool addTerrain()
	{
		// add terrain scene node
		scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
			"media\\terrain-heightmap.bmp",
			0,                  // parent node
			-1,                 // node id
			core::vector3df(0.f, 0.f, 0.f),     // position
			core::vector3df(0.f, 0.f, 0.f),     // rotation
			core::vector3df(40.f, 4.4f, 40.f),  // scale
			video::SColor ( 255, 255, 255, 255 ),   // vertexColor
			5,                  // maxLOD
			scene::ETPS_17,             // patchSize
			4                   // smoothFactor
			);

		terrain->setMaterialFlag(video::EMF_LIGHTING, false);

		terrain->setMaterialTexture(0,
				driver->getTexture("media\\terrain-texture.jpg"));
		terrain->setMaterialTexture(1,
				driver->getTexture("media\\detailmap3.jpg"));
    
		terrain->setMaterialType(video::EMT_DETAIL_MAP);

		terrain->scaleTexture(1.0f, 20.0f);


		scene::CDynamicMeshBuffer* buffer = new scene::CDynamicMeshBuffer(video::EVT_2TCOORDS, video::EIT_16BIT);
		terrain->getMeshBufferForLOD(*buffer, 0);
		video::S3DVertex2TCoords* data = (video::S3DVertex2TCoords*)buffer->getVertexBuffer().getData();
		buffer->getVertices();
		
		// Work on data or get the IndexBuffer with a similar call.
		buffer->drop(); // When done drop the buffer again.


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
			m_Spheres[i] = new CPhysXNode( mute->CreateActor(E_ACTOR_CREATION::eAC_Box,pos),smgr,*mute );
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
	virtual bool createWall()
	{
		PxVec3 pos(PxVec3(0.f,0.f,0.f));
		int j = 0;
		pos.y += 5;
		pos.x = 150;
		for( int i = 0; i < SPH; ++i )
		{
			m_Spheres[i] = new CPhysXNode( mute->CreateActor(E_ACTOR_CREATION::eAC_Box,pos),smgr,*mute );
			m_Spheres[i]->getIrrNode()->setMaterialFlag( irr::video::EMF_LIGHTING, true);
			m_Spheres[i]->getIrrNode()->setMaterialTexture(0, driver->getTexture("media\\wall.bmp") );

			if( j < 30 )
			{
				pos.z -= 5;
				++j;
			}
			else
			{
				j = 0;
				pos.y += 5;
				pos.z = 0;
			}
		}
		return true;
	}

	virtual bool PhysXWall()
	{
		this->addBlock();
		this->m_pxNode = new CPhysXNode(*mute, pyramid); 
		this->m_pxNode->createBlock(vector3df(0,0,0),vector3df(0));
		this->m_pxNode->createBlock(vector3df(0,15,0),vector3df(0));
		this->m_pxNode->createBlock(vector3df(0,25,0),vector3df(0));
		this->m_pxNode->createBlock(vector3df(0,35,0),vector3df(0));
		this->m_pxNode->createBlock(vector3df(0,45,0),vector3df(0));
		this->m_pxNode->createBlock(vector3df(0,55,0),vector3df(0));
		this->m_pxNode->createBlock(vector3df(0,65,0),vector3df(0));
		return true;
	}

	virtual bool addCamera()
	{
		scene::IMesh* mesh = smgr->getMesh("media\\testWall.obj");
		scene::IMeshSceneNode* test = smgr->addMeshSceneNode(mesh);
		test->setScale( core::vector3df(10.f) );
		test->setMaterialTexture(0, driver->getTexture("media\\diffuse.jpg") );
		test->setMaterialFlag( irr::video::EMF_LIGHTING, false );
		test->setPosition( core::vector3df(400,0,0) );
		test->setRotation( core::vector3df(0.f,270.f,0.f) );

		scene::IAnimatedMesh* nme = smgr->getMesh( "media\\ninja.b3d" );
		savage = smgr->addAnimatedMeshSceneNode( nme );
		savage->setMaterialFlag( video::EMF_LIGHTING, false );
		savage->setScale( vector3df(10.f) );
		savage->setPosition( vector3df( 0.f,0.f,-50.f ) );
		savage->setRotation( core::vector3df(0.f,90.f,0) );

		cam = smgr->addCameraSceneNodeFPS(0,100.f,0.2f);
		cam->setFarValue( 10000.f );

		smgr->addLightSceneNode(0, core::vector3df(0,100,0),
        video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 800.0f);

		scene::IAnimatedMesh* sydMesh = smgr->getMesh("media/faerie.md2");
		if (!sydMesh)
		{
			device->drop();
			return 1;
		}
		syd = smgr->addAnimatedMeshSceneNode( sydMesh );
		syd->setPosition( core::vector3df( savage->getPosition().X + 400.f, savage->getPosition().Y + 50.f, savage->getPosition().Z ) );
		syd->setMD2Animation( scene::EMAT_STAND );
		syd->setScale( core::vector3df(2.f) );

		scene::IParticleSystemSceneNode* ps[4];
		scene::IParticleEmitter* em[4];
		scene::IParticleAffector* paf[4];

		for( int i = 0; i < 4; ++i)
		{
			
			ps[i] = smgr->addParticleSystemSceneNode(false);
			
			em[i] = ps[i]->createBoxEmitter(
							core::aabbox3d<f32>(-7,0,-7,7,1,7), // emitter size
							core::vector3df(0.0f,0.06f,0.0f),   // initial direction
							80,100,                             // emit rate
							video::SColor(0,255,0,0),       // darkest color
							video::SColor(0,255,0,0),       // brightest color
							800,2000,0,                         // min and max age, angle
							core::dimension2df(10.f,10.f),         // min size
							core::dimension2df(20.f,20.f));        // max size

			ps[i]->setEmitter(em[0]); // this grabs the emitter
			em[i]->drop(); // so we can drop it here without deleting it

			
			paf[i] = ps[i]->createFadeOutParticleAffector();

			ps[i]->addAffector(paf[i]); // same goes for the affector
			paf[i]->drop();

			ps[i]->setPosition(core::vector3df(i*360.f,60,-10));
			ps[i]->setScale(core::vector3df(2,2,2));
			ps[i]->setMaterialFlag(video::EMF_LIGHTING, false);
			ps[i]->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
			ps[i]->setMaterialTexture(0, driver->getTexture("media/fire.bmp"));
			ps[i]->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		}

		/*this->m_mProjMat.buildProjectionMatrixPerspectiveRH(0.1f,0.1f,0.2f,5000.f);
		cam->setProjectionMatrix( m_mProjMat );*/
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
		physicsActor[eHEAD] = new CPhysXNode(mute->CreateActor(eAC_Sphere, PxVec3(0,25,0)), smgr, *mute);
		physicsActor[eNECK] = new CPhysXNode(mute->CreateActor(eAC_Capsule, PxVec3(0,22,0)),smgr, *mute);
		physicsActor[eR_ARM] = new CPhysXNode(mute->CreateActor(eAC_Capsule, PxVec3(0,22,0)),smgr, *mute);
		physicsActor[eR_FORE_ARM] = new CPhysXNode(mute->CreateActor(eAC_Capsule, PxVec3(0,20,0)),smgr, *mute);
		physicsActor[eL_ARM] = new CPhysXNode( mute->CreateActor(eAC_Capsule, PxVec3(0,22,0)),smgr, *mute);
		physicsActor[eL_FORE_ARM] = new CPhysXNode( mute->CreateActor(eAC_Capsule, PxVec3(0,20,0)),smgr, *mute);
		physicsActor[eTORSO] = new CPhysXNode( mute->CreateActor( eAC_Box, PxVec3(0,22,0)),smgr, *mute);
		physicsActor[eR_UPPER_LEG] = new CPhysXNode( mute->CreateActor(eAC_Capsule, PxVec3(0,12,0)),smgr, *mute);
		physicsActor[eR_LOWER_LEG] = new CPhysXNode( mute->CreateActor(eAC_Capsule, PxVec3(0,6,0)),smgr, *mute);
		physicsActor[eL_UPPER_LEG] = new CPhysXNode( mute->CreateActor( eAC_Capsule, PxVec3(0,12,0)), smgr, *mute);
		physicsActor[eL_LOWER_LEG] = new CPhysXNode( mute->CreateActor( eAC_Capsule, PxVec3(0,6,0)), smgr, *mute);
		return true;
	}
	virtual bool render()
	{
		bool pressed = false, set = false, sydDead = false;
		u32 frames=0;
		int n = 0;
		f32 dif = 0;
		vector3df newPos,camPos, sydPos;
		sydPos = core::vector3df( savage->getPosition().X + 400.f, savage->getPosition().Y + 50.f, savage->getPosition().Z );
		while(device->run())
		{
			mute->advance(0.5f);
			driver->beginScene( true, true, irr::video::SColor(0,100,100,100) );
			smgr->drawAll();
			device->getGUIEnvironment()->drawAll();
			
			//cam->setTarget( m_Spheres[SPH - 1]->getIrrNode()->getPosition() );
				core::stringw str = L"Irrlicht Engine [";
				str += driver->getName();
				str += L"] FPS: ";
				str += (s32)driver->getFPS();
				str += L" Objects: ";
				str += driver->getPrimitiveCountDrawn();
				str += L"Health: ";
				str += fHealth;
				device->setWindowCaption(str.c_str());
				frames=0;
				
			//mute->joinActors( mute->CreatePyramid(), mute->CreatePyramid() );
			if( rec->isMouseButtonDown( EMouseButton::MButton_LEFT ) )
			{
				++n;
				if( n == 1)
				{
				m_vCPxObj.push_back( new CPhysXNode( mute->CreateActor(	E_ACTOR_CREATION::eAC_Box, 
																		PxVec3(cam->getPosition().X,
																		cam->getPosition().Y, 
																		cam->getPosition().Z),
													PxVec3(	cam->getTarget().X*0.1,
															cam->getTarget().Y*0.1,
															cam->getTarget().Z)*0.1 ),
													smgr,
													*mute ));
				this->m_vCPxObj.back()->getIrrNode()->setMaterialTexture(0, driver->getTexture("media/diffuse.jpg") );
				}
				create = true;
			}
			else
				n = 0;
			if( rec->isMouseButtonDown( EMouseButton::MButton_RIGHT ) )
			{
				mute->joinActors( this->m_vCPxObj[1]->getPhysXActor(), this->m_vCPxObj[0]->getPhysXActor());
			}
			newPos = savage->getPosition();
			dif = syd->getPosition().X - savage->getPosition().X;
			if(dif < 100 && rec->isKeyDown( irr::KEY_SPACE ) && dif > -100)
				fHealth -= 1.f;
			
			if( rec->isKeyDown( irr::KEY_KEY_D ) )
			{
				newPos.X += 2.f;
				if( !pressed )
				{
					pressed = true;
					set = false;
					savage->setFrameLoop(15,30);
				}
			}
			else if( rec->isKeyDown( irr::KEY_KEY_A ) )
			{
				newPos.X -= 2.f;
				if (!pressed)
				{
					pressed = true;
					set = false;
					savage->setFrameLoop(146,158);
				}
			}
			else if( rec->isKeyDown( irr::KEY_SPACE ) )
			{
				if (!pressed)
				{
					pressed = true;
					set = false;
					savage->setFrameLoop(45,59);
					if( sydDead )
						sydPos.Y -= 2.f;
				}
			} else
			{
				pressed = false;
				if (!set)
				{
					set = true;
					savage->setFrameLoop(206,250);
					if( fHealth < 0 && !sydDead)
					{
						syd->setMD2Animation( scene::EMAT_DEATH_FALLFORWARD );
						sydDead = true;
					}
				}
			}
			if( sydDead )
				sydPos.Y -= 2.f;
			syd->setPosition( sydPos );
			
			if( rec->isKeyDown( irr::KEY_KEY_W ) )
				;
			if( rec->isKeyDown( irr::KEY_KEY_S ) )
				;
			
			if( rec->isKeyDown( irr::KEY_KEY_R ) )
				;
			if( rec->isKeyDown( irr::KEY_ESCAPE ) )
			{
				device->drop();
				return true;
			}
			/*camPos.X = newPos.X/25;
			camPos.Y = newPos.Y + 20.f;
			camPos.Z = newPos.Z + 20.f;*/
			
			
			if( savage )
				savage->setPosition(newPos);
			mute->setPlayerPosition( physx::PxVec3( newPos.X, newPos.Y + 30.f, newPos.Z), *pxPlayer );



			newPos.Z -= 200.f;
			newPos.Y += 50.f;
			cam->setPosition( newPos );

			driver->endScene();
			/*newPos.X = m_Spheres[SPH - 1]->getIrrNode()->getPosition().X;
			newPos.Y = m_Spheres[SPH - 1]->getIrrNode()->getPosition().Y + 50.f;
			newPos.Z = m_Spheres[SPH - 1]->getIrrNode()->getPosition().Z - 50.f;
			cam->setPosition(newPos);*/
			if( m_Spheres[0] != NULL)
			{
				for(int i = 0; i < SPH; ++i)
					m_Spheres[i]->updatePos();
			}
			//if( this->m_pxNode != NULL)
			//	this->m_pxNode->updatePosCustomNode();
			if(create)
				for(int j = 0; j < m_vCPxObj.size(); ++j)
					m_vCPxObj[j]->updatePos();
		}
		return true;
	}
};
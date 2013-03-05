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
	static const unsigned int SPH = 2;//
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
	irr::core::matrix4 matrixForPhysX;
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
		files->addFolderFileArchive("D:\\media");
		for( int i = 0; i < SPH; ++i )
			m_Spheres[i] = NULL;
		//Prototype( vector3df(1) );
		addScene();
		addObject(100, vector3df(0,10.f,10.f));
		addPhysXObject();
		//----------------BOX----------------------------
		addBlock();
		/////////////////////////////////////////////////
		//addChassis();
		addSpheres();
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
	virtual void Prototype(vector3df scale)
	{
		scene::IAnimatedMesh* mesh = smgr->getMesh("D:\\media\\cube.obj");
		s32 meshBufferCount = mesh->getMesh(0)->getMeshBufferCount();
		core::array<PxVec3> vertices;
		core::array<PxU32> indices;

		PxU32 tempIndexCount = 0;
		for( int i = 0; i < meshBufferCount; ++i)
		{
			scene::IMeshBuffer* mb = mesh->getMesh(0)->getMeshBuffer(i);
			s32 numVertices = mb->getVertexCount();
			s32 numIndices = mb->getIndexCount();

			video::S3DVertex2TCoords* mbVertices = (video::S3DVertex2TCoords*)mb->getVertices();
			u16* mbIndices = mb->getIndices();
			for( int j = 0; j < numVertices; ++j)
				vertices.push_back(PxVec3(mbVertices[j].Pos.X * scale.X, mbVertices[j].Pos.Y * scale.Y, mbVertices[j].Pos.Z * scale.Z));
			for( int k = 0; k < numIndices; ++k)
				indices.push_back(PxU32(mbIndices[k]) + tempIndexCount);
			tempIndexCount += numVertices;
		}
		mute->mTrialMeshDesc = new PxTriangleMeshDesc();
		mute->mTrialMeshDesc->points.count = vertices.size();
		mute->mTrialMeshDesc->triangles.count = indices.size() / 3;
		mute->mTrialMeshDesc->points.stride = sizeof(PxVec3);
		mute->mTrialMeshDesc->triangles.stride = 3*sizeof(PxVec3);
		mute->mTrialMeshDesc->points.data = vertices.const_pointer();
		mute->mTrialMeshDesc->triangles.data = indices.const_pointer();
		mute->mTrialMeshDesc->flags = PxMeshFlags::PxFlags(0);
		cout << endl << mute->mTrialMeshDesc->points.count << " Vertices.\n";
		cout << indices.size() << " indices.\n";
		cout << mute->mTrialMeshDesc->triangles.count << " triangles.\n";

		PxToolkit::MemoryOutputStream buf;
		mute->getCooking().cookTriangleMesh( *mute->mTrialMeshDesc, buf);
		PxToolkit::MemoryInputData readBuffer(buf.getData(),buf.getSize());
		mute->mTrialMesh = mute->getPhysX().createTriangleMesh(readBuffer);
		mute->mTrialGeom = new PxTriangleMeshGeometry();
		mute->mTrialGeom->triangleMesh = mute->mTrialMesh;
		PxRigidDynamic* ac = mute->getPhysX().createRigidDynamic( PxTransform() );
		PxShape* sh = ac->createShape( *mute->mTrialGeom, mute->getMaterial(), PxTransform( PxVec3(0,10,0) ) );
		mute->Scene().addActor( *ac );
	}
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
			m_Spheres[i]->getIrrNode()->setMaterialFlag( irr::video::EMF_LIGHTING, false);
			//m_Spheres[i]->getIrrNode()->setMaterialTexture(0, driver->getTexture("wall.bmp") );
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
	//If you want to add a Block just call this method
	//Parameter 1 = position
	//Parameter 2 = rotation
	//When placed in a smart way can create wall or a column
	//JaworekP1ay ^_^
	virtual bool addBlock(vector3df& position = vector3df(0), vector3df& rotation = vector3df(0))
	{
		matrixForPhysX;
		//----------------------------BASE----------------------------------------
		pyramid[0] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[0]->setPosition(			position );
		pyramid[0]->setRotation(			rotation + vector3df(0,45,0));
		pyramid[0]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );

		//vector3df(0,180,0)
		pyramid[1] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[1]->setPosition(			position + vector3df(0,0,0));
		pyramid[1]->setRotation(			vector3df(0,225,0));
		pyramid[1]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );
		//////////////////////////////////////////////////////////////////////////



		//----------------------------TOP-----------------------------------------
		pyramid[2] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[2]->setPosition(			position + vector3df(0,14,0) );
		pyramid[2]->setRotation(			vector3df(180,45,0) );
		pyramid[2]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );

		pyramid[3] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[3]->setPosition(			position + vector3df(0,14,0) );
		pyramid[3]->setRotation(			vector3df(180,225,0));
		pyramid[3]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );
		//////////////////////////////////////////////////////////////////////////



		//----------------------------RIGHT---------------------------------------
		pyramid[4] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[4]->setPosition(			position + vector3df(7,7,0) );
		pyramid[4]->setRotation(			vector3df(0,45,90));
		pyramid[4]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );

		pyramid[5] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[5]->setPosition(			position + vector3df(7,7,0) );
		pyramid[5]->setRotation(			vector3df(0,225,90));
		pyramid[5]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );
		//////////////////////////////////////////////////////////////////////////



		//----------------------------LEFT----------------------------------------
		pyramid[6] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[6]->setPosition(			position + vector3df(-7,7,0) );
		pyramid[6]->setRotation(			vector3df(180,45,90));
		pyramid[6]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );

		pyramid[7] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[7]->setPosition(			position + vector3df(-7,7,0) );
		pyramid[7]->setRotation(			vector3df(180,225,90));
		pyramid[7]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );
		//////////////////////////////////////////////////////////////////////////




		//----------------------------FRONT---------------------------------------
		pyramid[8] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[8]->setPosition(			position + vector3df(0,7,-7) );
		pyramid[8]->setRotation(			vector3df(90,0,45));
		pyramid[8]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );

		pyramid[9] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[9]->setPosition(			position + vector3df(0,7,-7) );
		pyramid[9]->setRotation(			vector3df(90,0,225));
		pyramid[9]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );
		//////////////////////////////////////////////////////////////////////////





		//----------------------------BACK----------------------------------------
		pyramid[10] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[10]->setPosition(			position + vector3df(0,7,7) );
		pyramid[10]->setRotation(			vector3df(-90,0,45));
		pyramid[10]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );

		pyramid[11] = new CCustomNode(		smgr->getRootSceneNode(), smgr,4,-1);
		pyramid[11]->setPosition(			position + vector3df(0,7,7) );
		pyramid[11]->setRotation(			vector3df(-90,0,-135));
		pyramid[11]->setMaterialTexture(		0,driver->getTexture("D:\\media\\wall.bmp") );
		//////////////////////////////////////////////////////////////////////////

		return true;
	}
	virtual bool addCamera()
	{
		cam = smgr->addCameraSceneNodeFPS(smgr->getRootSceneNode(), 100.f, 0.02f);
		cam->setPosition( irr::core::vector3df( 0.f, 40.f, 70.f ) );
		cam->setTarget( vector3df(0) );
		cam->setFarValue( 10000.f );
		//cam->setTarget(m_Spheres[SPH - 1]->getIrrNode()->getPosition());
		return true;
	}
	virtual bool addPhysXObject()
	{
		mute->CreatePyramid(PxVec3(0));
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
		vector3df rot = pyramid[5]->getRotation();
		while(device->run())
		{
			mute->advance(0.5f);
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
				str += L" Position X: ";
				str += rot.X;
				str += L", Y = ";
				str += rot.Y;
				str += L", Z = ";
				str += rot.Z;

				device->setWindowCaption(str.c_str());
				frames=0;


			/*if( rec->isKeyDown( irr::KEY_KEY_A ) )
				mute->moveLeft();
			if( rec->isKeyDown( irr::KEY_KEY_D ) )
				mute->moveRight();
			if( rec->isKeyDown( irr::KEY_KEY_W ) )
				mute->moveForward();
			if( rec->isKeyDown( irr::KEY_KEY_S ) )
				mute->moveIn();
			if( rec->isKeyDown( irr::KEY_SPACE ) )
				mute->Jump();*/
			
			if( rec->isKeyDown( irr::KEY_KEY_R ) )
			{
				cam->setPosition( vector3df(0.f,0.f,-100.f) );
				cam->setTarget( vector3df(0) );
				rot = vector3df(0,0,0);
				pyramid[5]->setRotation( rot );
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
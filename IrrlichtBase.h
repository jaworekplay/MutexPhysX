#pragma once
#include <irrlicht.h>
#include "MutexPhysX.h"
#include <iostream>

using namespace irr;
using namespace std;

class CIrrlichtBase
{
private:
	ICustomEventReceiver* rec;
	irr::core::dimension2d<irr::u32> size;
	irr::video::E_DRIVER_TYPE driverType;
	irr::IrrlichtDevice* device;
	irr::scene::IMesh* mesh;
	irr::scene::ISceneNode* m_rootNode;
	irr::core::vector3df m_pos;
	irr::core::vector3df m_rot;
	irr::core::vector3df m_scale;
	std::vector<int> m_vNPC;
	std::vector<int> m_vPxNPC;
	std::vector<int> m_vObj;
	std::vector<int> m_vPxObj;
	CMutex* mute;
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
			smgr->addMeshSceneNode(mesh,m_rootNode,id,m_pos,m_rot,m_scale);
		else
			smgr->addMeshSceneNode(mesh,m_rootNode,id,Position,m_rot,m_scale);
		m_vObj.push_back(id);
		return true;
	}
	virtual bool addPhysXObject(){return true;}
	virtual bool addPlayerCharacter(){return true;}
	virtual bool render()
	{
		while(device->run())
		{
			mute->advance(0.05f);
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
			m_pxNode->updatePos();
		}
		return true;
	}
};